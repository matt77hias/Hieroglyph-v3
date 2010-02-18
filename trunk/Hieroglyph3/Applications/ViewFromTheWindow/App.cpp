//--------------------------------------------------------------------------------
#include "App.h"
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

using namespace Glyph3;
//--------------------------------------------------------------------------------
App AppInstance; // Provides an instance of the application
//--------------------------------------------------------------------------------


//--------------------------------------------------------------------------------
App::App() : Application( 640, 480, true )
{
	m_bSaveScreenshot = false;
}
//--------------------------------------------------------------------------------
bool App::ConfigureEngineComponents()
{
	// The application currently supplies the 
	int width = this->DisplayWidth( ); 
	int height = this->DisplayHeight( ); 
	bool windowed = this->DisplayWindowed( );

	// Initialize the random number generator.
	srand( GetTickCount() );

	for ( int i = 0; i < 4; i++ )
	{
		// Set the render window parameters and initialize the window
		m_pWindow[i] = new Win32RenderWindow();

		int x = (double)rand() / (RAND_MAX + 1) * 200 + 400;
		int y = (double)rand() / (RAND_MAX + 1) * 200 + 300;
		int w = (double)rand() / (RAND_MAX + 1) * 200;	
		int h = (double)rand() / (RAND_MAX + 1) * 200;

		m_pWindow[i]->SetPosition( x, y );
		m_pWindow[i]->SetSize( w, h );
		m_pWindow[i]->SetCaption( std::wstring( L"Direct3D 11 Window" ) );
		m_pWindow[i]->Initialize();
	}

	
	// Create the renderer and initialize it for the desired device
	// type and feature level.

	m_pRenderer11 = new RendererDX11();

	if ( !m_pRenderer11->Initialize( D3D_DRIVER_TYPE_HARDWARE, D3D_FEATURE_LEVEL_10_0 ) )
	{
		Log::Get().Write( L"Could not create hardware device, trying to create the reference device..." );

		if ( !m_pRenderer11->Initialize( D3D_DRIVER_TYPE_REFERENCE, D3D_FEATURE_LEVEL_10_0 ) )
		{
			for ( int i = 0; i < 4; i++ )
				ShowWindow( m_pWindow[i]->GetHandle(), SW_HIDE );

			MessageBox( m_pWindow[0]->GetHandle(), L"Could not create a hardware or software Direct3D 11 device - the program will now abort!", L"Hieroglyph 3 Rendering", MB_ICONEXCLAMATION | MB_SYSTEMMODAL );
			RequestTermination();			
			return( false );
		}
	}


	// Create a swap chain for all of the windows.

	for ( int i = 0; i < 4; i++ )
	{
		SwapChainConfigDX11 Config;
		Config.SetWidth( m_pWindow[i]->GetWidth() );
		Config.SetHeight( m_pWindow[i]->GetHeight() );
		Config.SetOutputWindow( m_pWindow[i]->GetHandle() );
		int	iSwapChain = m_pRenderer11->CreateSwapChain( &Config );
		m_pWindow[i]->SetSwapChain( iSwapChain );
	}

	for ( int i = 0; i < 4; i++ )
	{
		// We'll keep a copy of the render target index to use in later examples.
		m_iRenderTarget[i] = m_pRenderer11->GetSwapChainRenderTargetViewID( m_pWindow[i]->GetSwapChain() );
	}


	// Next we create a depth buffer for use in the traditional rendering
	// pipeline.

	for ( int i = 0; i < 4; i++ )
	{
		Texture2dConfigDX11 DepthConfig;
		DepthConfig.SetDepthBuffer( m_pWindow[i]->GetWidth(), m_pWindow[i]->GetHeight() );
		int DepthID = m_pRenderer11->CreateTexture2D( &DepthConfig, 0 );
		m_iDepthTarget[i] = m_pRenderer11->CreateDepthStencilView( DepthID, 0 );
	}


	// Create a view port to use on the scene.  This basically selects the 
	// entire floating point area of the render target.

	D3D11_VIEWPORT viewport;
	viewport.Width = static_cast< float >( width );
	viewport.Height = static_cast< float >( height );
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;

	int ViewPort = m_pRenderer11->CreateViewPort( viewport );
	m_pRenderer11->SetViewPort( ViewPort );
	
	return( true );
}
//--------------------------------------------------------------------------------
void App::ShutdownEngineComponents()
{
	if ( m_pRenderer11 )
	{
		m_pRenderer11->Shutdown();
		delete m_pRenderer11;
	}

	for ( int i = 0; i < 4; i++ )
	{
		if ( m_pWindow[i] )
		{
			m_pWindow[i]->Shutdown();
			delete m_pWindow[i];
		}
	}
}
//--------------------------------------------------------------------------------
void App::Initialize()
{
	// Basic event handling is supported with the EventManager class.  This is a 
	// singleton class that allows an EventListener to register which events it
	// wants to receive.

	EventManager* pEventManager = EventManager::Get( );

	// The application object wants to know about these three events, so it 
	// registers itself with the appropriate event IDs.

	pEventManager->AddEventListener( SYSTEM_KEYBOARD_KEYUP, this );
	pEventManager->AddEventListener( SYSTEM_KEYBOARD_KEYDOWN, this );
	pEventManager->AddEventListener( SYSTEM_KEYBOARD_CHAR, this );
}
//--------------------------------------------------------------------------------
void App::Update()
{
	// Update the timer to determine the elapsed time since last frame.  This can 
	// then used for animation during the frame.

	m_pTimer->Update();

	// Send an event to everyone that a new frame has started.  This will be used
	// in later examples for using the material system with render views.

	EventManager::Get()->ProcessEvent( new EvtFrameStart() );


	// Perform the rendering and presentation for each window.

	for ( int i = 0; i < 4; i++ )
	{
		// Bind the swap chain render target and the depth buffer for use in 
		// rendering.  

		m_pRenderer11->BindRenderTargets( m_iRenderTarget[i], m_iDepthTarget[i] );

		float fBlue = sinf( m_pTimer->Runtime() * m_pTimer->Runtime() ) * 0.25f + 0.5f;

		m_pRenderer11->ClearBuffers( Vector4f( 0.0f, 0.0f, fBlue, 0.0f ), 1.0f );
		m_pRenderer11->Present( m_pWindow[i]->GetHandle(), m_pWindow[i]->GetSwapChain() );
	}


	// Save a screenshot if desired.  This is done by pressing the 's' key, which
	// demonstrates how an event is sent and handled by an event listener (which
	// in this case is the application object itself).

	if ( m_bSaveScreenshot  )
	{
		m_bSaveScreenshot = false;
		m_pRenderer11->SaveTextureScreenShot( 0, std::wstring( L"BasicApplication_" ), D3DX11_IFF_BMP );
	}
}
//--------------------------------------------------------------------------------
void App::Shutdown()
{
	// Print the framerate out for the log before shutting down.

	std::wstringstream out;
	out << L"Max FPS: " << m_pTimer->MaxFramerate();
	Log::Get().Write( out.str() );

	Application::Shutdown();
}
//--------------------------------------------------------------------------------
bool App::HandleEvent( IEvent* pEvent )
{
	eEVENT e = pEvent->GetEventType();

	if ( e == SYSTEM_KEYBOARD_KEYDOWN )
	{
		EvtKeyDown* pKeyDown = (EvtKeyDown*)pEvent;

		unsigned int key = pKeyDown->GetCharacterCode();

		return( true );
	}
	else if ( e == SYSTEM_KEYBOARD_KEYUP )
	{
		EvtKeyUp* pKeyUp = (EvtKeyUp*)pEvent;

		unsigned int key = pKeyUp->GetCharacterCode();

		if ( key == VK_ESCAPE ) // 'Esc' Key - Exit the application
		{
			this->RequestTermination();
			return( true );
		}
		else if ( key == 0x53 ) // 'S' Key - Save a screen shot for the next frame
		{
			m_bSaveScreenshot = true;
			return( true );
		}
		else
		{
			return( false );
		}
	}

	
	return( false );
}
//--------------------------------------------------------------------------------
std::wstring App::GetName( )
{
	return( std::wstring( L"BasicApplication" ) );
}
//--------------------------------------------------------------------------------