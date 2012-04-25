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
#include "FirstPersonCamera.h"

#include "EvtWindowResize.h"
#include "ViewPerspective.h"

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

	// Register for window based events here.
	m_pEventMgr->AddEventListener( WINDOW_RESIZE, this );
}
//--------------------------------------------------------------------------------
RenderApplication::~RenderApplication( )
{
}
//--------------------------------------------------------------------------------
Win32RenderWindow* RenderApplication::CreateRenderWindow( )
{
	return( new Win32RenderWindow() );
}
//--------------------------------------------------------------------------------
bool RenderApplication::ConfigureRenderingEngineComponents( UINT width, UINT height, D3D_FEATURE_LEVEL desiredLevel, D3D_DRIVER_TYPE driverType )
{
	// Create the renderer and initialize it for the desired device
	// type and feature level.

	m_pRenderer11 = new RendererDX11();

	if ( !m_pRenderer11->Initialize( driverType, desiredLevel ) )
	{
		Log::Get().Write( L"Could not create hardware device, trying to create the reference device..." );

		if ( !m_pRenderer11->Initialize( D3D_DRIVER_TYPE_REFERENCE, D3D_FEATURE_LEVEL_11_0 ) )
		{
			MessageBox( 0, L"Could not create a hardware or software Direct3D 11 device - the program will now abort!", L"Hieroglyph 3 Rendering", MB_ICONEXCLAMATION | MB_SYSTEMMODAL );
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
	m_pWindow = CreateRenderWindow();
	m_pWindow->SetPosition( 20, 20 );
	m_pWindow->SetSize( m_iWidth, m_iHeight );
	m_pWindow->SetCaption( GetName() );
	m_pWindow->Initialize( this );

	// Create a swap chain for the window.
	SwapChainConfigDX11 Config;
	Config.SetWidth( m_pWindow->GetWidth() );
	Config.SetHeight( m_pWindow->GetHeight() );
	Config.SetOutputWindow( m_pWindow->GetHandle() );
	m_pWindow->SetSwapChain( m_pRenderer11->CreateSwapChain( &Config ) );

	// We'll keep a copy of the swap chain's render target index to 
	// use later.
	m_BackBuffer = m_pRenderer11->GetSwapChainResource( m_pWindow->GetSwapChain() );

	m_pRenderer11->SetMultiThreadingState( true );

	return( true );
}
//--------------------------------------------------------------------------------
bool RenderApplication::ConfigureRenderingSetup()
{
	// Create the camera, and the render view that will produce an image of the 
	// from the camera's point of view of the scene.

	ViewPerspective* pPerspectiveView = new ViewPerspective( *m_pRenderer11, m_BackBuffer );
	pPerspectiveView->SetBackColor( Vector4f( 0.6f, 0.6f, 0.9f, 1.0f ) );
	m_pRenderView = pPerspectiveView;

	m_pTextOverlayView = new ViewTextOverlay( *m_pRenderer11, m_BackBuffer );


	m_pCamera = new FirstPersonCamera();
	m_pCamera->GetNode()->Rotation().Rotation( Vector3f( 0.0f, 0.0f, 0.0f ) );
	m_pCamera->GetNode()->Position() = Vector3f( 0.0f, 10.0f, -20.0f );
	m_pCamera->SetCameraView( m_pRenderView );
	m_pCamera->SetOverlayView( m_pTextOverlayView );
	m_pCamera->SetProjectionParams( 0.1f, 1000.0f, static_cast<float>(m_iWidth) / static_cast<float>(m_iHeight), static_cast<float>( GLYPH_PI ) / 4.0f );

	m_pScene->AddCamera( m_pCamera );

	return( true );
}
//--------------------------------------------------------------------------------
void RenderApplication::ShutdownRenderingEngineComponents()
{
	m_pRenderer11->Shutdown();
	SAFE_DELETE( m_pRenderer11 );

	m_pWindow->Shutdown();
	SAFE_DELETE( m_pWindow );
}
//--------------------------------------------------------------------------------
void RenderApplication::ShutdownRenderingSetup()
{
	SAFE_DELETE( m_pScene );
	SAFE_DELETE( m_pCamera );
}
//--------------------------------------------------------------------------------
bool RenderApplication::HandleEvent( IEvent* pEvent )
{
	// This method body is included here for future use, to allow the 
	// RenderApplication to handle some events if needed.

	eEVENT e = pEvent->GetEventType();

	if ( e == WINDOW_RESIZE )
	{
		EvtWindowResize* pResize = (EvtWindowResize*)pEvent;

		this->HandleWindowResize( pResize->GetWindowHandle(), pResize->NewWidth(), pResize->NewHeight() );

		return( true );
	}



	// Call the parent class's event handler if we haven't handled the event.
	
	return( Application::HandleEvent( pEvent ) );
}
//--------------------------------------------------------------------------------
void RenderApplication::HandleWindowResize( HWND handle, UINT width, UINT height )
{
	// TODO: are these local width and height members needed???
	if ( width < 1 ) width = 1;
	if ( height < 1 ) height = 1;
	m_iWidth = width;
	m_iHeight = height;

	// Resize our rendering window state if the handle matches
	if ( ( m_pWindow != 0 ) && ( m_pWindow->GetHandle() == handle ) ) {
		m_pWindow->ResizeWindow( width, height );
		m_pRenderer11->ResizeSwapChain( m_pWindow->GetSwapChain(), width, height );
	}

	// Update the projection matrix of our camera
	if ( m_pCamera != 0 ) {
		m_pCamera->SetAspectRatio( static_cast<float>(m_iWidth) / static_cast<float>(m_iHeight) );
	}

	// Update the render views being used to render the scene
	if ( m_pRenderView != 0 ) {
		m_pRenderView->Resize( width, height );
	}

	// Update the text render view being used to render the scene
	if ( m_pTextOverlayView != 0 ) {
		m_pTextOverlayView->Resize( width, height );
	}
}
//--------------------------------------------------------------------------------