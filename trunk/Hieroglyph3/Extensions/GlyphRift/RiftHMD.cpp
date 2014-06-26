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
		m_hmd( nullptr )
	{
		// Create the first device available.  If it isn't available then we throw an
		// exception to let the user know.

		m_hmd = ovrHmd_Create(0);

		if ( !m_hmd ) {
			Log::Get().Write( L"Unable to find hardware Rift device, creating a debug device instead..." );
			m_hmd = ovrHmd_CreateDebug( ovrHmd_DK1 );
		}
		
		if ( !m_hmd ) throw std::invalid_argument( "No HMD Devices were found!" );
	
		// Start the sensor which provides the Rift’s pose and motion.
		ovrHmd_StartSensor( m_hmd, ovrSensorCap_Orientation |
								ovrSensorCap_YawCorrection |
								ovrSensorCap_Position, 0 );

		ovrHmdDesc hmdDesc;
		ovrHmd_GetDesc( m_hmd, &hmdDesc );

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
		ovrHmdDesc hmdDesc;
		ovrHmd_GetDesc( m_hmd, &hmdDesc );

		return hmdDesc.Resolution.w;
	}
	//--------------------------------------------------------------------------------
	unsigned int HmdDisplayHeight()
	{
		ovrHmdDesc hmdDesc;
		ovrHmd_GetDesc( m_hmd, &hmdDesc );

		return hmdDesc.Resolution.h;
	}
	//--------------------------------------------------------------------------------
	unsigned int DesiredEyeTextureWidth()
	{
		// Get the desired texture sizes for the render targets.  Note that these are
		// typically larger than the resolution of the display panel itself.

		ovrHmdDesc hmdDesc;
		ovrHmd_GetDesc( m_hmd, &hmdDesc );

		OVR::Sizei tex0Size = ovrHmd_GetFovTextureSize( m_hmd, ovrEye_Left, hmdDesc.DefaultEyeFov[ovrEye_Left], 1.0f );
		OVR::Sizei tex1Size = ovrHmd_GetFovTextureSize( m_hmd, ovrEye_Right, hmdDesc.DefaultEyeFov[ovrEye_Right], 1.0f );

		return max( tex0Size.w, tex1Size.w );
	}
	//--------------------------------------------------------------------------------
	unsigned int DesiredEyeTextureHeight()
	{
		// Get the desired texture sizes for the render targets.  Note that these are
		// typically larger than the resolution of the display panel itself.

		ovrHmdDesc hmdDesc;
		ovrHmd_GetDesc( m_hmd, &hmdDesc );

		OVR::Sizei tex0Size = ovrHmd_GetFovTextureSize( m_hmd, ovrEye_Left, hmdDesc.DefaultEyeFov[ovrEye_Left], 1.0f );
		OVR::Sizei tex1Size = ovrHmd_GetFovTextureSize( m_hmd, ovrEye_Right, hmdDesc.DefaultEyeFov[ovrEye_Right], 1.0f );

		return max( tex0Size.h, tex1Size.h );
	}
	//--------------------------------------------------------------------------------
	Matrix3f GetOrientation( double time )
	{
		// Query the HMD for the sensor state at a given time. "0.0" means "most recent time".
		ovrSensorState ss = ovrHmd_GetSensorState( m_hmd, 0.0 );

		Matrix3f orientation;
		orientation.MakeIdentity();

		if ( ss.StatusFlags & ( ovrStatus_OrientationTracked | ovrStatus_PositionTracked ) )
		{
			float yaw, eyePitch, eyeRoll = 0.0f;
		
			// Get the most recent pose information from the sensor state
			OVR::Transformf movePose = ss.Recorded.Pose; //ss.Predicted.Pose;
		
			// Get the Euler angles from teh quaternion
			movePose.Rotation.GetEulerAngles< OVR::Axis_Y, OVR::Axis_X, OVR::Axis_Z, OVR::Rotate_CCW, OVR::Handed_L >( &yaw, &eyePitch, &eyeRoll );
	
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
		return Matrix4f::TranslationMatrix( eyeRenderDesc[eye].ViewAdjust.x,
											eyeRenderDesc[eye].ViewAdjust.y,
											eyeRenderDesc[eye].ViewAdjust.z );
	}
	//--------------------------------------------------------------------------------
	float BeginFrame()
	{
		ovrFrameTiming frameTiming = ovrHmd_BeginFrame( m_hmd, 0 );

		return( frameTiming.DeltaSeconds );
	}
	//--------------------------------------------------------------------------------
	void BeginEyeRender( unsigned int eye )
	{
		assert( eye == 0 || eye == 1 );
		eyePose[eye] = ovrHmd_BeginEyeRender( m_hmd, static_cast<ovrEyeType>( eye ) );
	}
	//--------------------------------------------------------------------------------
	void EndEyeRender( unsigned int eye )
	{
		ovrHmd_EndEyeRender( m_hmd, static_cast<ovrEyeType>( eye ), eyePose[eye], &eyeTexture[eye].Texture);
	}
	//--------------------------------------------------------------------------------
	void EndFrame()
	{
		ovrHmd_EndFrame( m_hmd );
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

		const unsigned   DistortionCaps = ovrDistortionCap_Chromatic | ovrDistortionCap_TimeWarp;

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

		eyeTexture[eye].D3D11.Header.API = ovrRenderAPI_D3D11;
		eyeTexture[eye].D3D11.Header.TextureSize = OVR::Sizei( desc.Width, desc.Height );
		eyeTexture[eye].D3D11.Header.RenderViewport = OVR::Recti( 0, 0, desc.Width, desc.Height );
		eyeTexture[eye].D3D11.pTexture = static_cast<ID3D11Texture2D*>( RendererDX11::Get()->GetTexture2DByIndex( texture->m_iResource )->GetResource() );
		eyeTexture[eye].D3D11.pSRView = RendererDX11::Get()->GetShaderResourceViewByIndex( texture->m_iResourceSRV ).GetSRV();
	}
	//--------------------------------------------------------------------------------
	RiftManagerPtr m_RiftMgr;
	ovrHmd m_hmd;
	ovrFovPort eyeFov[2];
	ovrEyeRenderDesc eyeRenderDesc[2];
	ovrD3D11Texture eyeTexture[2];
	ovrPosef eyePose[2];
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
float RiftHMD::BeginFrame()
{
	return m_pImpl->BeginFrame();
}
//--------------------------------------------------------------------------------
void RiftHMD::BeginEyeRender( unsigned int eye )
{
	return m_pImpl->BeginEyeRender( eye );
}
//--------------------------------------------------------------------------------
void RiftHMD::EndEyeRender( unsigned int eye )
{
	return m_pImpl->EndEyeRender( eye );
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