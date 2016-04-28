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
#include "BufferConfigDX11.h"
#include "MaterialGeneratorDX11.h"

#include "IParameterManager.h"

using namespace Glyph3;
//--------------------------------------------------------------------------------
App AppInstance; // Provides an instance of the application
//--------------------------------------------------------------------------------


//--------------------------------------------------------------------------------
App::App()
{
	m_bSolidRender				= false;
	m_bDefaultComplexity		= true;
	m_fTessFactor				= 3.0f;
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
	m_pWindow->SetCaption( GetName( ) );
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
	m_pRenderer11->pImmPipeline->RasterizerStage.DesiredState.RasterizerState.SetState( 0 );
	
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
	// Load and initialize the geometry to be rendered.

	//m_pGeometry = GeometryLoaderDX11::loadStanfordPlyFile( std::wstring( L"spaceship2.ply" ), true );
	m_pGeometry = GeometryLoaderDX11::loadStanfordPlyFile( std::wstring( L"CPNTest.ply" ) );
	//m_pGeometry = GeometryLoaderDX11::loadStanfordPlyFile( std::wstring( L"BoxWithBadNormals.ply" ), true );
	//m_pGeometry = GeometryLoaderDX11::loadStanfordPlyFile( std::wstring( L"CPNAdaptiveTest.ply" ), true );

	//m_pGeometry = GeometryLoaderDX11::loadMS3DFile2( std::wstring( L"box.ms3d" ) );
	//m_pGeometry->LoadToBuffers();
	//m_pGeometry->SetPrimitiveType( D3D11_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST );	
	
	// Create the material for use by the entities.
	CreateShaders();

	// Create the parameters for use with this effect
	m_pEdgeFactors = m_pRenderer11->m_pParamMgr->GetVectorParameterRef( std::wstring( L"EdgeFactors" ) );
	Vector4f tessParams = Vector4f( m_fTessFactor, m_fTessFactor, m_fTessFactor, m_fTessFactor );
	m_pRenderer11->m_pParamMgr->SetVectorParameter( m_pEdgeFactors, &tessParams );


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

	// Update any animation/camera config
	UpdateViewState();

	// Clear the window to white
	m_pRenderer11->pImmPipeline->ClearBuffers( Vector4f( 1.0f, 1.0f, 1.0f, 1.0f ), 1.0f );

	// Draw the main geometry
	m_pEffect->ConfigurePipeline( m_pRenderer11->pImmPipeline, m_pRenderer11->m_pParamMgr );

	m_pRenderer11->pImmPipeline->StartPipelineStatistics();
	{
		m_pRenderer11->pImmPipeline->Draw( *m_pEffect, m_pGeometry, m_pRenderer11->m_pParamMgr );
	}
	m_pRenderer11->pImmPipeline->EndPipelineStatistics();

	// Draw the UI text
	if( !m_bSaveScreenshot )
	{
		// Don't draw text if we're taking a screenshot - don't want the images
		// cluttered with UI text!
		std::wstringstream out;
		out << L"Hieroglyph 3 : Curved Point Normal Triangles\nFPS: " << m_pTimer->Framerate();

		const D3D11_QUERY_DATA_PIPELINE_STATISTICS& stats = m_pRenderer11->pImmPipeline->m_PipelineStatsData;
		int trisSent;
		int trisRasterized;
		float rasterPerc;

		trisSent = static_cast<int>(stats.IAPrimitives);
		trisRasterized = static_cast<int>(stats.CPrimitives);
		rasterPerc = 0.0f;
		if(0!=trisSent)
			rasterPerc = 100.0f * (static_cast<float>(trisRasterized) / static_cast<float>(trisSent));		

		// Stats
		out << L"\nTriangles: " << trisSent << " sent, " << trisRasterized << " rasterized (" << rasterPerc << "%)";

		// Screenshot
		out << L"\n S : Take Screenshot";

		// Rasterizer State
		out << L"\n W : Toggle Wireframe Display";

		// Advanced LoD
		out << L"\n +/- : Increase or Decrease Tessellation (" << m_fTessFactor << L")";

		// Automatic Rotation
		out << L"\n A : Toggle Adaptive Sillhouette Tessellation";

		m_pSpriteRenderer->RenderText( m_pRenderer11->pImmPipeline, m_pRenderer11->m_pParamMgr, m_pFont, out.str().c_str(), Matrix4f::Identity(), Vector4f( 1.f, 0.f, 0.f, 1.f ) );
	}

	// Present the results of the rendering to the output window.
	m_pRenderer11->Present( m_pWindow->GetHandle(), m_pWindow->GetSwapChain() );

#ifdef _DEBUG
	Log::Get().Write( m_pRenderer11->pImmPipeline->PrintPipelineStatistics() );
#endif

}
//--------------------------------------------------------------------------------
void App::Shutdown()
{
	m_pGeometry = NULL;
	
	SAFE_DELETE( m_pEffect );
	SAFE_DELETE( m_pCamera );

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

		if ( 'W' == key )
		{
			// Toggle Wireframe
			m_bSolidRender = !m_bSolidRender;
			m_pEffect->m_iRasterizerState = m_bSolidRender ? m_rsSolid : m_rsWireframe;
		}
		else if ( 'A' == key )
		{
			// Toggle adaptive/uniform
			m_bDefaultComplexity = !m_bDefaultComplexity;
			m_pEffect->SetHullShader( m_bDefaultComplexity ? m_iDefaultHullShader : m_iSilhouetteHullShader );
		}
		else if ( VK_ADD == key )
		{
			m_fTessFactor += 0.25f;
			
			if(m_fTessFactor > 10.0f)
				m_fTessFactor = 10.0f;

			Vector4f tessParams = Vector4f( m_fTessFactor, m_fTessFactor, m_fTessFactor, m_fTessFactor );
			m_pRenderer11->m_pParamMgr->SetVectorParameter( m_pEdgeFactors, &tessParams );
		}
		else if ( VK_SUBTRACT == key )
		{
			m_fTessFactor -= 0.25f;
			
			if(m_fTessFactor < 1.0f)
				m_fTessFactor = 1.0f;

			Vector4f tessParams = Vector4f( m_fTessFactor, m_fTessFactor, m_fTessFactor, m_fTessFactor );
			m_pRenderer11->m_pParamMgr->SetVectorParameter( m_pEdgeFactors, &tessParams );
		}
	}

	// Call the parent class's event handler if we haven't handled the event.

	return( Application::HandleEvent( pEvent ) );
}
//--------------------------------------------------------------------------------
std::wstring App::GetName( )
{
	return( std::wstring( L"Direct3D 11 Curved Point Normal Triangles Demo" ) );
}
//--------------------------------------------------------------------------------
void App::CreateShaders()
{
	Log::Get().Write( L"Creating shaders" );

	SAFE_DELETE( m_pEffect );
	m_pEffect = new RenderEffectDX11();

	// Create the vertex shader
	m_pEffect->SetVertexShader( m_pRenderer11->LoadShader( VERTEX_SHADER,
		std::wstring( L"CurvedPointNormalTriangles.hlsl" ),
		std::wstring( L"vsMain" ),
		std::wstring( L"vs_5_0" ) ) );
	_ASSERT( -1 != m_pEffect->GetVertexShader() );
	
	Log::Get().Write( L"... vertex shader created" );

	// Create the hull shader
	m_iDefaultHullShader = m_pRenderer11->LoadShader( HULL_SHADER,
							std::wstring( L"CurvedPointNormalTriangles.hlsl" ),
							std::wstring( L"hsDefault" ),
							std::wstring( L"hs_5_0" ) );
	_ASSERT( -1 != m_iDefaultHullShader );

	m_iSilhouetteHullShader = m_pRenderer11->LoadShader( HULL_SHADER,
							std::wstring( L"CurvedPointNormalTriangles.hlsl" ),
							std::wstring( L"hsSilhouette" ),
							std::wstring( L"hs_5_0" ) );
	_ASSERT( -1 != m_iSilhouetteHullShader );

	m_pEffect->SetHullShader( m_bDefaultComplexity ? m_iDefaultHullShader : m_iSilhouetteHullShader );

	Log::Get().Write( L"... hull shader created" );

	// Create the domain shader
	m_pEffect->SetDomainShader( m_pRenderer11->LoadShader( DOMAIN_SHADER,
		std::wstring( L"CurvedPointNormalTriangles.hlsl" ),
		std::wstring( L"dsMain" ),
		std::wstring( L"ds_5_0" ) ) );
	_ASSERT( -1 != m_pEffect->GetDomainShader() );

	Log::Get().Write( L"... domain shader created" );

	// Create the geometry shader
	m_pEffect->SetGeometryShader( m_pRenderer11->LoadShader( GEOMETRY_SHADER,
		std::wstring( L"CurvedPointNormalTriangles.hlsl" ),
		std::wstring( L"gsMain" ),
		std::wstring( L"gs_5_0" ) ) );
	_ASSERT( -1 != m_pEffect->GetGeometryShader() );

	Log::Get().Write( L"... geometry shader created" );

	// Create the pixel shader
	m_pEffect->SetPixelShader( m_pRenderer11->LoadShader( PIXEL_SHADER,
		std::wstring( L"CurvedPointNormalTriangles.hlsl" ),
		std::wstring( L"psMain" ),
		std::wstring( L"ps_5_0" ) ) );
	_ASSERT( -1 != m_pEffect->GetPixelShader() );

	Log::Get().Write( L"... pixel shader created" );

	// Create rasterizer states
	RasterizerStateConfigDX11 RS;
	
	RS.FillMode = D3D11_FILL_WIREFRAME;
	RS.CullMode = D3D11_CULL_NONE;
	m_rsWireframe = m_pRenderer11->CreateRasterizerState( &RS );

	RS.FillMode = D3D11_FILL_SOLID;
	RS.CullMode = D3D11_CULL_BACK;
	m_rsSolid = m_pRenderer11->CreateRasterizerState( &RS );

	// Assign default state
	m_pEffect->m_iRasterizerState = m_bSolidRender ? m_rsSolid : m_rsWireframe;

	Log::Get().Write( L"Created all shaders" );
}
//--------------------------------------------------------------------------------
void App::UpdateViewState()
{
	// If in 'auto' mode then simply keep rotating
	float time = m_pTimer->Runtime();

	// Create the world matrix
	Matrix4f mWorld, mWorldScale, mWorldRotation;
	
	mWorld.MakeIdentity();
	
	// Geometry is defined in the [-0.5,+0.5] range on the XZ plane

	// Create the inverse transpose world matrix
	Matrix4f mInvTPoseWorld;

	mInvTPoseWorld = mWorld.Inverse();
	mInvTPoseWorld = mInvTPoseWorld.Transpose();
	
	Vector3f vLookAt = Vector3f( 0.0f, 0.0f, 0.0f );
	Vector3f vLookFrom = Vector3f( 0.0f, 0.0f, 0.0f );
	Vector3f vLookUp = Vector3f( 0.0f, 1.0f, 0.0f );

	// based on time, determine where the camera is at and where it should look to
	float distance = time / 30.0f; // 30 seconds to do a single circuit
	float fromAngle = fmodf( distance * 2.0f * static_cast<float>(GLYPH_PI), 2.0f * static_cast<float>(GLYPH_PI));
	//float toAngle = fmodf( (distance + 0.08f) * 2.0f * static_cast<float>(GLYPH_PI), 2.0f * static_cast<float>(GLYPH_PI)); // ~30 degrees in front

	//vLookFrom.x = 4.5f;//sinf(fromAngle) * 2.5f;
	//vLookFrom.y = 5.25f;
	//vLookFrom.z = 7.5f;//cosf(fromAngle) * 2.5f;
	
	vLookFrom.x = sinf(fromAngle) * 2.5f;
	vLookFrom.y = 1.25f;
	vLookFrom.z = cosf(fromAngle) * 2.5f;

	/*
	vLookAt.x = sinf(toAngle) * 3.0f;
	vLookAt.y = 0.3f;
	vLookAt.z = cosf(toAngle) * 3.0f;
	*/

	// Create the view matrix
	Matrix4f mView = Matrix4f::LookAtLHMatrix( vLookFrom, vLookAt, vLookUp );

	// Create the projection matrix
	Matrix4f mProj = Matrix4f::PerspectiveFovLHMatrix( static_cast< float >(GLYPH_PI) / 3.0f, 
		static_cast<float>(m_pWindow->GetWidth()) /  static_cast<float>(m_pWindow->GetHeight()), 1.0f, 25.0f );

	// Composite together for the final transform
	Matrix4f mViewProj = mView * mProj;

	// Set the various values to the parameter manager
	m_pRenderer11->m_pParamMgr->SetMatrixParameter( L"mWorld", &mWorld );
	m_pRenderer11->m_pParamMgr->SetMatrixParameter( L"mViewProj", &mViewProj );
	m_pRenderer11->m_pParamMgr->SetMatrixParameter( L"mInvTposeWorld", &mInvTPoseWorld );

	Vector4f vCam = Vector4f( vLookFrom.x, vLookFrom.y, vLookFrom.z, /* unused */ 0.0f );
	m_pRenderer11->m_pParamMgr->SetVectorParameter( L"cameraPosition", &vCam );

	Vector4f vCamLook = Vector4f( vLookAt.x, vLookAt.y, vLookAt.z, /* unused */ 0.0f );
	m_pRenderer11->m_pParamMgr->SetVectorParameter( L"cameraLookAt", &vCamLook );
}
//--------------------------------------------------------------------------------