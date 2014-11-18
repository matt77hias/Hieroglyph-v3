//--------------------------------------------------------------------------------
// This file is a portion of the Hieroglyph 3 Rendering Engine.  It is distributed
// under the MIT License, available in the root of this distribution and 
// at the following URL:
//
// http://www.opensource.org/licenses/mit-license.php
//
// Copyright (c) Jason Zink 
//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
#include "RiftHMD.h"
#include <stdexcept>

#include "OVR_CAPI.h"

// Configure D3D11 as the Rendering API used, which is then also used for API
// specific distortion rendering.
#define OVR_D3D_VERSION 11
#include "OVR_CAPI_D3D.h"
//--------------------------------------------------------------------------------

#include "Kernel/OVR_Math.h"
#include "OVR_Stereo.h"
#include "RendererDX11.h"
#include "SwapChainDX11.h"
#include "Texture2dDX11.h"
#include "RenderTargetViewDX11.h"
#include "ShaderResourceViewDX11.h"
#include "Log.h"
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
struct RiftHMD::Impl
{
	Impl( RiftManagerPtr RiftMgr ) : 
		m_RiftMgr( RiftMgr ),
		m_hmd( nullptr ),
		frame( 0 )
	{
		// Create the first device available.  If it isn't available then we throw an
		// exception to let the user know.

		m_hmd = ovrHmd_Create(0);

		if ( !m_hmd ) {
			Log::Get().Write( L"Unable to find hardware Rift device, creating a debug device instead..." );
			m_hmd = ovrHmd_CreateDebug( ovrHmd_DK2 );
		}
		
		if ( !m_hmd ) throw std::invalid_argument( "No HMD Devices were found!" );
	
		// Start the sensor which provides the Rift’s pose and motion.
		ovrHmd_ConfigureTracking( m_hmd, 
			ovrTrackingCap_Orientation |
			ovrTrackingCap_MagYawCorrection | 
			ovrTrackingCap_Position, 0);

		ovrHmdDesc hmdDesc = *m_hmd;

		// Initialize FovSideTanMax, which allows us to change all Fov sides at once - Fov
		// starts at default and is clamped to this value.
		float FovSideTanLimit = OVR::FovPort::Max( hmdDesc.MaxEyeFov[0], hmdDesc.MaxEyeFov[1] ).GetMaxSideTan();
		float FovSideTanMax   = OVR::FovPort::Max( hmdDesc.DefaultEyeFov[0], hmdDesc.DefaultEyeFov[1] ).GetMaxSideTan();

		// Clamp Fov based on our dynamically adjustable FovSideTanMax.
		// Most apps should use the default, but reducing Fov does reduce rendering cost.
		eyeFov[0] = OVR::FovPort::Min( hmdDesc.DefaultEyeFov[0], OVR::FovPort(FovSideTanMax) );
		eyeFov[1] = OVR::FovPort::Min( hmdDesc.DefaultEyeFov[1], OVR::FovPort(FovSideTanMax) );
	}
	//--------------------------------------------------------------------------------
	~Impl()
	{
		if ( m_hmd ) {
			ovrHmd_Destroy( m_hmd );
			m_hmd = nullptr;
		}
	}
	//--------------------------------------------------------------------------------
	unsigned int HmdDisplayWidth()
	{
		ovrHmdDesc hmdDesc = *m_hmd;

		return hmdDesc.Resolution.w;
	}
	//--------------------------------------------------------------------------------
	unsigned int HmdDisplayHeight()
	{
		ovrHmdDesc hmdDesc = *m_hmd;

		return hmdDesc.Resolution.h;
	}
	//--------------------------------------------------------------------------------
	unsigned int DesiredEyeTextureWidth()
	{
		// Get the desired texture sizes for the render targets.  Note that these are
		// typically larger than the resolution of the display panel itself.

		ovrHmdDesc hmdDesc = *m_hmd;

		OVR::Sizei tex0Size = ovrHmd_GetFovTextureSize( m_hmd, ovrEye_Left, hmdDesc.DefaultEyeFov[ovrEye_Left], 1.0f );
		OVR::Sizei tex1Size = ovrHmd_GetFovTextureSize( m_hmd, ovrEye_Right, hmdDesc.DefaultEyeFov[ovrEye_Right], 1.0f );

		return max( tex0Size.w, tex1Size.w );
	}
	//--------------------------------------------------------------------------------
	unsigned int DesiredEyeTextureHeight()
	{
		// Get the desired texture sizes for the render targets.  Note that these are
		// typically larger than the resolution of the display panel itself.

		ovrHmdDesc hmdDesc = *m_hmd;

		OVR::Sizei tex0Size = ovrHmd_GetFovTextureSize( m_hmd, ovrEye_Left, hmdDesc.DefaultEyeFov[ovrEye_Left], 1.0f );
		OVR::Sizei tex1Size = ovrHmd_GetFovTextureSize( m_hmd, ovrEye_Right, hmdDesc.DefaultEyeFov[ovrEye_Right], 1.0f );

		return max( tex0Size.h, tex1Size.h );
	}
	//--------------------------------------------------------------------------------
	void ReadEyeData()
	{
		// Grab the two eye offsets in a contiguous array.
		ovrVector3f offsets[2];
		offsets[0] = eyeRenderDesc[0].HmdToEyeViewOffset;
		offsets[1] = eyeRenderDesc[1].HmdToEyeViewOffset;

		// Read out the state, and store the results in our member variables for use
		// later on.
		// TODO: Track the state and take default action if it is not tracking!

		ovrTrackingState ts;
		ovrHmd_GetEyePoses( m_hmd, frame, offsets, eyePose, &ts );
	}
	//--------------------------------------------------------------------------------
	Matrix3f GetOrientation( double time )
	{
		// Query the HMD for the sensor state at a given time. "0.0" means "most recent time".
		ovrTrackingState ts = ovrHmd_GetTrackingState( m_hmd, 0.0 );

		Matrix3f orientation;
		orientation.MakeIdentity();

		if ( ts.StatusFlags & ( ovrStatus_OrientationTracked | ovrStatus_PositionTracked ) )
		{
			float yaw, eyePitch, eyeRoll = 0.0f;
		
			// Get the pos of the head, and then extract the rotation angles
			OVR::Posef pose = ts.HeadPose.ThePose;
			pose.Rotation.GetEulerAngles<OVR::Axis_Y, OVR::Axis_X, OVR::Axis_Z>(&yaw, &eyePitch, &eyeRoll);
			
			// Apply those angles to our rotation matrix
			orientation.Rotation( Vector3f( eyePitch, yaw, -eyeRoll ) );
		}

		return( orientation );
	}
	//--------------------------------------------------------------------------------
	Matrix4f GetPerspectiveFov( unsigned int eye, float zn, float zf  )
	{
		// This method is based on the perspective matrix in the Oculus SDK.  It is 
		// reimplemented here to allow direct initialization of the Hieroglyph 3 
		// specific matrix object.

		Matrix4f p;

		float xScale = 2.0f / ( eyeFov[eye].LeftTan + eyeFov[eye].RightTan );
		float xOffset = ( eyeFov[eye].LeftTan - eyeFov[eye].RightTan ) * xScale * 0.5f;

		float yScale = 2.0f / ( eyeFov[eye].UpTan + eyeFov[eye].DownTan );
		float yOffset = ( eyeFov[eye].UpTan - eyeFov[eye].DownTan ) * yScale * 0.5f;

		// row 0
		p[ 0] = xScale;
		p[ 1] = 0.0f;
		p[ 2] = 0.0f;
		p[ 3] = 0.0f;

		// row 1
		p[ 4] = 0.0f;
		p[ 5] = yScale;
		p[ 6] = 0.0f;
		p[ 7] = 0.0f;

		// row 2
		p[ 8] = xOffset;
		p[ 9] = -yOffset;
		p[10] = zf / ( zf-zn );
		p[11] = 1.0f;

		// row 3
		p[12] = 0.0f;
		p[13] = 0.0f;
		p[14] = -zn*zf / ( zf-zn );
		p[15] = 0.0f;

		return p;
	}
	//--------------------------------------------------------------------------------
	Matrix4f GetEyeTranslation( unsigned int eye )
	{
		return Matrix4f::TranslationMatrix( eyeRenderDesc[eye].HmdToEyeViewOffset.x,
											eyeRenderDesc[eye].HmdToEyeViewOffset.y,
											eyeRenderDesc[eye].HmdToEyeViewOffset.z );
	}
	//--------------------------------------------------------------------------------
	Matrix4f GetEyeSpatialState( unsigned int eye )
	{
		// This method call was deprecated in OculusSDK 0.4.3.  Instead, the eye
		// pose is read out from the API using RiftHMD::ReadEyeData() which collects
		// the information for both eyes simultaneously.

		//eyePose[eye] = ovrHmd_GetEyePose( m_hmd, static_cast<ovrEyeType>(eye) );
		ReadEyeData();

		float yaw, eyePitch, eyeRoll = 0.0f;
		OVR::Posef pose = eyePose[eye];
		pose.Rotation.GetEulerAngles<OVR::Axis_Y, OVR::Axis_X, OVR::Axis_Z>(&yaw, &eyePitch, &eyeRoll);
			
		// Apply those angles to our rotation matrix
		Matrix4f orientation = Matrix4f::RotationMatrixXYZ( eyePitch, yaw, -eyeRoll );
		Matrix4f translation = Matrix4f::TranslationMatrix( -5.0f*pose.Translation.x, -5.0f*pose.Translation.y, 5.0f*pose.Translation.z );

		return translation * orientation;
	}
	//--------------------------------------------------------------------------------
	float BeginFrame()
	{
		// Take care of the health and safety warning
		ovrHSWDisplayState hswDisplayState;
		ovrHmd_GetHSWDisplayState(m_hmd, &hswDisplayState);

		if (hswDisplayState.Displayed) {
			ovrHmd_DismissHSWDisplay(m_hmd);
		}

		frame++;

		ovrFrameTiming frameTiming = ovrHmd_BeginFrame( m_hmd, frame );

		return( frameTiming.DeltaSeconds );
	}
	//--------------------------------------------------------------------------------
	void EndFrame()
	{
		ovrHmd_EndFrame( m_hmd, eyePose, eyeTexture );
	}
	//--------------------------------------------------------------------------------
	void ConfigureRendering( const ResourcePtr& renderTarget, int swapchain )
	{
		DXGI_SWAP_CHAIN_DESC swapDesc;
		RendererDX11::Get()->GetSwapChainByIndex( swapchain )->GetSwapChain()->GetDesc( &swapDesc );

		OVR::Sizei size( swapDesc.BufferDesc.Width, swapDesc.BufferDesc.Height );

		ovrD3D11Config cfg;
		cfg.D3D11.Header.API = ovrRenderAPI_D3D11;
		cfg.D3D11.Header.RTSize = size;
		cfg.D3D11.Header.Multisample = 1;
		cfg.D3D11.pDevice = RendererDX11::Get()->GetDevice();
		cfg.D3D11.pDeviceContext = RendererDX11::Get()->pImmPipeline->GetDeviceContext();
		cfg.D3D11.pBackBufferRT = RendererDX11::Get()->GetRenderTargetViewByIndex( renderTarget->m_iResourceRTV ).GetRTV();
		cfg.D3D11.pSwapChain = RendererDX11::Get()->GetSwapChainByIndex( swapchain )->GetSwapChain();

		// TODO: Time warp seems to introduce a stuttering during the rendering, which 
		//       is clearly not correct.  This is probably due to how I am using the 
		//       API, but it is disabled here until I can figure out why...
		const unsigned   DistortionCaps = ovrDistortionCap_Chromatic | ovrDistortionCap_Overdrive; // | ovrDistortionCap_TimeWarp;

		if ( !ovrHmd_ConfigureRendering( m_hmd, &cfg.Config, DistortionCaps, eyeFov, eyeRenderDesc ) ) {
			throw std::invalid_argument( "Couldn't configure rift HMD rendering!" );
		}

	}
	//--------------------------------------------------------------------------------
	void ConfigureEyeTexture( unsigned int eye, const ResourcePtr& texture )
	{
		assert( eye == 0 || eye == 1 );

		// Get the size of the passed in texture.
		D3D11_TEXTURE2D_DESC desc = RendererDX11::Get()->GetTexture2DByIndex( texture->m_iResource )->GetActualDescription();

		((ovrD3D11Texture*)eyeTexture)[eye].D3D11.Header.API = ovrRenderAPI_D3D11;
		((ovrD3D11Texture*)eyeTexture)[eye].D3D11.Header.TextureSize = OVR::Sizei( desc.Width, desc.Height );
		((ovrD3D11Texture*)eyeTexture)[eye].D3D11.Header.RenderViewport = OVR::Recti( 0, 0, desc.Width, desc.Height );
		((ovrD3D11Texture*)eyeTexture)[eye].D3D11.pTexture = static_cast<ID3D11Texture2D*>( RendererDX11::Get()->GetTexture2DByIndex( texture->m_iResource )->GetResource() );
		((ovrD3D11Texture*)eyeTexture)[eye].D3D11.pSRView = RendererDX11::Get()->GetShaderResourceViewByIndex( texture->m_iResourceSRV ).GetSRV();
	}
	//--------------------------------------------------------------------------------
	void AttachToWindow( HWND window )
	{
		ovrHmd_AttachToWindow( m_hmd, window, nullptr, nullptr );
	}
	//--------------------------------------------------------------------------------
	RiftManagerPtr m_RiftMgr;
	ovrHmd m_hmd;
	ovrFovPort eyeFov[2];
	ovrEyeRenderDesc eyeRenderDesc[2];
	ovrTexture eyeTexture[2];
	ovrPosef eyePose[2];
	unsigned int frame;
};
//--------------------------------------------------------------------------------



