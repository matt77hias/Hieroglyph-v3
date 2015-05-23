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
#include "GlyphRift/RiftHMD.h"
#include <stdexcept>

// Include the Oculus SDK
#define   OVR_D3D_VERSION 11
#include "OVR_CAPI_D3D.h"
//--------------------------------------------------------------------------------

#include "Extras/OVR_Math.h"
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
		frame( 0 ),
		hardwareDevice( true ),
		mirrorTexture( nullptr )
	{
		// Create the first device available.  If it isn't available then we throw an
		// exception to let the user know.

		ovrResult result = ovrHmd_Create( 0, &m_hmd );

		if ( !m_hmd ) {
			Log::Get().Write( L"Unable to find hardware Rift device, creating a debug device instead..." );
			hardwareDevice = false;
			result = ovrHmd_CreateDebug( ovrHmd_DK2, &m_hmd );
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

		// Initialize the frame time for future delta calculations.
		frame_time = ovr_GetTimeInSeconds();
	}
	//--------------------------------------------------------------------------------
	~Impl()
	{
		if ( mirrorTexture != nullptr ) {
			ovrHmd_DestroyMirrorTexture( m_hmd, mirrorTexture );
		}

		if ( m_hmd ) {
			ovrHmd_Destroy( m_hmd );
			m_hmd = nullptr;
		}
	}
	//--------------------------------------------------------------------------------
	bool IsHardwareDevice()
	{
		return hardwareDevice;
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
        // Get both eye poses simultaneously, with IPD offset already included. 
        ovrVector3f      HmdToEyeViewOffset[2] = { eyeRenderDesc[0].HmdToEyeViewOffset,
                                                   eyeRenderDesc[1].HmdToEyeViewOffset };

		ovrFrameTiming   ftiming  = ovrHmd_GetFrameTiming(m_hmd, 0);
        ovrTrackingState hmdState = ovrHmd_GetTrackingState(m_hmd, ftiming.DisplayMidpointSeconds);
        
		ovr_CalcEyePoses( hmdState.HeadPose.ThePose, HmdToEyeViewOffset, eyePose );

		// Update the layer information with our new pose information.

		for (int eye = 0; eye < 2; eye++)
        {
            //ld.ColorTexture[eye] = pEyeRenderTexture[eye]->TextureSet;
            //ld.Viewport[eye]     = eyeRenderViewport[eye];
            //ld.Fov[eye]          = HMD->DefaultEyeFov[eye];
            layer.RenderPose[eye]   = eyePose[eye];
        }

	}
	//--------------------------------------------------------------------------------
	Matrix3f GetOrientation( double time )
	{
		// Query the HMD for the sensor state at a given time. "0.0" means "most recent time".
		ovrTrackingState ts = ovrHmd_GetTrackingState( m_hmd, time );

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
		// The eye pose is read out from the API using RiftHMD::ReadEyeData() which 
		// collects the information for both eyes simultaneously.

		ReadEyeData();

		float yaw, eyePitch, eyeRoll = 0.0f;
		OVR::Posef pose = eyePose[eye];
		
		pose.Rotation.GetEulerAngles<OVR::Axis_Z, OVR::Axis_X, OVR::Axis_Y>(&eyeRoll, &eyePitch, &yaw);

		// Apply those angles to our rotation matrix

		Matrix4f orientation = 
			Matrix4f::RotationMatrixZ( -eyeRoll ) *
			Matrix4f::RotationMatrixX( eyePitch ) *
			Matrix4f::RotationMatrixY( yaw );
		Matrix4f translation = Matrix4f::TranslationMatrix( -5.0f*pose.Translation.x, -5.0f*pose.Translation.y, 5.0f*pose.Translation.z );

		return translation * orientation;
	}
	//--------------------------------------------------------------------------------
	float BeginFrame()
	{
		// Increment the frame number, and then read the predicted time for the next
		// frame.  We store the predicted frame time for calculating the delta time
		// used in the simulation.

		frame++;

		ovrFrameTiming frameTiming = ovrHmd_GetFrameTiming( m_hmd, frame );

		float delta = static_cast<float>( frameTiming.DisplayMidpointSeconds - frame_time );
		frame_time = frameTiming.DisplayMidpointSeconds;

		// Increment the texture slot to use, and automatically roll over the count
		// based on the number of slots available.

		pTextureSets[0]->CurrentIndex = (pTextureSets[0]->CurrentIndex + 1) % pTextureSets[0]->TextureCount;
		pTextureSets[1]->CurrentIndex = (pTextureSets[1]->CurrentIndex + 1) % pTextureSets[1]->TextureCount;

		return( delta );
	}
	//--------------------------------------------------------------------------------
	void EndFrame()
	{
		const unsigned int layer_count = 1;

		ovrLayerHeader* headers[layer_count] = { &layer.Header };
		ovrResult result = ovrHmd_SubmitFrame( m_hmd, frame, nullptr, headers, layer_count );
	}
	//--------------------------------------------------------------------------------
	void ConfigureRendering( unsigned int max_w, unsigned int max_h, DXGI_FORMAT format )
	{
		// Get the rendering description for each eye, based on an input FOV.
		// TODO: This FOV calculation could be modified to not use the default...

		eyeRenderDesc[0] = ovrHmd_GetRenderDesc( m_hmd, ovrEye_Left, m_hmd->DefaultEyeFov[0] );
		eyeRenderDesc[1] = ovrHmd_GetRenderDesc( m_hmd, ovrEye_Right, m_hmd->DefaultEyeFov[1] );

		// Get the desired texture sizes for the eye render targets.  Note that these 
		// are typically larger than the resolution of the HMD's display panel itself.

		unsigned int width = min( DesiredEyeTextureWidth(), max_w );
		unsigned int height = min( DesiredEyeTextureHeight(), max_h );


		// Create two render targets - one for each eye.  These are created with both
		// render target and shader resource bind points so that they can be used by
		// the Oculus SDK to do the distortion rendering at the end of the frame.

		pTextureSets[0] = nullptr;
		pTextureSets[1] = nullptr;

		Texture2dConfigDX11 renderConfig;
		renderConfig.SetWidth( width );
		renderConfig.SetHeight( height );
		renderConfig.SetFormat( format );
		renderConfig.SetBindFlags( D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE );
		D3D11_TEXTURE2D_DESC desc = renderConfig.GetTextureDesc();


		if ( ovrHmd_CreateSwapTextureSetD3D11( m_hmd, RendererDX11::Get()->GetDevice(), &desc, &pTextureSets[0] ) != ovrSuccess ) {
			Log::Get().Write( L"ERROR: Could not create swap texture sets!" );
		}

		if ( ovrHmd_CreateSwapTextureSetD3D11( m_hmd, RendererDX11::Get()->GetDevice(), &desc, &pTextureSets[1] ) != ovrSuccess ) {
			Log::Get().Write( L"ERROR: Could not create swap texture sets!" );
		}


		// Create render target views for each of the texture surfaces in our texture sets.
		// We just store those in a vector for reference later on, with one vector for
		// each eye.
		
		for ( unsigned int eye = 0; eye < 2; ++eye )
		{
			for ( int i = 0; i < pTextureSets[0]->TextureCount; ++i )
			{
				ovrD3D11Texture* tex = (ovrD3D11Texture*)&pTextureSets[eye]->Textures[i];
				ResourcePtr resource = RendererDX11::Get()->LoadTexture( tex->D3D11.pTexture );

				resource->m_iResourceRTV = 
					RendererDX11::Get()->CreateRenderTargetView(resource->m_iResource, nullptr );

				textureRTVs[eye].push_back( resource );
			}
		}

		// Initialize a single full screen FOV layer.
		
		layer.Header.Type = ovrLayerType_EyeFov;
		layer.Header.Flags = 0;
		layer.ColorTexture[0] = pTextureSets[0];
		layer.ColorTexture[1] = pTextureSets[1];
		layer.Fov[0] = eyeRenderDesc[0].Fov;
		layer.Fov[1] = eyeRenderDesc[1].Fov;
		layer.Viewport[0] = OVR::Recti(0, 0, width, height);
		layer.Viewport[1] = OVR::Recti(0, 0, width, height);
	}
	//--------------------------------------------------------------------------------
	ResourcePtr GetEyeTexture( unsigned int eye )
	{
		assert( eye == 0 || eye == 1 );

		return textureRTVs[eye][pTextureSets[eye]->CurrentIndex];
	}
	//--------------------------------------------------------------------------------
	ResourcePtr GetMirrorTexture( unsigned int w, unsigned int h, DXGI_FORMAT format )
	{
		// If the texture already exists, then check if it matches the right size.  If
		// it doesn't match, then delete it.
		if ( mirrorTexture != nullptr )
		{
			if ( mirrorTexture->Header.TextureSize.w != w || mirrorTexture->Header.TextureSize.h != h ) {
				ovrHmd_DestroyMirrorTexture( m_hmd, mirrorTexture );
				mirrorTexture = nullptr;
			}
		}

		// Ensure that an appropriately sized mirror texture is available.
		if ( mirrorTexture == nullptr )
		{
			D3D11_TEXTURE2D_DESC desc = { };
			desc.ArraySize        = 1;
			desc.Format           = format;
			desc.Width            = w;
			desc.Height           = h;
			desc.Usage            = D3D11_USAGE_DEFAULT;
			desc.SampleDesc.Count = 1;
			desc.MipLevels        = 1;
			ovrHmd_CreateMirrorTextureD3D11( m_hmd, RendererDX11::Get()->GetDevice(), &desc, &mirrorTexture );
		}

        ovrD3D11Texture* tex = (ovrD3D11Texture*)mirrorTexture;
		ResourcePtr resource = RendererDX11::Get()->LoadTexture( tex->D3D11.pTexture );

		return resource;
	}
	//--------------------------------------------------------------------------------
	RiftManagerPtr m_RiftMgr;
	ovrHmd m_hmd;
	ovrFovPort eyeFov[2];
	ovrEyeRenderDesc eyeRenderDesc[2];
	ovrPosef eyePose[2];
	
	ovrLayerEyeFov layer;
	ovrSwapTextureSet* pTextureSets[2];
	std::vector<ResourcePtr> textureRTVs[2];
	
	ovrTexture* mirrorTexture;

	unsigned int frame;
	double frame_time;
	bool hardwareDevice;
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
bool RiftHMD::IsHardwareDevice()
{
	return m_pImpl->IsHardwareDevice();
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
void RiftHMD::ConfigureRendering( unsigned int max_w, unsigned int max_h, DXGI_FORMAT format  )
{
	return m_pImpl->ConfigureRendering( max_w, max_h, format );
}
//--------------------------------------------------------------------------------
ResourcePtr RiftHMD::GetEyeTexture( unsigned int eye )
{
	return m_pImpl->GetEyeTexture( eye );
}
//--------------------------------------------------------------------------------
ResourcePtr RiftHMD::GetMirrorTexture( unsigned int w, unsigned int h, DXGI_FORMAT format )
{
	return m_pImpl->GetMirrorTexture( w, h, format );
}
//--------------------------------------------------------------------------------