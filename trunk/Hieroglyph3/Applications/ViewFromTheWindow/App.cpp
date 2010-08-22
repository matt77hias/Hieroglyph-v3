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

#include "ParameterManagerDX11.h"

using namespace Glyph3;
//--------------------------------------------------------------------------------
App AppInstance; // Provides an instance of the application
//--------------------------------------------------------------------------------


//--------------------------------------------------------------------------------
App::App()
{
	m_bSaveScreenshot = false;
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
		m_pWindow[i]->SetCaption( std::wstring( L"Direct3D 11 Window" ) );
		m_pWindow[i]->Initialize();

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
	m_pRenderer11->m_pPipeMgr->SetViewPort( ViewPort );
	
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
	// Basic event handling is supported with the EventManager class.  This is a 
	// singleton class that allows an EventListener to register which events it
	// wants to receive.

	EventManager* pEventManager = EventManager::Get( );

	// The application object wants to know about these three events, so it 
	// registers itself with the appropriate event IDs.

	pEventManager->AddEventListener( SYSTEM_KEYBOARD_KEYUP, this );
	pEventManager->AddEventListener( SYSTEM_KEYBOARD_KEYDOWN, this );
	pEventManager->AddEventListener( SYSTEM_KEYBOARD_CHAR, this );

	// Load and initialize the geometry to be rendered.

	m_pGeometry = GeometryLoaderDX11::loadMS3DFile2( std::wstring( L"../Data/Models/Sample_Scene.ms3d" ) );
	m_pGeometry->LoadToBuffers();
	m_pGeometry->SetPrimitiveType( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );


	// Create the parameters for use with this effect

	m_LightParams = Vector4f( 1.0f, 1.0f, 1.0f, 1.0f );
	m_pRenderer11->m_pParamMgr->SetVectorParameter( std::wstring( L"LightColor" ), &m_LightParams );

	m_LightPosition = Vector4f( 20.0f, 20.0f, -20.0f, 0.0f );
	m_pRenderer11->m_pParamMgr->SetVectorParameter( std::wstring( L"LightPositionWS" ), &m_LightPosition );

	// Create the material for use by the entities.

	m_pMaterial = MaterialGeneratorDX11::GeneratePhong( *m_pRenderer11 );


	// Create the camera, and the render view that will produce an image of the 
	// from the camera's point of view of the scene.

	m_pCamera = new Camera();
	m_pCamera->GetNode()->Rotation().RotationX( 0.307f );
	m_pCamera->GetNode()->Position() = Vector3f( 0.0f, 2.5f, -5.0f );
	m_pRenderView = new ViewPerspective( *m_pRenderer11, m_OffscreenTexture, m_DepthTarget );
	m_pRenderView->SetBackColor( Vector4f( 0.6f, 0.6f, 0.6f, 0.6f ) );
	m_pCamera->SetCameraView( m_pRenderView );
	m_pCamera->SetProjectionParams( 0.1f, 100.0f, static_cast<float>( D3DX_PI ) / 2.0f, m_DesktopRes.x / m_DesktopRes.y );

	// Create the scene and add the entities to it.  Then add the camera to the
	// scene so that it will be updated via the scene interface instead of 
	// manually manipulating it.

	m_pNode = new Node3D();
	m_pEntity = new Entity3D();
	m_pEntity->SetGeometry( m_pGeometry );
	m_pEntity->SetMaterial( m_pMaterial, false );
	
	m_pNode->AttachChild( m_pEntity );

	m_pScene->AddEntity( m_pNode );
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

	EventManager::Get()->ProcessEvent( new EvtFrameStart() );


	// Manipulate the scene here - simply rotate the root of the scene in this
	// example.

	Matrix3f rotation;
	rotation.RotationY( m_pTimer->Elapsed() );
	m_pNode->Rotation() *= rotation;


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

		m_pRenderer11->m_pPipeMgr->CopySubresourceRegion( m_RenderTarget[i], 0, 0, 0, 0, m_OffscreenTexture, 0, &box );
		m_pRenderer11->Present( m_pWindow[i]->GetHandle(), m_pWindow[i]->GetSwapChain() );
	}


	// Save a screenshot if desired.  This is done by pressing the 's' key, which
	// demonstrates how an event is sent and handled by an event listener (which
	// in this case is the application object itself).

	if ( m_bSaveScreenshot  )
	{
		m_bSaveScreenshot = false;
		m_pRenderer11->m_pPipeMgr->SaveTextureScreenShot( 0, std::wstring( L"BasicApplication_" ), D3DX11_IFF_BMP );
	}
}
//--------------------------------------------------------------------------------
void App::Shutdown()
{
	SAFE_DELETE( m_pEntity );
	
	SAFE_DELETE( m_pNode );

	SAFE_DELETE( m_pCamera );

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