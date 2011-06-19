//--------------------------------------------------------------------------------
#include "PCH.h"

#include "RenderApplication.h"
#include "Log.h"

#include <sstream>

#include "EventManager.h"
#include "EvtFrameStart.h"
#include "EvtChar.h"
#include "EvtKeyUp.h"
#include "EvtKeyDown.h"

#include "ScriptManager.h"

#include "SwapChainConfigDX11.h"
#include "Texture2dConfigDX11.h"

#include "GeometryLoaderDX11.h"
#include "GeometryGeneratorDX11.h"
#include "MaterialGeneratorDX11.h"
#include "RasterizerStateConfigDX11.h"

#include "IParameterManager.h"

using namespace Glyph3;
//--------------------------------------------------------------------------------
RenderApplication::RenderApplication()
{
	m_pRenderer11 = 0;
	m_pWindow = 0;

	m_iWidth = 800;
	m_iHeight = 600;

	m_pRenderView = 0;
	m_pTextOverlayView = 0;

	m_pCamera = 0;

	m_bSaveScreenshot = false;
}
//--------------------------------------------------------------------------------
bool RenderApplication::ConfigureRenderingEngineComponents( int width, int height, D3D_FEATURE_LEVEL desiredLevel, D3D_DRIVER_TYPE driverType )
{
	// Create the renderer and initialize it for the desired device
	// type and feature level.

	m_pRenderer11 = new RendererDX11();

	if ( !m_pRenderer11->Initialize( driverType, desiredLevel ) )
	{
		Log::Get().Write( L"Could not create hardware device, trying to create the reference device..." );

		if ( !m_pRenderer11->Initialize( D3D_DRIVER_TYPE_REFERENCE, D3D_FEATURE_LEVEL_11_0 ) )
		{
			MessageBox( m_pWindow->GetHandle(), L"Could not create a hardware or software Direct3D 11 device - the program will now abort!", L"Hieroglyph 3 Rendering", MB_ICONEXCLAMATION | MB_SYSTEMMODAL );
			RequestTermination();			
			return( false );
		}

		// If using the reference device, utilize a fixed time step for any animations.
		m_pTimer->SetFixedTimeStep( 1.0f / 10.0f );
	}

	// Create the window.
	m_iWidth = width;
	m_iHeight = height;

	// Create the window wrapper class instance.
	m_pWindow = new Win32RenderWindow();
	m_pWindow->SetPosition( 20, 20 );
	m_pWindow->SetSize( m_iWidth, m_iHeight );
	m_pWindow->SetCaption( GetName() );
	m_pWindow->Initialize();

	// Create a swap chain for the window.
	SwapChainConfigDX11 Config;
	Config.SetWidth( m_pWindow->GetWidth() );
	Config.SetHeight( m_pWindow->GetHeight() );
	Config.SetOutputWindow( m_pWindow->GetHandle() );
	m_pWindow->SetSwapChain( m_pRenderer11->CreateSwapChain( &Config ) );

	// We'll keep a copy of the swap chain's render target index to 
	// use later.
	m_RenderTarget = m_pRenderer11->GetSwapChainResource( m_pWindow->GetSwapChain() );

	// Next we create a depth buffer for use in the traditional rendering
	// pipeline.
	Texture2dConfigDX11 DepthConfig;
	DepthConfig.SetDepthBuffer( m_iWidth, m_iHeight );
	m_DepthTarget = m_pRenderer11->CreateTexture2D( &DepthConfig, 0 );


	// Create the camera, and the render view that will produce an image of the 
	// from the camera's point of view of the scene.

	m_pRenderView = new ViewPerspective( *m_pRenderer11, m_RenderTarget, m_DepthTarget );
	m_pRenderView->SetBackColor( Vector4f( 0.6f, 0.6f, 0.9f, 1.0f ) );

	m_pTextOverlayView = new ViewTextOverlay( *m_pRenderer11, m_RenderTarget, m_DepthTarget );


	m_pCamera = new Camera();
	m_pCamera->GetNode()->Rotation().Rotation( Vector3f( 0.0f, 0.0f, 0.0f ) );
	m_pCamera->GetNode()->Position() = Vector3f( 0.0f, 10.0f, -20.0f );
	m_pCamera->SetCameraView( m_pRenderView );
	m_pCamera->SetOverlayView( m_pTextOverlayView );
	m_pCamera->SetProjectionParams( 0.1f, 1000.0f, static_cast<float>(m_iWidth) / static_cast<float>(m_iHeight), static_cast<float>( D3DX_PI ) / 2.0f );

	m_pScene->AddCamera( m_pCamera );

	m_pRenderer11->SetMultiThreadingState( true );

	return( true );
}
//--------------------------------------------------------------------------------
