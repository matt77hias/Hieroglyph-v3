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
#include "DepthStencilStateConfigDX11.h"
#include "IParameterManager.h"
#include "SamplerParameterDX11.h"
#include "ShaderResourceParameterDX11.h"

#define clamp(value,minimum,maximum) (max(min((value),(maximum)),(minimum)))

using namespace Glyph3;
//--------------------------------------------------------------------------------
App AppInstance; // Provides an instance of the application
//--------------------------------------------------------------------------------


//--------------------------------------------------------------------------------
App::App()
{
	m_bViewPointInAutoMode = true;
	m_bSolidRender = false;
	m_smCurrentShading = SolidColour; // best for wireframe
	m_bSimpleComplexity = true;
}
//--------------------------------------------------------------------------------
bool App::ConfigureEngineComponents()
{
	// The application currently supplies the 
	int width = 1024;
	int height = 768;
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
	m_pbufferResults = m_pRenderer11->m_pParamMgr->GetUnorderedAccessParameterRef( std::wstring( L"bufferResults" ) );
	m_ptexLODLookup = m_pRenderer11->m_pParamMgr->GetShaderResourceParameterRef( std::wstring( L"texLODLookup" ) );
	m_ptexHeightMap = m_pRenderer11->m_pParamMgr->GetShaderResourceParameterRef( std::wstring( L"texHeightMap" ) );
	
	m_pmWorld = m_pRenderer11->m_pParamMgr->GetMatrixParameterRef( std::wstring( L"mWorld" ) );
	m_pmViewProj = m_pRenderer11->m_pParamMgr->GetMatrixParameterRef( std::wstring( L"mViewProj" ) );
	m_pmInvTposeWorld = m_pRenderer11->m_pParamMgr->GetMatrixParameterRef( std::wstring( L"mInvTposeWorld" ) );

	m_pcameraPosition = m_pRenderer11->m_pParamMgr->GetVectorParameterRef( std::wstring( L"cameraPosition" ) );
	m_pheightMapDimensions = m_pRenderer11->m_pParamMgr->GetVectorParameterRef( std::wstring( L"heightMapDimensions" ) );
	m_pminMaxDistance = m_pRenderer11->m_pParamMgr->GetVectorParameterRef( std::wstring( L"minMaxDistance" ) );
	m_pminMaxLOD = m_pRenderer11->m_pParamMgr->GetVectorParameterRef( std::wstring( L"minMaxLOD" ) );

	m_psmpHeightMap = m_pRenderer11->m_pParamMgr->GetSamplerStateParameterRef( std::wstring( L"smpHeightMap" ) );


	// Create the necessary resources
	CreateTerrainGeometry();
	CreateTerrainShaders();
	CreateTerrainTextures();
	CreateComputeShaderResources();
	RunComputeShader();

	// Set initial shader values
	// (computed off camera pos + geometry matrix)
	Vector4f vMinMaxDist = Vector4f( 4.0f, 18.0f, /* unused */ 0.0f, /* unused */ 0.0f );
	m_pRenderer11->m_pParamMgr->SetVectorParameter( m_pminMaxDistance, &vMinMaxDist );

	Vector4f vMinMaxLod = Vector4f( 1.0f, 5.0f, /* unused */ 0.0f, /* unused */ 0.0f );
	m_pRenderer11->m_pParamMgr->SetVectorParameter( m_pminMaxLOD, &vMinMaxLod );

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

	// Process any new events
	EvtManager.ProcessEvent( EvtFrameStartPtr( new EvtFrameStart( m_pTimer->Elapsed() ) ) );

	// Update any animation/camera config
	UpdateViewState();

	// Clear the window to white
	m_pRenderer11->pImmPipeline->ClearBuffers( Vector4f( 1.0f, 1.0f, 1.0f, 1.0f ), 1.0f );

	// Draw the main geometry
	m_pTerrainEffect->ConfigurePipeline( m_pRenderer11->pImmPipeline, m_pRenderer11->m_pParamMgr );

	m_pRenderer11->pImmPipeline->StartPipelineStatistics();
	{
		m_pRenderer11->pImmPipeline->Draw( *m_pTerrainEffect, m_pTerrainGeometry, m_pRenderer11->m_pParamMgr );
	}
	m_pRenderer11->pImmPipeline->EndPipelineStatistics();

	// Draw the UI text
	if( !m_bSaveScreenshot )
	{
		// Don't draw text if we're taking a screenshot - don't want the images
		// cluttered with UI text!
		std::wstringstream out;
		out << L"Hieroglyph 3 : Interlocking Terrain Tiles\nFPS: " << m_pTimer->Framerate();

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

		// Show LOD or Shading?
		out << L"\n D : Toggle Debug Colour Display";

		// Advanced LoD
		out << L"\n L : Toggle LoD Complexity";

		// Automatic Rotation
		out << L"\n A : Toggle Automated Camera";

		m_pSpriteRenderer->RenderText( m_pRenderer11->pImmPipeline, m_pRenderer11->m_pParamMgr, m_pFont, out.str().c_str(), Matrix4f::Identity(), Vector4f( 1.f, 0.f, 0.f, 1.f ) );
	}

	// Present the final image to the screen
	m_pRenderer11->Present( m_pWindow->GetHandle(), m_pWindow->GetSwapChain() );

	Log::Get().Write( m_pRenderer11->pImmPipeline->PrintPipelineStatistics() );

}
//--------------------------------------------------------------------------------
void App::Shutdown()
{
	// Safely dispose of our rendering resource
	m_pTerrainGeometry = NULL;
	SAFE_DELETE( m_pTerrainEffect );
	SAFE_DELETE( m_pComputeShaderEffect );

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
			m_pTerrainEffect->m_iRasterizerState = m_bSolidRender ? m_rsSolid : m_rsWireframe;
		}
		else if ( 'D' == key )
		{
			// Debug colours - show LOD or N-dot-L shading
			switch( m_smCurrentShading )
			{
				case SolidColour:	m_smCurrentShading = SimpleShading; break;
				case SimpleShading: m_smCurrentShading = LodDebugView; break;
				case LodDebugView:	m_smCurrentShading = SolidColour; break;
			}

			m_pTerrainEffect->SetDomainShader( m_TerrainDomainShaders[ m_smCurrentShading ] );
		}
		else if ( 'L' == key )
		{
			// Toggle between simple and CS-based LOD
			m_bSimpleComplexity = !m_bSimpleComplexity;

			m_pTerrainEffect->SetHullShader( m_bSimpleComplexity ? m_iSimpleHullShader : m_iComplexHullShader );
		}
		else if ( 'A' == key )
		{
			// Toggle automated camera
			m_bViewPointInAutoMode = !m_bViewPointInAutoMode;
		}
	}

	// Call the parent class's event handler if we haven't handled the event.

	return( Application::HandleEvent( pEvent ) );
}
//--------------------------------------------------------------------------------
std::wstring App::GetName( )
{
	return( std::wstring( L"Direct3D 11 Interlocking Terrain Tiles Demo" ) );
}
//--------------------------------------------------------------------------------
void App::CreateTerrainGeometry()
{
	Log::Get().Write( L"Creating terrain geometry" );

	// Setup actual resource
	m_pTerrainGeometry = NULL;
	m_pTerrainGeometry = GeometryPtr( new GeometryDX11( ) );

	// Create vertex data
	VertexElementDX11 *pPositions = new VertexElementDX11( 3, (TERRAIN_X_LEN + 1) * (TERRAIN_Z_LEN + 1) );
		pPositions->m_SemanticName = "CONTROL_POINT_POSITION";
		pPositions->m_uiSemanticIndex = 0;
		pPositions->m_Format = DXGI_FORMAT_R32G32B32_FLOAT;
		pPositions->m_uiInputSlot = 0;
		pPositions->m_uiAlignedByteOffset = 0;
		pPositions->m_InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		pPositions->m_uiInstanceDataStepRate = 0;

	VertexElementDX11 *pTexCoords = new VertexElementDX11( 2, (TERRAIN_X_LEN + 1) * (TERRAIN_Z_LEN + 1) );
		pTexCoords->m_SemanticName = "CONTROL_POINT_TEXCOORD";
		pTexCoords->m_uiSemanticIndex = 0;
		pTexCoords->m_Format = DXGI_FORMAT_R32G32_FLOAT;
		pTexCoords->m_uiInputSlot = 0;
		pTexCoords->m_uiAlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
		pTexCoords->m_InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		pTexCoords->m_uiInstanceDataStepRate = 0;

	Vector3f *pPosData = pPositions->Get3f( 0 );
	Vector2f *pTCData = pTexCoords->Get2f( 0 );

	float fWidth = static_cast< float >( TERRAIN_X_LEN );
	float fHeight = static_cast< float >( TERRAIN_Z_LEN );

	for( int x = 0; x < TERRAIN_X_LEN + 1; ++x )
	{
		for( int z = 0; z < TERRAIN_Z_LEN + 1; ++z )
		{
			float fX = static_cast<float>(x) / fWidth - 0.5f;
			float fZ = static_cast<float>(z) / fHeight - 0.5f;
			pPosData[ x + z * (TERRAIN_X_LEN + 1) ] = Vector3f( fX, 0.0f, fZ );
			pTCData[ x + z * (TERRAIN_X_LEN + 1) ] = Vector2f( fX + 0.5f, fZ + 0.5f );
		}
	}

	m_pTerrainGeometry->AddElement( pPositions );
	m_pTerrainGeometry->AddElement( pTexCoords );

	// Create index data
	for( int x = 0; x < TERRAIN_X_LEN; ++x )
	{
		for( int z = 0; z < TERRAIN_Z_LEN; ++z )
		{
			// Define 12 control points per terrain quad

			// 0-3 are the actual quad vertices
			m_pTerrainGeometry->AddIndex( (z + 0) + (x + 0) * (TERRAIN_X_LEN + 1) );
			m_pTerrainGeometry->AddIndex( (z + 1) + (x + 0) * (TERRAIN_X_LEN + 1) );
			m_pTerrainGeometry->AddIndex( (z + 0) + (x + 1) * (TERRAIN_X_LEN + 1) );
			m_pTerrainGeometry->AddIndex( (z + 1) + (x + 1) * (TERRAIN_X_LEN + 1) );

			// 4-5 are +x
			m_pTerrainGeometry->AddIndex( clamp(z + 0, 0, TERRAIN_Z_LEN) + clamp(x + 2, 0, TERRAIN_X_LEN) * (TERRAIN_X_LEN + 1) );
			m_pTerrainGeometry->AddIndex( clamp(z + 1, 0, TERRAIN_Z_LEN) + clamp(x + 2, 0, TERRAIN_X_LEN) * (TERRAIN_X_LEN + 1) );

			// 6-7 are +z
			m_pTerrainGeometry->AddIndex( clamp(z + 2, 0, TERRAIN_Z_LEN) + clamp(x + 0, 0, TERRAIN_X_LEN) * (TERRAIN_X_LEN + 1) );
			m_pTerrainGeometry->AddIndex( clamp(z + 2, 0, TERRAIN_Z_LEN) + clamp(x + 1, 0, TERRAIN_X_LEN) * (TERRAIN_X_LEN + 1) );

			// 8-9 are -x
			m_pTerrainGeometry->AddIndex( clamp(z + 0, 0, TERRAIN_Z_LEN) + clamp(x - 1, 0, TERRAIN_X_LEN) * (TERRAIN_X_LEN + 1) );
			m_pTerrainGeometry->AddIndex( clamp(z + 1, 0, TERRAIN_Z_LEN) + clamp(x - 1, 0, TERRAIN_X_LEN) * (TERRAIN_X_LEN + 1) );

			// 10-11 are -z			
			m_pTerrainGeometry->AddIndex( clamp(z - 1, 0, TERRAIN_Z_LEN) + clamp(x + 0, 0, TERRAIN_X_LEN) * (TERRAIN_X_LEN + 1) );
			m_pTerrainGeometry->AddIndex( clamp(z - 1, 0, TERRAIN_Z_LEN) + clamp(x + 1, 0, TERRAIN_X_LEN) * (TERRAIN_X_LEN + 1) );
		}
	}

	// Move the in-memory geometry to be 
	// an actual renderable resource
	m_pTerrainGeometry->LoadToBuffers();
	m_pTerrainGeometry->SetPrimitiveType( D3D11_PRIMITIVE_TOPOLOGY_12_CONTROL_POINT_PATCHLIST );

	Log::Get().Write( L"Created terrain geometry" );
}
//--------------------------------------------------------------------------------
void App::CreateTerrainShaders()
{
	Log::Get().Write( L"Creating shaders" );

	SAFE_DELETE( m_pTerrainEffect );
	m_pTerrainEffect = new RenderEffectDX11();

	// Create the vertex shader
	m_pTerrainEffect->SetVertexShader( m_pRenderer11->LoadShader( VERTEX_SHADER,
		std::wstring( L"InterlockingTerrainTiles.hlsl" ),
		std::wstring( L"vsMain" ),
		std::wstring( L"vs_5_0" ) ) );
	_ASSERT( -1 != m_pTerrainEffect->GetVertexShader() );
	
	Log::Get().Write( L"... vertex shader created" );

	// Create the hull shader
	m_iSimpleHullShader = m_pRenderer11->LoadShader( HULL_SHADER,
							std::wstring( L"InterlockingTerrainTiles.hlsl" ),
							std::wstring( L"hsSimple" ),
							std::wstring( L"hs_5_0" ) );
	_ASSERT( -1 != m_iSimpleHullShader );

	m_iComplexHullShader = m_pRenderer11->LoadShader( HULL_SHADER,
							std::wstring( L"InterlockingTerrainTiles.hlsl" ),
							std::wstring( L"hsComplex" ),
							std::wstring( L"hs_5_0" ) );
	_ASSERT( -1 != m_iComplexHullShader );

	m_pTerrainEffect->SetHullShader( m_bSimpleComplexity ? m_iSimpleHullShader : m_iComplexHullShader );

	Log::Get().Write( L"... hull shader created" );

	// Create the domain shaders

	D3D_SHADER_MACRO dsSolid[2] = { "SHADING_SOLID", "1", NULL, NULL };
	m_TerrainDomainShaders[SolidColour] = m_pRenderer11->LoadShader( DOMAIN_SHADER, std::wstring( L"InterlockingTerrainTiles.hlsl" ), std::wstring( L"dsMain" ), std::wstring( L"ds_5_0" ), dsSolid );
	_ASSERT( -1 != m_TerrainDomainShaders[SolidColour] );

	D3D_SHADER_MACRO dsShaded[2] = { "SHADING_SIMPLE", "1", NULL, NULL };
	m_TerrainDomainShaders[SimpleShading] = m_pRenderer11->LoadShader( DOMAIN_SHADER, std::wstring( L"InterlockingTerrainTiles.hlsl" ), std::wstring( L"dsMain" ), std::wstring( L"ds_5_0" ), dsShaded );
	_ASSERT( -1 != m_TerrainDomainShaders[SimpleShading] );

	D3D_SHADER_MACRO dsDebug[2] = { "SHADING_DEBUG_LOD", "1", NULL, NULL };
	m_TerrainDomainShaders[LodDebugView] = m_pRenderer11->LoadShader( DOMAIN_SHADER, std::wstring( L"InterlockingTerrainTiles.hlsl" ), std::wstring( L"dsMain" ), std::wstring( L"ds_5_0" ), dsDebug );
	_ASSERT( -1 != m_TerrainDomainShaders[LodDebugView] );

	m_pTerrainEffect->SetDomainShader( m_TerrainDomainShaders[ m_smCurrentShading ] );

	Log::Get().Write( L"... domain shaders created" );

	// Create the geometry shader
	m_pTerrainEffect->SetGeometryShader( m_pRenderer11->LoadShader( GEOMETRY_SHADER,
		std::wstring( L"InterlockingTerrainTiles.hlsl" ),
		std::wstring( L"gsMain" ),
		std::wstring( L"gs_5_0" ) ) );
	_ASSERT( -1 != m_pTerrainEffect->GetGeometryShader() );

	Log::Get().Write( L"... geometry shader created" );

	// Create the pixel shader
	m_pTerrainEffect->SetPixelShader( m_pRenderer11->LoadShader( PIXEL_SHADER,
		std::wstring( L"InterlockingTerrainTiles.hlsl" ),
		std::wstring( L"psMain" ),
		std::wstring( L"ps_5_0" ) ) );
	_ASSERT( -1 != m_pTerrainEffect->GetPixelShader() );

	Log::Get().Write( L"... pixel shader created" );

	// Create rasterizer states
	RasterizerStateConfigDX11 RS;
	
	RS.FillMode = D3D11_FILL_WIREFRAME;
	RS.CullMode = D3D11_CULL_NONE;
	m_rsWireframe = m_pRenderer11->CreateRasterizerState( &RS );

	RS.FillMode = D3D11_FILL_SOLID;
	RS.CullMode = D3D11_CULL_FRONT;
	m_rsSolid = m_pRenderer11->CreateRasterizerState( &RS );

	// Assign default state
	m_pTerrainEffect->m_iRasterizerState = m_bSolidRender ? m_rsSolid : m_rsWireframe;

	Log::Get().Write( L"Created all shaders" );
}
//--------------------------------------------------------------------------------
void App::CreateTerrainTextures()
{
	Log::Get().Write( L"Creating textures" );

	// Load the texture
	m_pHeightMapTexture = m_pRenderer11->LoadTexture( std::wstring( L"TerrainHeightMap.png" ) );

	// Store the height/width to the param manager
	D3D11_TEXTURE2D_DESC d = m_pHeightMapTexture->m_pTexture2dConfig->GetTextureDesc();
	Vector4f vTexDim = Vector4f( static_cast<float>(d.Width), static_cast<float>(d.Height), static_cast<float>(TERRAIN_X_LEN), static_cast<float>(TERRAIN_Z_LEN) );
	m_pRenderer11->m_pParamMgr->SetVectorParameter( m_pheightMapDimensions, &vTexDim );

	//// Create the SRV
	//ShaderResourceParameterDX11* pHeightMapTexParam = new ShaderResourceParameterDX11();
	//pHeightMapTexParam->SetParameterData( &m_pHeightMapTexture->m_iResourceSRV );
	//pHeightMapTexParam->SetName( std::wstring( L"texHeightMap" ) );

	// Map it to the param manager
	m_pRenderer11->m_pParamMgr->SetShaderResourceParameter( m_ptexHeightMap, m_pHeightMapTexture );

	// Create a sampler
	D3D11_SAMPLER_DESC sampDesc;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampDesc.BorderColor[0] = sampDesc.BorderColor[1] = sampDesc.BorderColor[2] = sampDesc.BorderColor[3] = 0;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampDesc.MaxAnisotropy = 16;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
	sampDesc.MinLOD = 0.0f;
	sampDesc.MipLODBias = 0.0f;
	int samplerState = m_pRenderer11->CreateSamplerState( &sampDesc );

	// Set it to the param manager
	m_pRenderer11->m_pParamMgr->SetSamplerParameter( m_psmpHeightMap, &samplerState );

	Log::Get().Write( L"Created textures" );
}
//--------------------------------------------------------------------------------
void App::UpdateViewState()
{
	if( m_bViewPointInAutoMode )
	{
		// If in 'auto' mode then simply keep rotating
		float time = m_pTimer->Runtime();

		// Create the world matrix
		Matrix4f mWorld, mWorldScale, mWorldRotation;
		
		mWorld.MakeIdentity();
		
		// Geometry is defined in the [-0.5,+0.5] range on the XZ plane
		mWorldScale = Matrix4f::ScaleMatrixXYZ( 15.0f, 1.0f, 15.0f ); // Domain Shader controls Y scale, not here!
		mWorld = mWorldScale * mWorld;

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
		float toAngle = fmodf( (distance + 0.08f) * 2.0f * static_cast<float>(GLYPH_PI), 2.0f * static_cast<float>(GLYPH_PI)); // ~30 degrees in front

		vLookFrom.x = sinf(fromAngle) * 10.0f;
		vLookFrom.y = 4.f;
		vLookFrom.z = cosf(fromAngle) * 10.0f;

		vLookAt.x = sinf(toAngle) * 3.0f;
		vLookAt.y = 0.3f;
		vLookAt.z = cosf(toAngle) * 3.0f;

		// Create the view matrix
		Matrix4f mView = Matrix4f::LookAtLHMatrix( vLookFrom, vLookAt, vLookUp );

		// Create the projection matrix
		Matrix4f mProj = Matrix4f::PerspectiveFovLHMatrix( static_cast< float >(GLYPH_PI) / 3.0f, 
			static_cast<float>(m_pWindow->GetWidth()) / static_cast<float>(m_pWindow->GetHeight()), 1.0f, 25.0f );

		// Composite together for the final transform
		Matrix4f mViewProj = mView * mProj;

		// Set the various values to the parameter manager
		m_pRenderer11->m_pParamMgr->SetMatrixParameter( m_pmWorld, &mWorld );
		m_pRenderer11->m_pParamMgr->SetMatrixParameter( m_pmViewProj, &mViewProj );
		m_pRenderer11->m_pParamMgr->SetMatrixParameter( m_pmInvTposeWorld, &mInvTPoseWorld );

		Vector4f vCam = Vector4f( vLookFrom.x, vLookFrom.y, vLookFrom.z, /* unused */ 0.0f );
		m_pRenderer11->m_pParamMgr->SetVectorParameter( m_pcameraPosition, &vCam );
	}
	else
	{
		// Else, if in 'manual' mode then update according to the
		// current user's input
	}
}
//--------------------------------------------------------------------------------
void App::CreateComputeShaderResources()
{
	Log::Get().Write( L"Creating compute shader resources" );

	// Assert on the input texture dimensions
	D3D11_TEXTURE2D_DESC d = m_pHeightMapTexture->m_pTexture2dConfig->GetTextureDesc();
	_ASSERT( 0 == (d.Width % 16) );
	_ASSERT( 0 == (d.Height % 16) );

	// Create the output texture
	Texture2dConfigDX11 LookupTextureConfig;
	LookupTextureConfig.SetFormat( DXGI_FORMAT_R32G32B32A32_FLOAT );
	LookupTextureConfig.SetColorBuffer( TERRAIN_X_LEN, TERRAIN_Z_LEN ); 
	LookupTextureConfig.SetBindFlags( D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE );

	m_pLodLookupTexture = m_pRenderer11->CreateTexture2D( &LookupTextureConfig, 0 );

	// Create the effect
	SAFE_DELETE( m_pComputeShaderEffect );
	m_pComputeShaderEffect = new RenderEffectDX11( );

	// Compile the compute shader
	m_pComputeShaderEffect->SetComputeShader( m_pRenderer11->LoadShader( COMPUTE_SHADER,
		std::wstring( L"InterlockingTerrainTilesComputeShader.hlsl" ),
		std::wstring( L"csMain" ),
		std::wstring( L"cs_5_0" ) ) );
	_ASSERT( -1 != m_pComputeShaderEffect->GetComputeShader() );

	Log::Get().Write( L"Compute Shader Resources Created" );
}
//--------------------------------------------------------------------------------
void App::RunComputeShader()
{
	Log::Get().Write( L"Running Compute Shader Pre-Pass" );

	// Bind the resources
	m_pRenderer11->m_pParamMgr->SetUnorderedAccessParameter( m_pbufferResults, m_pLodLookupTexture );
	m_pRenderer11->m_pParamMgr->SetShaderResourceParameter( m_ptexHeightMap, m_pHeightMapTexture );

	// Determine number of threads
	D3D11_TEXTURE2D_DESC d = m_pHeightMapTexture->m_pTexture2dConfig->GetTextureDesc();

	// Run the compute shader
	m_pRenderer11->pImmPipeline->Dispatch( *m_pComputeShaderEffect, d.Width / 16, d.Height / 16, 1, m_pRenderer11->m_pParamMgr );

	// Bind the output to the hull shader
	m_pRenderer11->m_pParamMgr->SetShaderResourceParameter( m_ptexLODLookup, m_pLodLookupTexture );

	Log::Get().Write( L"Compute Shader Pre-Pass Complete" );
}
//--------------------------------------------------------------------------------