//--------------------------------------------------------------------------------
RiftHMD::RiftHMD( RiftManagerPtr RiftMgr ) : 
	m_pImpl( new Impl( RiftMgr ) )	
{
}
//--------------------------------------------------------------------------------
RiftHMD::~RiftHMD()
{
	delete m_pImpl;
}
//--------------------------------------------------------------------------------
unsigned int RiftHMD::HmdDisplayWidth()
{
	return m_pImpl->HmdDisplayWidth();
}
//--------------------------------------------------------------------------------
unsigned int RiftHMD::HmdDisplayHeight()
{
	return m_pImpl->HmdDisplayHeight();
}
//--------------------------------------------------------------------------------
unsigned int RiftHMD::DesiredEyeTextureWidth()
{
	return m_pImpl->DesiredEyeTextureWidth();
}
//--------------------------------------------------------------------------------
unsigned int RiftHMD::DesiredEyeTextureHeight()
{
	return m_pImpl->DesiredEyeTextureHeight();
}
//--------------------------------------------------------------------------------
void RiftHMD::ReadEyeData()
{
	return m_pImpl->ReadEyeData();
}
//--------------------------------------------------------------------------------
Matrix3f RiftHMD::GetOrientation( double time )
{
	return m_pImpl->GetOrientation( time );
}
//--------------------------------------------------------------------------------
Matrix4f RiftHMD::GetPerspectiveFov( unsigned int eye, float zn, float zf  )
{
	return m_pImpl->GetPerspectiveFov( eye, zn, zf );
}
//--------------------------------------------------------------------------------
Matrix4f RiftHMD::GetEyeTranslation( unsigned int eye )
{
	return m_pImpl->GetEyeTranslation( eye );
}
//--------------------------------------------------------------------------------
Matrix4f RiftHMD::GetEyeSpatialState( unsigned int eye )
{
	return m_pImpl->GetEyeSpatialState( eye );
}
//--------------------------------------------------------------------------------
float RiftHMD::BeginFrame()
{
	return m_pImpl->BeginFrame();
}
//--------------------------------------------------------------------------------
void RiftHMD::EndFrame()
{
	return m_pImpl->EndFrame();
}
//--------------------------------------------------------------------------------
void RiftHMD::ConfigureRendering( const ResourcePtr& renderTarget, int swapchain )
{
	return m_pImpl->ConfigureRendering( renderTarget, swapchain );
}
//--------------------------------------------------------------------------------
void RiftHMD::ConfigureEyeTexture( unsigned int eye, const ResourcePtr& texture )
{
	return m_pImpl->ConfigureEyeTexture( eye, texture );
}
//--------------------------------------------------------------------------------
void RiftHMD::AttachToWindow( HWND window )
{
	return m_pImpl->AttachToWindow( window );
}
//--------------------------------------------------------------------------------