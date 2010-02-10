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

#include "GeometryGeneratorDX11.h"
#include "RenderEffectDX11.h"

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

	// Set the render window parameters and initialize the window
	m_pWindow = new Win32RenderWindow();
	m_pWindow->SetPosition( 25, 25 );
	m_pWindow->SetSize( width, height );
	m_pWindow->SetCaption( std::wstring( L"Direct3D 11 Window #1" ) );
	m_pWindow->Initialize();

	// Record the application handle.

	this->SetWindowID( m_pWindow->GetHandle() );

	
	// Create the renderer and initialize it for the desired device
	// type and feature level.

	m_pRenderer11 = new RendererDX11();

	if ( !m_pRenderer11->Initialize( D3D_DRIVER_TYPE_HARDWARE, D3D_FEATURE_LEVEL_11_0 ) )
	{
		Log::Get().Write( L"Could not create hardware device, trying to create the reference device..." );

		if ( !m_pRenderer11->Initialize( D3D_DRIVER_TYPE_REFERENCE, D3D_FEATURE_LEVEL_11_0 ) )
		{
			ShowWindow( m_pWindow->GetHandle(), SW_HIDE );
			MessageBox( m_pWindow->GetHandle(), L"Could not create a hardware or software Direct3D 11 device - the program will now abort!", L"Hieroglyph 3 Rendering", MB_ICONEXCLAMATION | MB_SYSTEMMODAL );
			RequestTermination();			
			return( false );
		}
	}


	// Create a swap chain for the window that we started out with.  This
	// demonstrates using a configuration object for fast and concise object
	// creation.

	SwapChainConfigDX11 Config;
	Config.SetWidth( m_pWindow->GetWidth() );
	Config.SetHeight( m_pWindow->GetHeight() );
	Config.SetOutputWindow( m_pWindow->GetHandle() );
	m_iSwapChain = m_pRenderer11->CreateSwapChain( &Config );
	m_pWindow->SetSwapChain( m_iSwapChain );

	// We'll keep a copy of the render target index to use in later examples.

	m_iRenderTarget = m_pRenderer11->GetSwapChainRenderTargetViewID( m_iSwapChain );

	// Next we create a depth buffer for use in the traditional rendering
	// pipeline.

	Texture2dConfigDX11 DepthConfig;
	DepthConfig.SetDepthBuffer( width, height );
	int DepthID = m_pRenderer11->CreateTexture2D( &DepthConfig, 0 );
	m_iDepthTarget = m_pRenderer11->CreateDepthStencilView( DepthID, 0 );
	
	// Bind the swap chain render target and the depth buffer for use in 
	// rendering.  

	m_pRenderer11->BindRenderTargets( m_iRenderTarget, m_iDepthTarget );

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

	if ( m_pWindow )
	{
		m_pWindow->Shutdown();
		delete m_pWindow;
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

	// Here we load our desired texture, and create a shader resource view to 
	// use for input to the compute shader.  By specifying null for the 
	// configuration, the view is created from the default resource configuration.

	m_iTexture = m_pRenderer11->LoadTexture( L"../Data/Textures/Outcrop.png" );
	m_iTextureSRV = m_pRenderer11->CreateShaderResourceView( m_iTexture, 0 );

	// Create the texture for output of the compute shader.
	Texture2dConfigDX11 FilteredConfig;
	FilteredConfig.SetColorBuffer( 640, 480 ); 
	FilteredConfig.SetBindFlags( D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE );

	m_iOutput = m_pRenderer11->CreateTexture2D( &FilteredConfig, 0 );
	m_iOutputUAV = m_pRenderer11->CreateUnorderedAccessView( m_iOutput, 0 );
	m_iOutputSRV = m_pRenderer11->CreateShaderResourceView( m_iOutput, 0 );

	// Create RenderEffectDX11 instances to utilize the DX11 pipeline for 
	// processing data.  Both the standard rendering pipeline and the compute
	// shader pipeline use the RenderEffect to configure itself.

	m_pFilterEffect = new RenderEffectDX11();
	m_pFilterEffect->m_iComputeShader = 
		m_pRenderer11->LoadShader( COMPUTE_SHADER, 
		std::wstring( L"../Data/Shaders/InvertColorCS.hlsl" ),
		std::wstring( L"CSMAIN" ),
		std::wstring( L"cs_5_0" ) );

	m_pTextureEffect = new RenderEffectDX11();
	m_pTextureEffect->m_iVertexShader = 
		m_pRenderer11->LoadShader( VERTEX_SHADER,
		std::wstring( L"../Data/Shaders/TextureVS.hlsl" ),
		std::wstring( L"VSMAIN" ),
		std::wstring( L"vs_5_0" ) );
	m_pTextureEffect->m_iPixelShader = 
		m_pRenderer11->LoadShader( PIXEL_SHADER,
		std::wstring( L"../Data/Shaders/TexturePS.hlsl" ),
		std::wstring( L"PSMAIN" ),
		std::wstring( L"ps_5_0" ) );

	// Create a full screen quad for rendering the texture to the backbuffer.

	m_pFullScreen = new GeometryDX11();
	GeometryGeneratorDX11::GenerateFullScreenQuad( m_pFullScreen );

	m_pFullScreen->GenerateInputLayout( m_pTextureEffect->m_iVertexShader );
	m_pFullScreen->LoadToBuffers();

	// Generate the bindings for the resources

	m_pRenderer11->SetShaderResourceParameter( L"InputMap", &m_iTextureSRV );
	m_pRenderer11->SetUnorderedAccessParameter( L"OutputMap", &m_iOutputUAV );
	m_pRenderer11->SetShaderResourceParameter( L"ColorMap00", &m_iOutputSRV );
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

	// Perform the filtering with the compute shader.  The assumption in this case
	// is that the texture is 640x480 - if there is a different size then the 
	// dispatch call can be modified accordingly.

	m_pRenderer11->Dispatch( *m_pFilterEffect, 32, 24, 1 );

	// Render the texture to the backbuffer.

	m_pRenderer11->ClearBuffers( Vector4f( 0.0f, 0.0f, 0.0f, 0.0f ), 1.0f );
	m_pRenderer11->Draw( *m_pTextureEffect, *m_pFullScreen );

	// Present the results to the window.

	m_pRenderer11->Present( m_pWindow->GetHandle(), m_pWindow->GetSwapChain() );

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
	SAFE_RELEASE( m_pFullScreen );

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