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

using namespace Glyph3;
//--------------------------------------------------------------------------------
App AppInstance; // Provides an instance of the application
//--------------------------------------------------------------------------------


//--------------------------------------------------------------------------------
App::App()
{
	m_bSaveScreenshot = false;
	m_bGeometryMode = QUAD_MODE;
}
//--------------------------------------------------------------------------------
bool App::ConfigureEngineComponents()
{
	// The application currently supplies the 
	int width = 320;
	int height = 240;
	bool windowed = true;

	// Set the render window parameters and initialize the window
	m_pWindow = new Win32RenderWindow();
	m_pWindow->SetPosition( 25, 25 );
	m_pWindow->SetSize( width, height );
	m_pWindow->SetCaption( std::wstring( L"Direct3D 11 Tessellation Parameters Demo" ) );
	m_pWindow->Initialize();

	
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

	m_RenderTarget = m_pRenderer11->GetSwapChainResource( m_iSwapChain );

	// Next we create a depth buffer for use in the traditional rendering
	// pipeline.

	Texture2dConfigDX11 DepthConfig;
	DepthConfig.SetDepthBuffer( width, height );
	m_DepthTarget = m_pRenderer11->CreateTexture2D( &DepthConfig, 0 );
	
	// Bind the swap chain render target and the depth buffer for use in 
	// rendering.  

	m_pRenderer11->pImmPipeline->ClearRenderTargets();
	m_pRenderer11->pImmPipeline->BindRenderTargets( 0, m_RenderTarget );
	m_pRenderer11->pImmPipeline->BindDepthTarget( m_DepthTarget );
	m_pRenderer11->pImmPipeline->ApplyRenderTargets();


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
	m_pRenderer11->pImmPipeline->SetViewPort( ViewPort );
	
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

	// Create the necessary resources
	CreateQuadResources();
	CreateTriangleResources();

	// We'll be using a static view for this sample; animation/viewpoints aren't
	// the focus of this demo. Initialize and store these variables here.

	Matrix4f mView;
	D3DXVECTOR3 vLookAt = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
	D3DXVECTOR3 vLookFrom = D3DXVECTOR3( -2.0f, 2.0f, -2.0f );
	D3DXVECTOR3 vLookUp = D3DXVECTOR3( 0.0f, 1.0f, 0.0f );
	D3DXMatrixLookAtLH( reinterpret_cast<D3DXMATRIX*>(&mView), &vLookFrom, &vLookAt, &vLookUp );

	Matrix4f mWorld;
	D3DXMatrixIdentity( reinterpret_cast<D3DXMATRIX*>(&mWorld) );

	Matrix4f mProj;
	D3DXMatrixPerspectiveFovLH( reinterpret_cast<D3DXMATRIX*>(&mProj), static_cast< float >(D3DX_PI) / 4.0f, 320.0f /  240.0f, 0.1f, 50.0f );

	// Composite together for the final transform
	Matrix4f mViewProj = mView * mProj;

	// H3 engine requires no transposing of matrices...

	m_pRenderer11->m_pParamMgr->SetMatrixParameter( std::wstring( L"mWorld" ), &mWorld );
	m_pRenderer11->m_pParamMgr->SetMatrixParameter( std::wstring( L"mViewProj" ), &mViewProj );

	// Create the text rendering
	m_pFont = new SpriteFontDX11();
	m_pFont->Initialize( L"Consolas", 12.0f, 0, true );
	
	m_pSpriteRenderer = new SpriteRendererDX11();
	m_pSpriteRenderer->Initialize();
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

	// Clear the window to white
	m_pRenderer11->pImmPipeline->ClearBuffers( Vector4f( 1.0f, 1.0f, 1.0f, 1.0f ), 1.0f );

	// Draw the main geometry
	if( QUAD_MODE == m_bGeometryMode )
		m_pRenderer11->pImmPipeline->Draw( *m_pQuadEffect, *m_pQuadGeometry, m_pRenderer11->m_pParamMgr ); 
	else if( TRI_MODE == m_bGeometryMode )
		m_pRenderer11->pImmPipeline->Draw( *m_pTriangleEffect, *m_pTriangleGeometry, m_pRenderer11->m_pParamMgr ); 

	// Draw the UI text
	if( !m_bSaveScreenshot )
	{
		// Don't draw text if we're taking a screenshot - don't want the images
		// cluttered with UI text!
		std::wstringstream out;
		out << L"Hieroglyph 3 : Tessellation Parameters\nFPS: " << m_pTimer->Framerate();

		// Screenshot
		out << L"\nS : Take Screenshot";

		// Geometry toggle
		out << L"\nG : Toggle Geometry";

		// Edge weights
			// 3 of 4
		if( QUAD_MODE == m_bGeometryMode )
			out << L"\nEdge Weights: [1.2, 3.4, 5.6, 7.8]";
		else if( TRI_MODE == m_bGeometryMode )
			out << L"\nEdge Weights: [1.2, 3.4, 5.6]";

		out << L"\n[1-4] Current Edge: 1";

		// Inside weights
			// 1 or 2
		if( QUAD_MODE == m_bGeometryMode )
			out << L"\nInside Weights: [1.2, 3.4]";
		else if( TRI_MODE == m_bGeometryMode )
			out << L"\nInside Weight: [1.2]";

		out << L"\n[5-6] Current Inner Weight: 1";

		// Partitioning mode
			// int
			// pow2
			// fractional_odd
			// fractional_even
		out << L"\nP : Change Partitioning Mode ('pow2')";

		m_pSpriteRenderer->RenderText( m_pRenderer11->pImmPipeline, m_pRenderer11->m_pParamMgr, *m_pFont, out.str().c_str(), Matrix4f::Identity(), Vector4f( 1.f, 0.f, 0.f, 1.f ) );
	}

	// Present the final image to the screen
	m_pRenderer11->Present( m_pWindow->GetHandle(), m_pWindow->GetSwapChain() );

	// Save a screenshot if desired.  This is done by pressing the 's' key, which
	// demonstrates how an event is sent and handled by an event listener (which
	// in this case is the application object itself).

	if ( m_bSaveScreenshot  )
	{
		m_bSaveScreenshot = false;
		m_pRenderer11->pImmPipeline->SaveTextureScreenShot( 0, std::wstring( L"TessParamsDemo_" ), D3DX11_IFF_BMP );
	}
}
//--------------------------------------------------------------------------------
void App::Shutdown()
{
	// Safely dispose of our rendering resource
	SAFE_RELEASE( m_pQuadGeometry );
	SAFE_DELETE( m_pQuadEffect );

	SAFE_RELEASE( m_pTriangleGeometry );
	SAFE_DELETE( m_pTriangleEffect );

	SAFE_DELETE( m_pFont );
	SAFE_DELETE( m_pSpriteRenderer );

	// Print the framerate out for the log before shutting down.
	std::wstringstream out;
	out << L"Max FPS: " << m_pTimer->MaxFramerate();
	Log::Get().Write( out.str() );
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
		else if ( 'G' == key  )
		{
			m_bGeometryMode = !m_bGeometryMode;
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
	return( std::wstring( L"Direct3D 11 Tessellation Parameters Demo" ) );
}
//--------------------------------------------------------------------------------
void App::CreateQuadResources()
{
	SAFE_RELEASE( m_pQuadGeometry );
	m_pQuadGeometry = new GeometryDX11();

	// Create only four vertices and no index buffer

	// Position Data
	VertexElementDX11 *pPositions = new VertexElementDX11( 3 /* components - xyz */, 4 /* elements */ );

	pPositions->m_SemanticName = "CONTROL_POINT_POSITION";
	pPositions->m_uiSemanticIndex = 0;
	pPositions->m_Format = DXGI_FORMAT_R32G32B32_FLOAT;
	pPositions->m_uiInputSlot = 0;
	pPositions->m_uiAlignedByteOffset = 0;
	pPositions->m_InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	pPositions->m_uiInstanceDataStepRate = 0;

	Vector3f *pPositionData = pPositions->Get3f( 0 );

	pPositionData[0] = Vector3f( -1.0f, 0.0f, -1.0f );
	pPositionData[1] = Vector3f( -1.0f, 0.0f,  1.0f );
	pPositionData[2] = Vector3f(  1.0f, 0.0f, -1.0f );
	pPositionData[3] = Vector3f(  1.0f, 0.0f,  1.0f );

	// Colour Data
	VertexElementDX11 *pColours = new VertexElementDX11( 4 /* components - argb */, 4 /* elements */ );

	pColours->m_SemanticName = "COLOUR";
	pColours->m_uiSemanticIndex = 0;
	pColours->m_Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	pColours->m_uiInputSlot = 0;
	pColours->m_uiAlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	pColours->m_InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	pColours->m_uiInstanceDataStepRate = 0;

	Vector4f *pColourData = pColours->Get4f( 0 );

	pColourData[0] = Vector4f( 0.0f, 0.0f, 0.0f, 1.0f );
	pColourData[1] = Vector4f( 0.0f, 0.0f, 0.0f, 1.0f );
	pColourData[2] = Vector4f( 0.0f, 0.0f, 0.0f, 1.0f );
	pColourData[3] = Vector4f( 0.0f, 0.0f, 0.0f, 1.0f );

	// Finally build the actual geometry buffer
	m_pQuadGeometry->AddElement( pPositions );
	m_pQuadGeometry->AddElement( pColours );

	m_pQuadGeometry->AddIndex( 0 );
	m_pQuadGeometry->AddIndex( 1 );
	m_pQuadGeometry->AddIndex( 2 );
	m_pQuadGeometry->AddIndex( 3 );

	m_pQuadGeometry->LoadToBuffers();
	m_pQuadGeometry->SetPrimitiveType( D3D11_PRIMITIVE_TOPOLOGY_4_CONTROL_POINT_PATCHLIST );


	Log::Get().Write( L"Created quad geometry" );

	// Create the appropriate shaders
	SAFE_DELETE( m_pQuadEffect );
	m_pQuadEffect = new RenderEffectDX11();
	m_pQuadEffect->m_iVertexShader = 
		m_pRenderer11->LoadShader( VERTEX_SHADER,
		std::wstring( L"../Data/Shaders/TessellationParameters.hlsl" ),
		std::wstring( L"vsMain" ),
		std::wstring( L"vs_5_0" ) );
	m_pQuadEffect->m_iHullShader =
		m_pRenderer11->LoadShader( HULL_SHADER,
		std::wstring( L"../Data/Shaders/TessellationParameters.hlsl" ),
		std::wstring( L"hsQuadMain" ),
		std::wstring( L"hs_5_0" ) );
	m_pQuadEffect->m_iDomainShader =
		m_pRenderer11->LoadShader( DOMAIN_SHADER,
		std::wstring( L"../Data/Shaders/TessellationParameters.hlsl" ),
		std::wstring( L"dsQuadMain" ),
		std::wstring( L"ds_5_0" ) );
	m_pQuadEffect->m_iGeometryShader =
		m_pRenderer11->LoadShader( GEOMETRY_SHADER,
		std::wstring( L"../Data/Shaders/TessellationParameters.hlsl" ),
		std::wstring( L"gsMain" ),
		std::wstring( L"gs_5_0" ) );
	m_pQuadEffect->m_iPixelShader = 
		m_pRenderer11->LoadShader( PIXEL_SHADER,
		std::wstring( L"../Data/Shaders/TessellationParameters.hlsl" ),
		std::wstring( L"psMain" ),
		std::wstring( L"ps_5_0" ) );

	RasterizerStateConfigDX11 RS;
	RS.FillMode = D3D11_FILL_WIREFRAME;
	m_pQuadEffect->m_iRasterizerState = m_pRenderer11->CreateRasterizerState( &RS );

	Log::Get().Write( L"Created quad shaders" );
}
//--------------------------------------------------------------------------------
void App::CreateTriangleResources()
{
	SAFE_RELEASE( m_pTriangleGeometry );
	m_pTriangleGeometry = new GeometryDX11();

	// Create only three vertices and no index buffer

	// Position Data
	VertexElementDX11 *pPositions = new VertexElementDX11( 3 /* components - xyz */, 3 /* elements */ );

	pPositions->m_SemanticName = "CONTROL_POINT_POSITION";
	pPositions->m_uiSemanticIndex = 0;
	pPositions->m_Format = DXGI_FORMAT_R32G32B32_FLOAT;
	pPositions->m_uiInputSlot = 0;
	pPositions->m_uiAlignedByteOffset = 0;
	pPositions->m_InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	pPositions->m_uiInstanceDataStepRate = 0;

	Vector3f *pPositionData = pPositions->Get3f( 0 );

	pPositionData[0] = Vector3f( -1.0f, 0.0f, -1.0f );
	pPositionData[1] = Vector3f( -1.0f, 0.0f,  1.0f );
	pPositionData[2] = Vector3f(  1.0f, 0.0f, -1.0f );

	// Colour Data
	VertexElementDX11 *pColours = new VertexElementDX11( 4 /* components - argb */, 3 /* elements */ );

	pColours->m_SemanticName = "COLOUR";
	pColours->m_uiSemanticIndex = 0;
	pColours->m_Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	pColours->m_uiInputSlot = 0;
	pColours->m_uiAlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	pColours->m_InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	pColours->m_uiInstanceDataStepRate = 0;

	Vector4f *pColourData = pColours->Get4f( 0 );

	pColourData[0] = Vector4f( 0.0f, 0.0f, 0.0f, 1.0f );
	pColourData[1] = Vector4f( 0.0f, 0.0f, 0.0f, 1.0f );
	pColourData[2] = Vector4f( 0.0f, 0.0f, 0.0f, 1.0f );

	// Finally build the actual geometry buffer
	m_pTriangleGeometry->AddElement( pPositions );
	m_pTriangleGeometry->AddElement( pColours );

	m_pTriangleGeometry->AddIndex( 0 );
	m_pTriangleGeometry->AddIndex( 1 );
	m_pTriangleGeometry->AddIndex( 2 );

	m_pTriangleGeometry->LoadToBuffers();
	m_pTriangleGeometry->SetPrimitiveType( D3D11_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST );

	Log::Get().Write( L"Created triangle geometry" );

	// Create the appropriate shaders
	SAFE_DELETE( m_pTriangleEffect );
	m_pTriangleEffect = new RenderEffectDX11();
	m_pTriangleEffect->m_iVertexShader = 
		m_pRenderer11->LoadShader( VERTEX_SHADER,
		std::wstring( L"../Data/Shaders/TessellationParameters.hlsl" ),
		std::wstring( L"vsMain" ),
		std::wstring( L"vs_5_0" ) );
	m_pTriangleEffect->m_iHullShader =
		m_pRenderer11->LoadShader( HULL_SHADER,
		std::wstring( L"../Data/Shaders/TessellationParameters.hlsl" ),
		std::wstring( L"hsTriangleMain" ),
		std::wstring( L"hs_5_0" ) );
	m_pTriangleEffect->m_iDomainShader =
		m_pRenderer11->LoadShader( DOMAIN_SHADER,
		std::wstring( L"../Data/Shaders/TessellationParameters.hlsl" ),
		std::wstring( L"dsTriangleMain" ),
		std::wstring( L"ds_5_0" ) );
	m_pTriangleEffect->m_iGeometryShader =
		m_pRenderer11->LoadShader( GEOMETRY_SHADER,
		std::wstring( L"../Data/Shaders/TessellationParameters.hlsl" ),
		std::wstring( L"gsMain" ),
		std::wstring( L"gs_5_0" ) );
	m_pTriangleEffect->m_iPixelShader = 
		m_pRenderer11->LoadShader( PIXEL_SHADER,
		std::wstring( L"../Data/Shaders/TessellationParameters.hlsl" ),
		std::wstring( L"psMain" ),
		std::wstring( L"ps_5_0" ) );

	RasterizerStateConfigDX11 RS;
	RS.FillMode = D3D11_FILL_WIREFRAME;
	m_pTriangleEffect->m_iRasterizerState = m_pRenderer11->CreateRasterizerState( &RS );

	Log::Get().Write( L"Created triangle shaders" );
}
//--------------------------------------------------------------------------------