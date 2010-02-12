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
#include "RasterizerStateConfigDX11.h"
#include "BufferConfigDX11.h"

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

		// If using the reference device, utilize a fixed time step for any animations.
		m_pTimer->SetFixedTimeStep( 1.0f / 10.0f );
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

	// First create the basic visualization render effect
	m_pTessellationEffect = new RenderEffectDX11();
	m_pTessellationEffect->m_iVertexShader = 
		m_pRenderer11->LoadShader( VERTEX_SHADER,
		std::wstring( L"../Data/Shaders/BasicTessellation.hlsl" ),
		std::wstring( L"VSMAIN" ),
		std::wstring( L"vs_5_0" ) );
	m_pTessellationEffect->m_iHullShader =
		m_pRenderer11->LoadShader( HULL_SHADER,
		std::wstring( L"../Data/Shaders/BasicTessellation.hlsl" ),
		std::wstring( L"HSMAIN" ),
		std::wstring( L"hs_5_0" ) );
	m_pTessellationEffect->m_iDomainShader =
		m_pRenderer11->LoadShader( DOMAIN_SHADER,
		std::wstring( L"../Data/Shaders/BasicTessellation.hlsl" ),
		std::wstring( L"DSMAIN" ),
		std::wstring( L"ds_5_0" ) );
	m_pTessellationEffect->m_iPixelShader = 
		m_pRenderer11->LoadShader( PIXEL_SHADER,
		std::wstring( L"../Data/Shaders/BasicTessellation.hlsl" ),
		std::wstring( L"PSMAIN" ),
		std::wstring( L"ps_5_0" ) );

	RasterizerStateConfigDX11 RS;
	RS.FillMode = D3D11_FILL_WIREFRAME;
	m_pTessellationEffect->m_iRasterizerState = 
		m_pRenderer11->CreateRasterizerState( &RS );


	// Load and initialize the geometry to be rendered.

	m_pGeometry = GeometryLoaderDX11::loadMS3DFile2( std::wstring( L"../Data/Models/box.ms3d" ) );
	m_pGeometry->GenerateInputLayout( m_pTessellationEffect->m_iVertexShader );
	m_pGeometry->LoadToBuffers();
	m_pGeometry->SetPrimitiveType( D3D11_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST );


	// Create the constant buffers for use with this effect

	BufferConfigDX11 cbuffer;
	cbuffer.SetDefaultConstantBuffer( 2*sizeof( D3DXMATRIX ), true );
	int CB = m_pRenderer11->CreateConstantBuffer( &cbuffer, 0 );
	m_pRenderer11->SetConstantBufferParameter( std::wstring( L"Transforms" ), &CB );

	cbuffer.SetDefaultConstantBuffer( 2*sizeof( Vector4f ), true );
	CB = m_pRenderer11->CreateConstantBuffer( &cbuffer, 0 );
	m_pRenderer11->SetConstantBufferParameter( std::wstring( L"TessellationParameters" ), &CB );

	m_TessParams = Vector4f( 1.0f, 1.0f, 1.0f, 1.0f );
	m_pRenderer11->SetVectorParameter( std::wstring( L"EdgeFactors" ), &m_TessParams );


	// Create the material for use by the entities.

	m_pMaterial = new MaterialDX11();
	m_pMaterial->Params[VT_PERSPECTIVE].pEffect = m_pTessellationEffect;
	m_pMaterial->Params[VT_PERSPECTIVE].bRender = true;


	m_pRenderView = new ViewPerspective( *m_pRenderer11, 0 );
	m_pRenderView->SetBackColor( Vector4f( 0.6f, 0.6f, 0.6f, 0.6f ) );
	m_pRoot = new Node3D();


	for ( int i = 0; i < 10; i++ )
	{
		m_pEntity[i] = new Entity3D();
		m_pEntity[i]->SetGeometry( m_pGeometry );
		m_pEntity[i]->SetMaterial( m_pMaterial, false );
		m_pEntity[i]->Position() = Vector3f( i * 4, 4.0f * ( i % 2 ) - 2.0f, 0.0f );

		m_pRoot->AttachChild( m_pEntity[i] );
	}
	
	m_pRenderView->SetRoot( m_pRoot );
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

	// Clear the window to a time varying color.

	//float fBlue = sinf( m_pTimer->Runtime() * m_pTimer->Runtime() ) * 0.25f + 0.5f;
	//m_pRenderer11->ClearBuffers( Vector4f( 0.0f, 0.0f, fBlue, 0.0f ), 1.0f );

	Matrix3f rotation;
	rotation.RotationX( m_pTimer->Elapsed() );
	m_pRoot->Rotation() *= rotation;

	m_pRoot->Update( m_pTimer->Elapsed() );

	m_pRenderView->Draw( *m_pRenderer11 );

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
	SAFE_DELETE( m_pRenderView );
	SAFE_DELETE( m_pRoot );

	for ( int i = 0; i < 10; i++ )
		SAFE_DELETE( m_pEntity[i] );

	// TODO: make the render effects managed by the materials!
	SAFE_DELETE( m_pTessellationEffect );
	

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