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
#include "EvtErrorMessage.h"
#include "EventManager.h"
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
struct RiftHMD::Impl
{
	Impl( RiftManagerPtr RiftMgr ) : 
		m_RiftMgr( RiftMgr ),
		m_hmd( nullptr ),
		frame( 0 ),
		mirrorTexture( nullptr )
	{
		// Create the first device available.  If it isn't available then we throw an
		// exception to let the user know.

		ovrResult result = ovr_Create( &m_hmd, &m_luid );

		if ( !m_hmd ) {
			Log::Get().Write( L"Unable to find hardware Rift device!" );
			throw std::exception( "No HMD Devices were found!" );
		}
		
		ovrHmdDesc hmdDesc = ovr_GetHmdDesc( m_hmd );

		// Initialize FovSideTanMax, which allows us to change all Fov sides at once - Fov
		// starts at default and is clamped to this value.

		float FovSideTanLimit = OVR::FovPort::Max( hmdDesc.MaxEyeFov[0], hmdDesc.MaxEyeFov[1] ).GetMaxSideTan();
		float FovSideTanMax   = OVR::FovPort::Max( hmdDesc.DefaultEyeFov[0], hmdDesc.DefaultEyeFov[1] ).GetMaxSideTan();

		// Clamp Fov based on our dynamically adjustable FovSideTanMax.
		// Most apps should use the default, but reducing Fov does reduce rendering cost.
		
		eyeFov[0] = OVR::FovPort::Min( hmdDesc.DefaultEyeFov[0], OVR::FovPort(FovSideTanMax) );
		eyeFov[1] = OVR::FovPort::Min( hmdDesc.DefaultEyeFov[1], OVR::FovPort(FovSideTanMax) );

		// Initialize the frame time for future delta calculations.

		last_frame_time = ovr_GetTimeInSeconds();
	}
	//--------------------------------------------------------------------------------
	~Impl()
	{
		if ( mirrorTexture != nullptr ) {
			ovr_DestroyMirrorTexture( m_hmd, mirrorTexture );
		}

		for ( int eye = 0; eye < 2; ++eye ) {
			if ( textureSets[eye] != nullptr ) {
				ovr_DestroyTextureSwapChain( m_hmd, textureSets[eye] );
			}
		}

		if ( m_hmd ) {
			ovr_Destroy( m_hmd );
			m_hmd = nullptr;
		}
	}
	//--------------------------------------------------------------------------------
	unsigned int HmdDisplayWidth()
	{
		ovrHmdDesc hmdDesc = ovr_GetHmdDesc( m_hmd );

		return hmdDesc.Resolution.w;
	}
	//--------------------------------------------------------------------------------
	unsigned int HmdDisplayHeight()
	{
		ovrHmdDesc hmdDesc = ovr_GetHmdDesc( m_hmd );

		return hmdDesc.Resolution.h;
	}
	//--------------------------------------------------------------------------------
	unsigned int DesiredEyeTextureWidth()
	{
		// Get the desired texture sizes for the render targets.  Note that these are
		// typically larger than the resolution of the display panel itself.

		ovrHmdDesc hmdDesc = ovr_GetHmdDesc( m_hmd );

		OVR::Sizei tex0Size = ovr_GetFovTextureSize( m_hmd, ovrEye_Left, hmdDesc.DefaultEyeFov[ovrEye_Left], 1.0f );
		OVR::Sizei tex1Size = ovr_GetFovTextureSize( m_hmd, ovrEye_Right, hmdDesc.DefaultEyeFov[ovrEye_Right], 1.0f );

		return max( tex0Size.w, tex1Size.w );
	}
	//--------------------------------------------------------------------------------
	unsigned int DesiredEyeTextureHeight()
	{
		// Get the desired texture sizes for the render targets.  Note that these are
		// typically larger than the resolution of the display panel itself.

		ovrHmdDesc hmdDesc = ovr_GetHmdDesc( m_hmd );

		OVR::Sizei tex0Size = ovr_GetFovTextureSize( m_hmd, ovrEye_Left, hmdDesc.DefaultEyeFov[ovrEye_Left], 1.0f );
		OVR::Sizei tex1Size = ovr_GetFovTextureSize( m_hmd, ovrEye_Right, hmdDesc.DefaultEyeFov[ovrEye_Right], 1.0f );

		return max( tex0Size.h, tex1Size.h );
	}
	//--------------------------------------------------------------------------------
	void ReadEyeData()
	{
        // Get both eye poses simultaneously, with IPD offset already included. 
        ovrVector3f      HmdToEyeViewOffset[2] = { eyeRenderDesc[0].HmdToEyeOffset,
                                                   eyeRenderDesc[1].HmdToEyeOffset };

		double timing = ovr_GetPredictedDisplayTime( m_hmd, 0 );
        ovrTrackingState hmdState = ovr_GetTrackingState( m_hmd, timing, ovrTrue );
        
		ovr_CalcEyePoses( hmdState.HeadPose.ThePose, HmdToEyeViewOffset, eyePose );

		// Update the layer information with our new pose information.

		for ( int eye = 0; eye < 2; eye++ ) {
            layer.RenderPose[eye]   = eyePose[eye];
        }

	}
	//--------------------------------------------------------------------------------
	Matrix3f GetOrientation( double time )
	{
		// Query the HMD for the sensor state at a given time. "0.0" means "most recent time".
		// The last parameter indicates if this is the call to get the tracking state
		// immediately before rendering, which in this case it is not.  That is in the
		// ReadEyeData function above.

		ovrTrackingState ts = ovr_GetTrackingState( m_hmd, time, ovrFalse );

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
		return Matrix4f::TranslationMatrix( eyeRenderDesc[eye].HmdToEyeOffset.x,
											eyeRenderDesc[eye].HmdToEyeOffset.y,
											eyeRenderDesc[eye].HmdToEyeOffset.z );
	}
	//--------------------------------------------------------------------------------
	Matrix4f GetEyeSpatialState( unsigned int eye )
	{
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

		double frame_time = ovr_GetPredictedDisplayTime( m_hmd, frame );


		// The delta time is returned to the application, and is subsequently used
		// to update the state of the application.

		float delta = static_cast<float>( frame_time - last_frame_time );
		last_frame_time = frame_time;


		// The eye pose is read out from the API using RiftHMD::ReadEyeData() which 
		// collects the information for both eyes simultaneously.

		ReadEyeData();


		return( delta );
	}
	//--------------------------------------------------------------------------------
	void EndFrame()
	{
		// Commit the current swap chain buffers for each eye.
		//
		// TODO: It may be worth it to move this to its own method in RiftHMD (something
		//       like RiftHMD::EyeRenderComplete() or similar) if there is a performance
		//       difference.  This needs to be tested, then either remove this note, or
		//       move the commit calls.

		ovr_CommitTextureSwapChain( m_hmd, textureSets[ovrEye_Left] );
		ovr_CommitTextureSwapChain( m_hmd, textureSets[ovrEye_Right] );

		const unsigned int layer_count = 1;

		ovrLayerHeader* headers[layer_count] = { &layer.Header };
		ovrResult result = ovr_SubmitFrame( m_hmd, frame, nullptr, headers, layer_count );
	}
	//--------------------------------------------------------------------------------
	ovrTextureFormat TranslateDXGItoOVR( DXGI_FORMAT format )
	{
		ovrTextureFormat ovrFormat = ovrTextureFormat::OVR_FORMAT_UNKNOWN;

		switch ( format )
		{
		case DXGI_FORMAT_B5G6R5_UNORM:
			ovrFormat = OVR_FORMAT_B5G6R5_UNORM;    ///< Not currently supported on PC. Would require a DirectX 11.1 device.
			break;
		case DXGI_FORMAT_B5G5R5A1_UNORM:
			ovrFormat = OVR_FORMAT_B5G5R5A1_UNORM;  ///< Not currently supported on PC. Would require a DirectX 11.1 device.
			break;
		case DXGI_FORMAT_B4G4R4A4_UNORM:
			ovrFormat = OVR_FORMAT_B4G4R4A4_UNORM;  ///< Not currently supported on PC. Would require a DirectX 11.1 device.
			break;
		case DXGI_FORMAT_R8G8B8A8_UNORM:
			ovrFormat = OVR_FORMAT_R8G8B8A8_UNORM;
			break;
		case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
			ovrFormat = OVR_FORMAT_R8G8B8A8_UNORM_SRGB;
			break;
		case DXGI_FORMAT_B8G8R8A8_UNORM:
			ovrFormat = OVR_FORMAT_B8G8R8A8_UNORM;
			break;
		case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB:
			ovrFormat = OVR_FORMAT_B8G8R8A8_UNORM_SRGB; ///< Not supported for OpenGL applications
			break;
		case DXGI_FORMAT_B8G8R8X8_UNORM:
			ovrFormat = OVR_FORMAT_B8G8R8X8_UNORM;      ///< Not supported for OpenGL applications
			break;
		case DXGI_FORMAT_B8G8R8X8_UNORM_SRGB:
			ovrFormat = OVR_FORMAT_B8G8R8X8_UNORM_SRGB; ///< Not supported for OpenGL applications
			break;
		case DXGI_FORMAT_R16G16B16A16_FLOAT:
			ovrFormat = OVR_FORMAT_R16G16B16A16_FLOAT;
			break;
		case DXGI_FORMAT_D16_UNORM:
			ovrFormat = OVR_FORMAT_D16_UNORM;
			break;
		case DXGI_FORMAT_D24_UNORM_S8_UINT:
			ovrFormat = OVR_FORMAT_D24_UNORM_S8_UINT;
			break;
		case DXGI_FORMAT_D32_FLOAT:
			ovrFormat = OVR_FORMAT_D32_FLOAT;
			break;
		case DXGI_FORMAT_D32_FLOAT_S8X24_UINT:
			ovrFormat = OVR_FORMAT_D32_FLOAT_S8X24_UINT;
			break;
		}

		return ovrFormat;
	}
	//--------------------------------------------------------------------------------
	void ConfigureRendering( unsigned int max_w, unsigned int max_h, DXGI_FORMAT format )
	{
		// Get the rendering description for each eye, based on an input FOV.
		// TODO: This FOV calculation could be modified to not use the default...

		ovrHmdDesc hmdDesc = ovr_GetHmdDesc( m_hmd );

		eyeRenderDesc[0] = ovr_GetRenderDesc( m_hmd, ovrEye_Left, hmdDesc.DefaultEyeFov[0] );
		eyeRenderDesc[1] = ovr_GetRenderDesc( m_hmd, ovrEye_Right, hmdDesc.DefaultEyeFov[1] );

		// Get the desired texture sizes for the eye render targets.  Note that these 
		// are typically larger than the resolution of the HMD's display panel itself.

		unsigned int width = min( DesiredEyeTextureWidth(), max_w );
		unsigned int height = min( DesiredEyeTextureHeight(), max_h );


		// Create two render targets - one for each eye.  These are created with both
		// render target and shader resource bind points so that they can be used by
		// the Oculus SDK to do the distortion rendering at the end of the frame.

		textureSets[0] = nullptr;
		textureSets[1] = nullptr;

		ovrTextureSwapChainDesc config;
		config.Type = ovrTexture_2D;
		config.Format = TranslateDXGItoOVR( format );
		config.ArraySize = 1;
		config.Width = width;
		config.Height = height;
		config.MipLevels = 1;
		config.SampleCount = 1;
		config.StaticImage = ovrFalse;
		config.MiscFlags = ovrTextureMisc_None;
		config.BindFlags = ovrTextureBind_DX_RenderTarget;

		
		if ( ovr_CreateTextureSwapChainDX( m_hmd, RendererDX11::Get()->GetDevice(), &config, &textureSets[0] ) != ovrSuccess ) {
			Log::Get().Write( L"ERROR: Could not create swap texture sets!" );
		}

		if ( ovr_CreateTextureSwapChainDX( m_hmd, RendererDX11::Get()->GetDevice(), &config, &textureSets[1] ) != ovrSuccess ) {
			Log::Get().Write( L"ERROR: Could not create swap texture sets!" );
		}


		// Create render target views for each of the texture surfaces in our texture sets.
		// We just store those in a vector for reference later on, with one vector for
		// each eye.
		
		for ( unsigned int eye = 0; eye < 2; ++eye )
		{
			int count = 0;
			ovr_GetTextureSwapChainLength( m_hmd, textureSets[eye], &count );

			for ( int i = 0; i < count; ++i )
			{
				ID3D11Texture2D* texture = nullptr;
				ovr_GetTextureSwapChainBufferDX( m_hmd, textureSets[eye], i, IID_PPV_ARGS(&texture));
				
				ResourcePtr resource = RendererDX11::Get()->LoadTexture( texture );

				resource->m_iResourceRTV = 
					RendererDX11::Get()->CreateRenderTargetView(resource->m_iResource, nullptr );

				textureRTVs[eye].push_back( resource );

				texture->Release(); 
			}
		}

		// Initialize a single full screen FOV layer.
		
		layer.Header.Type = ovrLayerType_EyeFov;
		layer.Header.Flags = 0;
		layer.ColorTexture[0] = textureSets[0];
		layer.ColorTexture[1] = textureSets[1];
		layer.Fov[0] = eyeRenderDesc[0].Fov;
		layer.Fov[1] = eyeRenderDesc[1].Fov;
		layer.Viewport[0] = OVR::Recti(0, 0, width, height);
		layer.Viewport[1] = OVR::Recti(0, 0, width, height);
	}
	//--------------------------------------------------------------------------------
	ResourcePtr GetEyeTexture( unsigned int eye )
	{
		assert( eye == 0 || eye == 1 );

		int currentIndex = 0;
		ovr_GetTextureSwapChainCurrentIndex( m_hmd, textureSets[eye], &currentIndex );

		return textureRTVs[eye][currentIndex];
	}
	//--------------------------------------------------------------------------------
	ResourcePtr GetMirrorTexture( unsigned int w, unsigned int h, DXGI_FORMAT format )
	{
		// If the texture already exists, then delete it.
		if ( mirrorTexture != nullptr )
		{
			ovr_DestroyMirrorTexture( m_hmd, mirrorTexture );
			mirrorTexture = nullptr;
		}

		// Ensure that an appropriately sized mirror texture is available.
		if ( mirrorTexture == nullptr )
		{
			ovrMirrorTextureDesc config = { };
			config.Format = TranslateDXGItoOVR( format );
			config.Width = w;
			config.Height = h;
			config.MiscFlags = 0;

			ovr_CreateMirrorTextureDX( m_hmd, RendererDX11::Get()->GetDevice(), &config, &mirrorTexture );
		}

        ID3D11Texture2D* tex = nullptr;

        ovr_GetMirrorTextureBufferDX( m_hmd, mirrorTexture, IID_PPV_ARGS(&tex));
		ResourcePtr resource = RendererDX11::Get()->LoadTexture( tex );

		tex->Release();

		return resource;
	}
	//--------------------------------------------------------------------------------
	RiftManagerPtr m_RiftMgr;
	ovrSession m_hmd;
	ovrGraphicsLuid m_luid;
	ovrFovPort eyeFov[2];
	ovrEyeRenderDesc eyeRenderDesc[2];
	ovrPosef eyePose[2];
	
	ovrLayerEyeFov layer;
	ovrTextureSwapChain textureSets[2];
	std::vector<ResourcePtr> textureRTVs[2];
	
	ovrMirrorTexture mirrorTexture;

	unsigned int frame;
	double last_frame_time;
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