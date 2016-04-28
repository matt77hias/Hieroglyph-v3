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
#include "BufferConfigDX11.h"

#include "GeometryLoaderDX11.h"
#include "GeometryGeneratorDX11.h"
#include "RenderEffectDX11.h"
#include "RasterizerStateConfigDX11.h"

#include "IParameterManager.h"

using namespace Glyph3;
//--------------------------------------------------------------------------------
App AppInstance; // Provides an instance of the application
//--------------------------------------------------------------------------------


//--------------------------------------------------------------------------------
App::App()
{
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
		m_pTimer->SetFixedTimeStep( 1.0f / 20.0f );
	}

	// Create a swap chain for the window that we started out with.  This
	// demonstrates using a configuration object for fast and concise object
	// creation.

	DXGI_SAMPLE_DESC sd;
	sd.Quality = 0;
	sd.Count = 1;

	SwapChainConfigDX11 Config;
	Config.SetWidth( m_pWindow->GetWidth() );
	Config.SetHeight( m_pWindow->GetHeight() );
	Config.SetOutputWindow( m_pWindow->GetHandle() );
	Config.SetSampleDesc( sd );
	
	m_iSwapChain = m_pRenderer11->CreateSwapChain( &Config );
	m_pWindow->SetSwapChain( m_iSwapChain );

	// We'll keep a copy of the render target index to use in later examples.

	m_RenderTarget = m_pRenderer11->GetSwapChainResource( m_iSwapChain );

	// Next we create a depth buffer for use in the traditional rendering
	// pipeline.

	Texture2dConfigDX11 DepthConfig;
	DepthConfig.SetDepthBuffer( width, height );
	DepthConfig.SetSampleDesc( sd );
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
	// Load the shaders and state for the tessellation effects

	m_pTessellationEffect = new RenderEffectDX11();
	m_pTessellationEffect->SetVertexShader( m_pRenderer11->LoadShader( VERTEX_SHADER,
		std::wstring( L"BasicTessellation.hlsl" ),
		std::wstring( L"VSMAIN" ),
		std::wstring( L"vs_5_0" ) ) );

	m_pTessellationEffect->SetHullShader( m_pRenderer11->LoadShader( HULL_SHADER,
		std::wstring( L"BasicTessellation.hlsl" ),
		std::wstring( L"HSMAIN" ),
		std::wstring( L"hs_5_0" ) ) );

	m_pTessellationEffect->SetDomainShader( m_pRenderer11->LoadShader( DOMAIN_SHADER,
		std::wstring( L"BasicTessellation.hlsl" ),
		std::wstring( L"DSMAIN" ),
		std::wstring( L"ds_5_0" ) ) );

	m_pTessellationEffect->SetPixelShader( m_pRenderer11->LoadShader( PIXEL_SHADER,
		std::wstring( L"BasicTessellation.hlsl" ),
		std::wstring( L"PSMAIN" ),
		std::wstring( L"ps_5_0" ) ) );

	RasterizerStateConfigDX11 RS;
	RS.FillMode = D3D11_FILL_WIREFRAME;
	m_pTessellationEffect->m_iRasterizerState = 
		m_pRenderer11->CreateRasterizerState( &RS );

	// Set up the geometry to be rendered
	//m_pGeometry = GeometryLoaderDX11::loadMS3DFile2( std::wstring( L"box.ms3d" ) );
	m_pGeometry = GeometryLoaderDX11::loadMS3DFile2( std::wstring( L"hedra.ms3d" ) );
	m_pGeometry->LoadToBuffers();
	m_pGeometry->SetPrimitiveType( D3D11_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST );
	//m_pGeometry->SetPrimitiveType( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );

	// Build the world, view, and projection matrices

	// Create the world matrix
	m_WorldMatrix.MakeIdentity();

	// Create the view matrix
	Vector3f vLookAt = Vector3f( 0.0f, 0.75f, 0.0f );
	Vector3f vLookFrom = Vector3f( 5.0f, 5.5f, -5.0f );
	Vector3f vLookUp = Vector3f( 0.0f, 1.0f, 0.0f );
	m_ViewMatrix = Matrix4f::LookAtLHMatrix( vLookFrom, vLookAt, vLookUp );

	// Create the projection matrix
	m_ProjMatrix = Matrix4f::PerspectiveFovLHMatrix( static_cast< float >(GLYPH_PI) / 2.0f, 640.0f / 480.0f, 0.1f, 25.0f );

	// Concatenate the view and projection matrices
	m_ViewProjMatrix = m_ViewMatrix * m_ProjMatrix;


	m_pRenderer11->m_pParamMgr->SetMatrixParameter( std::wstring( L"WorldMatrix" ), &m_WorldMatrix );
	m_pRenderer11->m_pParamMgr->SetMatrixParameter( std::wstring( L"ViewProjMatrix" ), &m_ViewProjMatrix );

	m_TessParams = Vector4f( 1.0f, 1.0f, 1.0f, 1.0f );
	m_pRenderer11->m_pParamMgr->SetVectorParameter( std::wstring( L"EdgeFactors" ), &m_TessParams );
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

	// Use a time varying quantity for animation.

	static float fRotation = 0.0f;
	fRotation += m_pTimer->Elapsed() * 3.14f * 0.05f;

	static float fTessellation = 3.0f * 3.14f / 2.0f;
	fTessellation += m_pTimer->Elapsed() * 0.2f * 3.14f;

	float factor = sinf( fTessellation ) * 6.0f + 7.0f;
	m_TessParams = Vector4f( factor, factor, factor, factor );
	m_pRenderer11->m_pParamMgr->SetVectorParameter( std::wstring( L"EdgeFactors" ), &m_TessParams );

	m_WorldMatrix.RotationY( fRotation );
	m_pRenderer11->m_pParamMgr->SetMatrixParameter( std::wstring( L"WorldMatrix" ), &m_WorldMatrix );

	m_pRenderer11->pImmPipeline->ClearBuffers( Vector4f( 0.6f, 0.6f, 0.6f, 0.6f ), 1.0f );

	m_pRenderer11->pImmPipeline->Draw( *m_pTessellationEffect, m_pGeometry, m_pRenderer11->m_pParamMgr ); 


	m_pRenderer11->Present( m_pWindow->GetHandle(), m_pWindow->GetSwapChain() );

}
//--------------------------------------------------------------------------------
void App::Shutdown()
{
	SAFE_DELETE( m_pTessellationEffect );
	m_pGeometry = NULL;

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
	}

	// Call the parent class's event handler if we haven't handled the event.

	return( Application::HandleEvent( pEvent ) );
}
//--------------------------------------------------------------------------------
std::wstring App::GetName( )
{
	return( std::wstring( L"BasicTessellation" ) );
}
//--------------------------------------------------------------------------------