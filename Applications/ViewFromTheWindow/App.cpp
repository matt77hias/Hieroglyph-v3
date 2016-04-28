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

#include "GeometryLoaderDX11.h"
#include "MaterialGeneratorDX11.h"

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
	// Create the renderer and initialize it for the desired device
	// type and feature level.

	m_pRenderer11 = new RendererDX11();

	if ( !m_pRenderer11->Initialize( D3D_DRIVER_TYPE_HARDWARE, D3D_FEATURE_LEVEL_11_0 ) )
	{
		Log::Get().Write( L"Could not create hardware device, trying to create the reference device..." );

		if ( !m_pRenderer11->Initialize( D3D_DRIVER_TYPE_REFERENCE, D3D_FEATURE_LEVEL_11_0 ) )
		{
			MessageBox( m_pWindow[0]->GetHandle(), L"Could not create a hardware or software Direct3D 11 device - the program will now abort!", L"Hieroglyph 3 Rendering", MB_ICONEXCLAMATION | MB_SYSTEMMODAL );
			RequestTermination();			
			return( false );
		}

		// If using the reference device, utilize a fixed time step for any animations.
		m_pTimer->SetFixedTimeStep( 1.0f / 10.0f );
	}

	// Get the current desktop resolution and make a render target 
	// and depth buffer that can fill it.

	m_DesktopRes = m_pRenderer11->GetDesktopResolution();

	Texture2dConfigDX11 TexConfig;
	TexConfig.SetColorBuffer( static_cast<unsigned int>( m_DesktopRes.x ), static_cast<unsigned int>( m_DesktopRes.y ) );
	TexConfig.SetBindFlags( D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE );
	TexConfig.SetFormat( DXGI_FORMAT_R8G8B8A8_UNORM );
	m_OffscreenTexture = m_pRenderer11->CreateTexture2D( &TexConfig, 0 );


	// Next we create a depth buffer for use in the traditional rendering
	// pipeline.

	Texture2dConfigDX11 DepthConfig;
	DepthConfig.SetDepthBuffer( static_cast<unsigned int>( m_DesktopRes.x ), static_cast<unsigned int>( m_DesktopRes.y ) );
	m_DepthTarget = m_pRenderer11->CreateTexture2D( &DepthConfig, 0 );


	// Initialize the random number generator.
	srand( GetTickCount() );


	// Create all of the windows.

	for ( int i = 0; i < NUM_WINDOWS; i++ )
	{
		// Create the window wrapper class instance.
		m_pWindow[i] = new Win32RenderWindow();

		// Generate random locations and sizes of the window, constrained 
		// to the desktop.
		int x = static_cast<int>( (double)rand() / RAND_MAX * 800 + 200 );
		int y = static_cast<int>( (double)rand() / RAND_MAX * 400 + 0 );
		int w = static_cast<int>( (double)rand() / RAND_MAX * ( m_DesktopRes.x - x ) * 0.5f );	// Limit the right hand side to within the screen
		int h = static_cast<int>( (double)rand() / RAND_MAX * ( m_DesktopRes.y - y ) * 0.5f );	// Limit the bottom side to within the screen

		// Configure the window and initialize it.
		m_pWindow[i]->SetPosition( x, y );
		m_pWindow[i]->SetSize( w, h );
		m_pWindow[i]->SetCaption( GetName() );
		m_pWindow[i]->Initialize( this );

		// Create a swap chain for the window.
		SwapChainConfigDX11 Config;
		Config.SetWidth( m_pWindow[i]->GetWidth() );
		Config.SetHeight( m_pWindow[i]->GetHeight() );
		Config.SetOutputWindow( m_pWindow[i]->GetHandle() );
		int	iSwapChain = m_pRenderer11->CreateSwapChain( &Config );
		m_pWindow[i]->SetSwapChain( iSwapChain );

		// We'll keep a copy of the swap chain's render target index to 
		// use later.
		m_RenderTarget[i] = m_pRenderer11->GetSwapChainResource( iSwapChain );
	}


	// Create a view port to use on the scene.  This basically selects the 
	// entire floating point area of the render target.

	D3D11_VIEWPORT viewport;
	viewport.Width = static_cast< float >( m_DesktopRes.x );
	viewport.Height = static_cast< float >( m_DesktopRes.y );
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

	for ( int i = 0; i < NUM_WINDOWS; i++ )
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
	// Load and initialize the geometry to be rendered.

	m_pGeometry = GeometryLoaderDX11::loadMS3DFile2( std::wstring( L"Sample_Scene.ms3d" ) );
	m_pGeometry->LoadToBuffers();
	m_pGeometry->SetPrimitiveType( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );


	// Create the parameters for use with this effect

	m_LightParams = Vector4f( 1.0f, 1.0f, 1.0f, 1.0f );
	m_pLightColor = m_pRenderer11->m_pParamMgr->GetVectorParameterRef( std::wstring( L"LightColor" ) );
	m_pLightColor->InitializeParameterData( &m_LightParams );

	m_LightPosition = Vector4f( 20.0f, 20.0f, -20.0f, 0.0f );
	m_pLightPosition = m_pRenderer11->m_pParamMgr->GetVectorParameterRef( std::wstring( L"LightPositionWS" ) );
	m_pLightPosition->InitializeParameterData( &m_LightPosition );
	

	// Create the material for use by the entities.

	m_pMaterial = MaterialGeneratorDX11::GeneratePhong( *m_pRenderer11 );


	// Create the camera, and the render view that will produce an image of the 
	// from the camera's point of view of the scene.

	m_pCamera = new Camera();
	m_pCamera->Spatial().SetRotation( Vector3f( 0.307f, 0.0f, 0.0f ) );
	m_pCamera->Spatial().SetTranslation( Vector3f( 0.0f, 2.5f, -5.0f ) );
	m_pRenderView = new ViewPerspective( *m_pRenderer11, m_OffscreenTexture, m_DepthTarget );
	m_pRenderView->SetBackColor( Vector4f( 0.6f, 0.6f, 0.6f, 0.6f ) );
	m_pCamera->SetCameraView( m_pRenderView );
	m_pCamera->SetProjectionParams( 0.1f, 100.0f, m_DesktopRes.x / m_DesktopRes.y, static_cast<float>( GLYPH_PI ) / 2.0f );

	// Create the scene and add the entities to it.  Then add the camera to the
	// scene so that it will be updated via the scene interface instead of 
	// manually manipulating it.

	m_pActor = new Actor();
	m_pActor->GetBody()->Visual.SetGeometry( m_pGeometry );
	m_pActor->GetBody()->Visual.SetMaterial( m_pMaterial );
	m_pActor->GetNode()->Transform.Position() = Vector3f( 0.0f, 0.0f, 0.0f );

	m_pScene->AddActor( m_pActor );

	m_pScene->AddCamera( m_pCamera );

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


	// Manipulate the scene here - simply rotate the root of the scene in this
	// example.

	Matrix3f rotation;
	rotation.RotationY( m_pTimer->Elapsed() );
	m_pActor->GetNode()->Transform.Rotation() *= rotation;


	// Update the scene, and then render all cameras within the scene.

	m_pScene->Update( m_pTimer->Elapsed() );
	m_pScene->Render( m_pRenderer11 );


	// Perform the rendering and presentation for each window.

	for ( int i = 0; i < NUM_WINDOWS; i++ )
	{
		// Bind the swap chain render target and the depth buffer for use in 
		// rendering.  

		D3D11_BOX box;
		box.left = m_pWindow[i]->GetLeft();
		box.right = m_pWindow[i]->GetWidth() + box.left;
		box.top = m_pWindow[i]->GetTop();
		box.bottom = m_pWindow[i]->GetHeight() + box.top;
		box.front = 0;
		box.back = 1;

		if ( box.left < 0 ) box.left = 0;
		if ( box.right > (unsigned int)m_DesktopRes.x - 1 ) box.right = (unsigned int)m_DesktopRes.x - 1;
		if ( box.top < 0 ) box.top = 0;
		if ( box.bottom > (unsigned int)m_DesktopRes.y - 1 ) box.bottom = (unsigned int)m_DesktopRes.y - 1;

		m_pRenderer11->pImmPipeline->CopySubresourceRegion( m_RenderTarget[i], 0, 0, 0, 0, m_OffscreenTexture, 0, &box );
		m_pRenderer11->Present( m_pWindow[i]->GetHandle(), m_pWindow[i]->GetSwapChain() );
	}

}
//--------------------------------------------------------------------------------
void App::Shutdown()
{
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
	return( std::wstring( L"View From The Window" ) );
}
//--------------------------------------------------------------------------------