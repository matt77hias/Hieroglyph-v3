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
#include "GeometryGeneratorDX11.h"
#include "MaterialGeneratorDX11.h"
#include "RasterizerStateConfigDX11.h"

#include "ParameterManagerDX11.h"

#include "ReflectiveSphereEntity.h"
#include "DiffuseSphereEntity.h"

#include "RotationController.h"

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
	// Basic event handling is supported with the EventManager class.  This is a 
	// singleton class that allows an EventListener to register which events it
	// wants to receive.

	EventManager* pEventManager = EventManager::Get( );

	// The application object wants to know about these three events, so it 
	// registers itself with the appropriate event IDs.

	pEventManager->AddEventListener( SYSTEM_KEYBOARD_KEYUP, this );
	pEventManager->AddEventListener( SYSTEM_KEYBOARD_KEYDOWN, this );
	pEventManager->AddEventListener( SYSTEM_KEYBOARD_CHAR, this );


	// Create the renderer and initialize it for the desired device
	// type and feature level.

	m_pRenderer11 = new RendererDX11();

	if ( !m_pRenderer11->Initialize( D3D_DRIVER_TYPE_HARDWARE, D3D_FEATURE_LEVEL_11_0 ) )
	{
		Log::Get().Write( L"Could not create hardware device, trying to create the reference device..." );

		if ( !m_pRenderer11->Initialize( D3D_DRIVER_TYPE_REFERENCE, D3D_FEATURE_LEVEL_11_0 ) )
		{
			MessageBox( m_pWindow->GetHandle(), L"Could not create a hardware or software Direct3D 11 device - the program will now abort!", L"Hieroglyph 3 Rendering", MB_ICONEXCLAMATION | MB_SYSTEMMODAL );
			RequestTermination();			
			return( false );
		}

		// If using the reference device, utilize a fixed time step for any animations.
		m_pTimer->SetFixedTimeStep( 1.0f / 10.0f );
	}

	// Create the window.
	int width = 512;
	int height = 512;

	// Create the window wrapper class instance.
	m_pWindow = new Win32RenderWindow();
	m_pWindow->SetPosition( 20, 20 );
	m_pWindow->SetSize( width, height );
	m_pWindow->SetCaption( std::wstring( L"Direct3D 11 Window" ) );
	m_pWindow->Initialize();

	// Create a swap chain for the window.
	SwapChainConfigDX11 Config;
	Config.SetWidth( m_pWindow->GetWidth() );
	Config.SetHeight( m_pWindow->GetHeight() );
	Config.SetOutputWindow( m_pWindow->GetHandle() );
	m_pWindow->SetSwapChain( m_pRenderer11->CreateSwapChain( &Config ) );

	// We'll keep a copy of the swap chain's render target index to 
	// use later.
	m_RenderTarget = m_pRenderer11->GetSwapChainResource( m_pWindow->GetSwapChain() );

	// Next we create a depth buffer for use in the traditional rendering
	// pipeline.
	Texture2dConfigDX11 DepthConfig;
	DepthConfig.SetDepthBuffer( width, height );
	m_DepthTarget = m_pRenderer11->CreateTexture2D( &DepthConfig, 0 );

	// Create a view port to use on the scene.  This basically selects the 
	// entire floating point area of the render target.
	D3D11_VIEWPORT viewport;
	viewport.Width = static_cast< float >( width );
	viewport.Height = static_cast< float >( height );
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;

