//--------------------------------------------------------------------------------
// This file is a portion of the Hieroglyph 3 Rendering Engine.  It is distributed
// under the MIT License, available in the root of this distribution and 
// at the following URL:
//
// http://www.opensource.org/licenses/mit-license.php
//
// Copyright (c) Jason Zink 
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
	m_bGeometryMode = QUAD_MODE;

	m_bEdgeOrInsideMode = EDGE_MODE;
}
//--------------------------------------------------------------------------------
bool App::ConfigureEngineComponents()
{
	// The application currently supplies the 
	int width = 640;
	int height = 480;
	bool windowed = true;

	// Set the render window parameters and initialize the window
	m_pWindow = new Win32RenderWindow();
	m_pWindow->SetPosition( 25, 25 );
	m_pWindow->SetSize( width, height );
	m_pWindow->SetCaption( GetName() );
	m_pWindow->Initialize( this );

	
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
	m_pRenderer11->pImmPipeline->OutputMergerStage.DesiredState.RenderTargetViews.SetState( 0, m_RenderTarget->m_iResourceRTV );
	m_pRenderer11->pImmPipeline->OutputMergerStage.DesiredState.DepthTargetViews.SetState( m_DepthTarget->m_iResourceDSV );
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
	m_pRenderer11->pImmPipeline->RasterizerStage.DesiredState.ViewportCount.SetState( 1 );
	m_pRenderer11->pImmPipeline->RasterizerStage.DesiredState.Viewports.SetState( 0, ViewPort );
	
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
	// Create the necessary resources
	CreateQuadResources();
	CreateTriangleResources();

	// We'll be using a static view for this sample; animation/viewpoints aren't
	// the focus of this demo. Initialize and store these variables here.

	Matrix4f mView;
	Vector3f vLookAt = Vector3f( 0.0f, 0.0f, 0.0f );
	Vector3f vLookFrom = Vector3f( -2.0f, 2.0f, -2.0f );
	Vector3f vLookUp = Vector3f( 0.0f, 1.0f, 0.0f );
	mView = Matrix4f::LookAtLHMatrix( vLookFrom, vLookAt, vLookUp );

	Matrix4f mWorld;
	mWorld.MakeIdentity();

	Matrix4f mProj;
	mProj = Matrix4f::PerspectiveFovLHMatrix( static_cast< float >(GLYPH_PI) / 4.0f, 640.0f /  480.0f, 0.1f, 50.0f );

	// Composite together for the final transform
	Matrix4f mViewProj = mView * mProj;

	// H3 engine requires no transposing of matrices...

	m_pRenderer11->m_pParamMgr->SetWorldMatrixParameter( &mWorld );
	m_pRenderer11->m_pParamMgr->SetViewMatrixParameter( &mView );
	m_pRenderer11->m_pParamMgr->SetProjMatrixParameter( &mProj );
	//m_pRenderer11->m_pParamMgr->SetMatrixParameter( std::wstring( L"mWorld" ), &mWorld );
	//m_pRenderer11->m_pParamMgr->SetMatrixParameter( std::wstring( L"mViewProj" ), &mViewProj );

	// Set up the default tessellation params
	SetEdgeWeight( 0, 1.0f );
	SetEdgeWeight( 1, 1.0f );
	SetEdgeWeight( 2, 1.0f );
	SetEdgeWeight( 3, 1.0f );

	SetInsideWeight( 0, 1.0f );
	SetInsideWeight( 1, 1.0f );

	SetPartitioningMode(Integer);

	// Create the text rendering
	m_pFont = SpriteFontLoaderDX11::LoadFont( std::wstring( L"Consolas" ), 12.0f, 0, true );
	
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

	EvtManager.ProcessEvent( EvtFrameStartPtr( new EvtFrameStart( m_pTimer->Elapsed() ) ) );

	// Clear the window to white
	m_pRenderer11->pImmPipeline->ClearBuffers( Vector4f( 1.0f, 1.0f, 1.0f, 1.0f ), 1.0f );

	// Draw the main geometry
	if( QUAD_MODE == m_bGeometryMode )
	{
		//m_pQuadEffect->ConfigurePipeline( m_pRenderer11->pImmPipeline, m_pRenderer11->m_pParamMgr );
		m_pRenderer11->pImmPipeline->Draw( *m_pQuadEffect, m_pQuadGeometry, m_pRenderer11->m_pParamMgr ); 
	}
	else if( TRI_MODE == m_bGeometryMode )
	{
		//m_pTriangleEffect->ConfigurePipeline( m_pRenderer11->pImmPipeline, m_pRenderer11->m_pParamMgr );
		m_pRenderer11->pImmPipeline->Draw( *m_pTriangleEffect, m_pTriangleGeometry, m_pRenderer11->m_pParamMgr ); 
	}

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
			out << L"\nEdge Weights: [" << m_fEdgeWeights[0] << ", " << m_fEdgeWeights[1] << ", " << m_fEdgeWeights[2] << ", " << m_fEdgeWeights[3] << "]";
		else if( TRI_MODE == m_bGeometryMode )
			out << L"\nEdge Weights: [" << m_fEdgeWeights[0] << ", " << m_fEdgeWeights[1] << ", " << m_fEdgeWeights[2] << "]";

		out << L"\nE : Toggle Edge";

		// Inside weights
			// 1 or 2
		if( QUAD_MODE == m_bGeometryMode )
			out << L"\nInside Weights: [" << m_fInsideWeights[0] << ", " << m_fInsideWeights[1] << "]";
		else if( TRI_MODE == m_bGeometryMode )
			out << L"\nInside Weight: [" << m_fInsideWeights[0] << "]";

		out << L"\nI : Toggle Inner Weight";

		out << L"\n+/- : Change Weight of ";
		if(EDGE_MODE == m_bEdgeOrInsideMode)
			out << L"Edge " << m_iEdgeIndex;
		else
			out << L"Inside " << m_iInsideIndex;


		// Partitioning mode
			// int
			// pow2
			// fractional_odd
			// fractional_even
		out << L"\nP : Change Partitioning Mode ";
		switch(m_pmPartitioningMode)
		{
			case Power2:			out << L"('pow2')"; break;
			case Integer:			out << L"('integer')"; break;
			case FractionalOdd:		out << L"('fractional_odd')"; break;
			case FractionalEven:	out << L"('fractional_even')"; break;
		}

		m_pSpriteRenderer->RenderText( m_pRenderer11->pImmPipeline, m_pRenderer11->m_pParamMgr, m_pFont, out.str().c_str(), Matrix4f::Identity(), Vector4f( 1.f, 0.f, 0.f, 1.f ) );
	}

	// Present the final image to the screen
	m_pRenderer11->Present( m_pWindow->GetHandle(), m_pWindow->GetSwapChain() );

}
//--------------------------------------------------------------------------------
void App::Shutdown()
{
	// Safely dispose of our rendering resource
	m_pQuadGeometry = NULL;
	SAFE_DELETE( m_pQuadEffect );

	m_pTriangleGeometry = NULL;
	SAFE_DELETE( m_pTriangleEffect );

	SAFE_DELETE( m_pSpriteRenderer );

	// Print the framerate out for the log before shutting down.
	std::wstringstream out;
	out << L"Max FPS: " << m_pTimer->MaxFramerate();
	Log::Get().Write( out.str() );
}
//--------------------------------------------------------------------------------
void App::TakeScreenShot()
{
	if ( m_bSaveScreenshot  )
	{
		m_bSaveScreenshot = false;
		m_pRenderer11->pImmPipeline->SaveTextureScreenShot( 0, GetName() );
	}
}
//--------------------------------------------------------------------------------
bool App::HandleEvent( EventPtr pEvent )
{
	eEVENT e = pEvent->GetEventType();

	if ( e == SYSTEM_KEYBOARD_KEYDOWN )
	{
		EvtKeyDownPtr pKeyDown = std::static_pointer_cast<EvtKeyDown>( pEvent );

		unsigned int key = pKeyDown->GetCharacterCode();
	}
	else if ( e == SYSTEM_KEYBOARD_KEYUP )
	{
		EvtKeyUpPtr pKeyUp = std::static_pointer_cast<EvtKeyUp>( pEvent );

		unsigned int key = pKeyUp->GetCharacterCode();

		if ( 'G' == key  )
		{
			m_bGeometryMode = !m_bGeometryMode;
		}
		else if ( 'P' == key )
		{
			// Change partitioning mode
			switch(m_pmPartitioningMode)
			{
				case Power2:			SetPartitioningMode( Integer ); break;
				case Integer:			SetPartitioningMode( FractionalOdd ); break;
				case FractionalOdd:		SetPartitioningMode( FractionalEven ); break;
				case FractionalEven:	SetPartitioningMode( Power2 ); break;
			}
		}
		else if ( 'E' == key )
		{
			// Set to 'edge' mode
			// if already in mode, toggle which to use
			if(EDGE_MODE == m_bEdgeOrInsideMode)
			{
				m_iEdgeIndex = (m_iEdgeIndex + 1) % (TRI_MODE == m_bGeometryMode ? 3 : 4);
			}
			else
			{
				m_bEdgeOrInsideMode = EDGE_MODE;
			}
		}
		else if ( 'I' == key )
		{
			// Set to 'inside' mode
			// if already in mode, toggle which to use (if quad)
			if(INSIDE_MODE == m_bEdgeOrInsideMode)
			{
				m_iInsideIndex = (m_iInsideIndex + 1) % (TRI_MODE == m_bGeometryMode ? 1 : 2);
			}
			else
			{
				m_bEdgeOrInsideMode = INSIDE_MODE;
			}
		}
		else if ( VK_ADD == key )
		{
			// Increase weight
			if(EDGE_MODE == m_bEdgeOrInsideMode)
			{
				SetEdgeWeight( m_iEdgeIndex, m_fEdgeWeights[m_iEdgeIndex] + 0.1f );
			}
			else
			{
				SetInsideWeight( m_iInsideIndex, m_fInsideWeights[m_iInsideIndex] + 0.1f );
			}
		}
		else if ( VK_SUBTRACT == key )
		{
			// Decrease weight
			if(EDGE_MODE == m_bEdgeOrInsideMode)
			{
				SetEdgeWeight( m_iEdgeIndex, m_fEdgeWeights[m_iEdgeIndex] - 0.1f );
			}
			else
			{
				SetInsideWeight( m_iInsideIndex, m_fInsideWeights[m_iInsideIndex] - 0.1f );
			}
		}
	}

	// Call the parent class's event handler if we haven't handled the event.
	
	return( Application::HandleEvent( pEvent ) );
}
//--------------------------------------------------------------------------------
std::wstring App::GetName( )
{
	return( std::wstring( L"Direct3D 11 Tessellation Parameters Demo" ) );
}
//--------------------------------------------------------------------------------
void App::CreateQuadResources()
{
	m_pQuadGeometry = NULL;
	m_pQuadGeometry = GeometryPtr( new GeometryDX11() );

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

	m_pQuadEffect->SetVertexShader( m_pRenderer11->LoadShader( VERTEX_SHADER,
		std::wstring( L"TessellationParameters.hlsl" ),
		std::wstring( L"vsMain" ),
		std::wstring( L"vs_5_0" ) ) );
	_ASSERT( -1 != m_pQuadEffect->GetVertexShader() );

	D3D_SHADER_MACRO hsPow2Mode[2] = { "POW2_PARTITIONING", "1", NULL, NULL };
	m_QuadHullShaders[Power2] = m_pRenderer11->LoadShader( HULL_SHADER, std::wstring( L"TessellationParameters.hlsl" ), std::wstring( L"hsQuadMain" ), std::wstring( L"hs_5_0" ), hsPow2Mode );
	_ASSERT( -1 != m_QuadHullShaders[Power2] );

	D3D_SHADER_MACRO hsIntMode[2] = { "INTEGER_PARTITIONING", "1", NULL, NULL };
	m_QuadHullShaders[Integer] = m_pRenderer11->LoadShader( HULL_SHADER, std::wstring( L"TessellationParameters.hlsl" ), std::wstring( L"hsQuadMain" ), std::wstring( L"hs_5_0" ), hsIntMode );
	_ASSERT( -1 != m_QuadHullShaders[Integer] );

	D3D_SHADER_MACRO hsFracOddMode[2] = { "FRAC_ODD_PARTITIONING", "1", NULL, NULL };
	m_QuadHullShaders[FractionalOdd] = m_pRenderer11->LoadShader( HULL_SHADER, std::wstring( L"TessellationParameters.hlsl" ), std::wstring( L"hsQuadMain" ), std::wstring( L"hs_5_0" ), hsFracOddMode );
	_ASSERT( -1 != m_QuadHullShaders[FractionalOdd] );

	D3D_SHADER_MACRO hsFracEvenMode[2] = { "FRAC_EVEN_PARTITIONING", "1", NULL, NULL };
	m_QuadHullShaders[FractionalEven] = m_pRenderer11->LoadShader( HULL_SHADER, std::wstring( L"TessellationParameters.hlsl" ), std::wstring( L"hsQuadMain" ), std::wstring( L"hs_5_0" ), hsFracEvenMode );
	_ASSERT( -1 != m_QuadHullShaders[FractionalEven] );

	m_pQuadEffect->SetDomainShader( m_pRenderer11->LoadShader( DOMAIN_SHADER,
		std::wstring( L"TessellationParameters.hlsl" ),
		std::wstring( L"dsQuadMain" ),
		std::wstring( L"ds_5_0" ) ) );
	_ASSERT( -1 != m_pQuadEffect->GetDomainShader() );

	m_pQuadEffect->SetGeometryShader( m_pRenderer11->LoadShader( GEOMETRY_SHADER,
		std::wstring( L"TessellationParameters.hlsl" ),
		std::wstring( L"gsMain" ),
		std::wstring( L"gs_5_0" ) ) );
	_ASSERT( -1 != m_pQuadEffect->GetGeometryShader() );

	m_pQuadEffect->SetPixelShader( m_pRenderer11->LoadShader( PIXEL_SHADER,
		std::wstring( L"TessellationParameters.hlsl" ),
		std::wstring( L"psMain" ),
		std::wstring( L"ps_5_0" ) ) );
	_ASSERT( -1 != m_pQuadEffect->GetPixelShader() );

	RasterizerStateConfigDX11 RS;
	RS.FillMode = D3D11_FILL_WIREFRAME;
	m_pQuadEffect->m_iRasterizerState = m_pRenderer11->CreateRasterizerState( &RS );

	Log::Get().Write( L"Created quad shaders" );
}
//--------------------------------------------------------------------------------
void App::CreateTriangleResources()
{
	m_pTriangleGeometry = NULL;
	m_pTriangleGeometry = GeometryPtr( new GeometryDX11() );

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
	
	m_pTriangleEffect->SetVertexShader( m_pRenderer11->LoadShader( VERTEX_SHADER,
		std::wstring( L"TessellationParameters.hlsl" ),
		std::wstring( L"vsMain" ),
		std::wstring( L"vs_5_0" ) ) );
	_ASSERT( -1 != m_pTriangleEffect->GetVertexShader() );

	D3D_SHADER_MACRO hsPow2Mode[2] = { "POW2_PARTITIONING", "1", NULL, NULL };
	m_TriangleHullShaders[Power2] = m_pRenderer11->LoadShader( HULL_SHADER, std::wstring( L"TessellationParameters.hlsl" ), std::wstring( L"hsTriangleMain" ), std::wstring( L"hs_5_0" ), hsPow2Mode );
	_ASSERT( -1 != m_TriangleHullShaders[Power2] );

	D3D_SHADER_MACRO hsIntMode[2] = { "INTEGER_PARTITIONING", "1", NULL, NULL };
	m_TriangleHullShaders[Integer] = m_pRenderer11->LoadShader( HULL_SHADER, std::wstring( L"TessellationParameters.hlsl" ), std::wstring( L"hsTriangleMain" ), std::wstring( L"hs_5_0" ), hsIntMode );
	_ASSERT( -1 != m_TriangleHullShaders[Integer] );

	D3D_SHADER_MACRO hsFracOddMode[2] = { "FRAC_ODD_PARTITIONING", "1", NULL, NULL };
	m_TriangleHullShaders[FractionalOdd] = m_pRenderer11->LoadShader( HULL_SHADER, std::wstring( L"TessellationParameters.hlsl" ), std::wstring( L"hsTriangleMain" ), std::wstring( L"hs_5_0" ), hsFracOddMode );
	_ASSERT( -1 != m_TriangleHullShaders[FractionalOdd] );

	D3D_SHADER_MACRO hsFracEvenMode[2] = { "FRAC_EVEN_PARTITIONING", "1", NULL, NULL };
	m_TriangleHullShaders[FractionalEven] = m_pRenderer11->LoadShader( HULL_SHADER, std::wstring( L"TessellationParameters.hlsl" ), std::wstring( L"hsTriangleMain" ), std::wstring( L"hs_5_0" ), hsFracEvenMode );
	_ASSERT( -1 != m_TriangleHullShaders[FractionalEven] );

	m_pTriangleEffect->SetDomainShader( m_pRenderer11->LoadShader( DOMAIN_SHADER,
		std::wstring( L"TessellationParameters.hlsl" ),
		std::wstring( L"dsTriangleMain" ),
		std::wstring( L"ds_5_0" ) ) );
	_ASSERT( -1 != m_pTriangleEffect->GetDomainShader() );

	m_pTriangleEffect->SetGeometryShader( m_pRenderer11->LoadShader( GEOMETRY_SHADER,
		std::wstring( L"TessellationParameters.hlsl" ),
		std::wstring( L"gsMain" ),
		std::wstring( L"gs_5_0" ) ) );
	_ASSERT( -1 != m_pTriangleEffect->GetGeometryShader() );

	m_pTriangleEffect->SetPixelShader( m_pRenderer11->LoadShader( PIXEL_SHADER,
		std::wstring( L"TessellationParameters.hlsl" ),
		std::wstring( L"psMain" ),
		std::wstring( L"ps_5_0" ) ) );
	_ASSERT( -1 != m_pTriangleEffect->GetPixelShader() );

	RasterizerStateConfigDX11 RS;
	RS.FillMode = D3D11_FILL_WIREFRAME;
	m_pTriangleEffect->m_iRasterizerState = m_pRenderer11->CreateRasterizerState( &RS );

	Log::Get().Write( L"Created triangle shaders" );
}
//--------------------------------------------------------------------------------
void App::SetEdgeWeight(unsigned int index, float weight)
{
	// Check index is in range
	if(index > (unsigned)(TRI_MODE == m_bGeometryMode ? 2 : 3))
		return;

	// Check weight is in range
	if( (weight < 1.0f) || (weight > 64.0f) )
		return;

	// Store internal state
	m_fEdgeWeights[ index ] = weight;

	// Update shader state
	Vector4f v = Vector4f( m_fEdgeWeights[0], m_fEdgeWeights[1], m_fEdgeWeights[2], m_fEdgeWeights[3] );
	m_pRenderer11->m_pParamMgr->SetVectorParameter( std::wstring( L"vEdgeWeights" ), &v );
}
//--------------------------------------------------------------------------------
void App::SetInsideWeight(unsigned int index, float weight)
{
	// Check index is in range
	if(index > (unsigned)(TRI_MODE == m_bGeometryMode ? 0 : 1))
		return;

	// Check weight is in range
	if( (weight < 1.0f) || (weight > 64.0f) )
		return;

	// Store internal state
	m_fInsideWeights[ index ] = weight;

	// Update shader state
	Vector4f v = Vector4f( m_fInsideWeights[0], m_fInsideWeights[1], 0.0f, 0.0f );
	m_pRenderer11->m_pParamMgr->SetVectorParameter( std::wstring( L"vInsideWeights" ), &v );
}
//--------------------------------------------------------------------------------
void App::SetPartitioningMode( const PartitioningMode& mode )
{
	m_pmPartitioningMode = mode;

	// Update the tri HS
	m_pTriangleEffect->SetHullShader( m_TriangleHullShaders[ m_pmPartitioningMode ] );

	// Update the quad HS
	m_pQuadEffect->SetHullShader( m_QuadHullShaders[ m_pmPartitioningMode ] );
}
//--------------------------------------------------------------------------------