//	m_pRenderer11->pImmPipeline->SetViewPort( m_pRenderer11->CreateViewPort( viewport ) );
//	m_pRenderer11->m_pDeferredPipeline->SetViewPort( 0 );
	
	m_pFont = new SpriteFontDX11();
	m_pFont->Initialize( L"Consolas", 16.0f, 0, false );
	
	m_pSpriteRenderer = new SpriteRendererDX11();
	m_pSpriteRenderer->Initialize();

	m_pRenderer11->SetMultiThreadingState( true );

	return( true );
}
//--------------------------------------------------------------------------------
void App::ShutdownEngineComponents()
{
	SAFE_DELETE( m_pFont );
	SAFE_DELETE( m_pSpriteRenderer );

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

	// Set any parameters that will be needed by the shaders used above.
	
	m_LightParams = Vector4f( 1.0f, 1.0f, 1.0f, 1.0f );
	m_pRenderer11->m_pParamMgr->SetVectorParameter( std::wstring( L"LightColor" ), &m_LightParams );

	m_LightPosition = Vector4f( 20.0f, 20.0f, -20.0f, 0.0f );
	m_pRenderer11->m_pParamMgr->SetVectorParameter( std::wstring( L"LightPositionWS" ), &m_LightPosition );


	// Create the camera, and the render view that will produce an image of the 
	// from the camera's point of view of the scene.

	m_pCamera = new Camera();
	m_pCamera->GetNode()->Rotation().Rotation( Vector3f( static_cast<float>( D3DX_PI ) / 6.0f, static_cast<float>( D3DX_PI ) / 4.0f, 0.0f ) );
	m_pCamera->GetNode()->Position() = Vector3f( -5.0f, 5.0f, -5.0f );
	m_pRenderView = new ViewPerspective( *m_pRenderer11, m_RenderTarget, m_DepthTarget );
	m_pRenderView->SetBackColor( Vector4f( 0.2f, 0.2f, 0.2f, 0.2f ) );
	m_pCamera->SetCameraView( m_pRenderView );
	m_pCamera->SetProjectionParams( 0.1f, 1000.0f, 512.0f / 512.0f, static_cast<float>( D3DX_PI ) / 4.0f );

	// Create the scene and add the entities to it.  Then add the camera to the
	// scene so that it will be updated via the scene interface instead of 
	// manually manipulating it.


	static float radius = 10.0f;

	m_pNode = new Node3D();
	
	m_pDiffuseActor = new Actor();
	m_pDiffuseActor->GetNode()->AttachController( new RotationController( Vector3f( 1.0f, 0.0f, 0.0f ), 0.5f ) );
	for ( int i = 0; i < 100; i++ )
	{
		float x = static_cast<float>( (double)rand() / RAND_MAX ) * 2.0f - 1.0f;
		float y = static_cast<float>( (double)rand() / RAND_MAX ) * 2.0f - 1.0f;
		float z = static_cast<float>( (double)rand() / RAND_MAX ) * 2.0f - 1.0f;
		
		Vector3f pos = Vector3f( x, y, z );
		pos.Normalize();

		Entity3D* pEntity = new DiffuseSphereEntity();
		pEntity->Position() = pos * radius;
		m_pDiffuseActor->GetNode()->AttachChild( pEntity );
		m_pDiffuseActor->AddElement( pEntity );
	}

	m_pReflectiveActor = new Actor();
	
	
	

	for ( int i = 0; i < 16; i++ )
	{
		m_pReflector[i] = new ReflectiveSphereEntity();
		m_pReflector[i]->Position() = Vector3f( -1.0, 1.0f+(i*3), 0.0f );
		m_pReflector[i]->m_pParaboloidView->SetRoot( m_pScene->GetRoot() );
		m_pNode->AttachChild( m_pReflector[i] );
	}

	m_pNode->AttachChild( m_pDiffuseActor->GetNode() );
	

	m_pScene->AddEntity( m_pNode );
	m_pScene->AddCamera( m_pCamera );

	//m_pRenderer11->SetMultiThreadingState( false );
}
//--------------------------------------------------------------------------------
void App::Update()
{
	// Update the timer to determine the elapsed time since last frame.  This can 
	// then used for animation during the frame.

	m_pTimer->Update();

	// Create a series of time factors for use in the simulation.  The factors 
	// are as follows:
	// x: Time in seconds since the last frame.
	// y: Current framerate, which is updated once per second.
	// z: Time in seconds since application startup.
	// w: Current frame number since application startup.

	Vector4f TimeFactors = Vector4f( m_pTimer->Elapsed()*2.0f, (float)m_pTimer->Framerate(), 
		m_pTimer->Runtime(), (float)m_pTimer->FrameCount() );

	Vector4f ViewPosition = Vector4f( m_pCamera->GetNode()->Position().x,
		                              m_pCamera->GetNode()->Position().y,
		                              m_pCamera->GetNode()->Position().z,
		                              1.0f );
	m_pRenderer11->m_pParamMgr->SetVectorParameter( std::wstring( L"ViewPosition" ), &ViewPosition );

	//std::wstringstream s;
	//s << L"Frame Number: " << m_pTimer->FrameCount() << L" Elapsed Time: " << m_pTimer->Elapsed();
	//Log::Get().Write( s.str() );

	m_pRenderer11->m_pParamMgr->SetVectorParameter( std::wstring( L"TimeFactors" ), &TimeFactors );

	// Send an event to everyone that a new frame has started.  This will be used
	// in later examples for using the material system with render views.

	EventManager::Get()->ProcessEvent( new EvtFrameStart() );


	// Manipulate the scene here - simply rotate the root of the scene in this
	// example.

	Matrix3f rotation;
	rotation.RotationY( m_pTimer->Elapsed() * 0.2f );
	m_pNode->Rotation() *= rotation;


	// Update the scene, and then render all cameras within the scene.

	//m_pRenderer11->StartPipelineStatistics();

	m_pScene->Update( m_pTimer->Elapsed() );
	m_pScene->Render( m_pRenderer11 );

	//m_pRenderer11->EndPipelineStatistics();
	//Log::Get().Write( m_pRenderer11->PrintPipelineStatistics() );

	std::wstringstream out;
	out << L"Hieroglyph 3 : Water Simulation\nFPS: " << m_pTimer->Framerate();


	//m_pSpriteRenderer->RenderText( m_pRenderer11->pImmPipeline, m_pRenderer11->m_pParamMgr, *m_pFont, out.str().c_str(), Matrix4f::Identity() );

	// Perform the rendering and presentation for the window.

	m_pRenderer11->Present( m_pWindow->GetHandle(), m_pWindow->GetSwapChain() );


	// Save a screenshot if desired.  This is done by pressing the 's' key, which
	// demonstrates how an event is sent and handled by an event listener (which
	// in this case is the application object itself).

	if ( m_bSaveScreenshot  )
	{
		m_bSaveScreenshot = false;
		m_pRenderer11->pImmPipeline->SaveTextureScreenShot( 0, std::wstring( L"WaterSimulation_" ), D3DX11_IFF_BMP );
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