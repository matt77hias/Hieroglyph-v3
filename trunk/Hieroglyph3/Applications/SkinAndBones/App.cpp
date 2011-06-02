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

#include "IParameterManager.h"

#include "GeometryGeneratorDX11.h"
#include "ShaderResourceParameterWriterDX11.h"

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
		m_pTimer->SetFixedTimeStep( 1.0f / 80.0f );
	}

	// Create the window.
	int width = 800;
	int height = 600;

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

	m_pRenderer11->pImmPipeline->SetViewPort( m_pRenderer11->CreateViewPort( viewport ) );
	
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

	// Create the light parameters for use with this effect

	Vector4f LightParams = Vector4f( 0.2f, 0.7f, 0.2f, 0.7f );
	m_pLightColor = m_pRenderer11->m_pParamMgr->GetVectorParameterRef( std::wstring( L"LightColor" ) );
	m_pLightColor->InitializeParameterData( &LightParams );

	Vector4f LightPosition = Vector4f( -1000.0f, 200.0f, 0.0f, 0.0f );
	m_pLightPosition = m_pRenderer11->m_pParamMgr->GetVectorParameterRef( std::wstring( L"LightPositionWS" ) );
	m_pLightPosition->InitializeParameterData( &LightPosition );


	// Create the camera, and the render view that will produce an image of the 
	// from the camera's point of view of the scene.

	m_pCamera = new Camera();
	m_pCamera->GetNode()->Rotation().Rotation( Vector3f( 0.30f, 1.5f, 0.0f ) );
	m_pCamera->GetNode()->Position() = Vector3f( -20.0f, 25.0f, 10.0f );
	m_pRenderView = new ViewPerspective( *m_pRenderer11, m_RenderTarget, m_DepthTarget );
	m_pRenderView->SetBackColor( Vector4f( 0.1f, 0.1f, 0.3f, 0.0f ) );
	m_pCamera->SetCameraView( m_pRenderView );
	m_pCamera->SetProjectionParams( 0.1f, 1000.0f, 800.0f / 600.0f, static_cast<float>( D3DX_PI ) / 2.0f );


	// Create the scene and add the entities to it.  Then add the camera to the
	// scene so that it will be updated via the scene interface instead of 
	// manually manipulating it.

	m_pNode = new Node3D();


	// Create the displaced skinned actor

	m_pDisplacedActor = new SkinnedActor();
	GeometryDX11* pGeometry = new GeometryDX11();
	GeometryGeneratorDX11::GenerateWeightedSkinnedCone( pGeometry, 10, 20, 2.0f, 40.0f, 6, m_pDisplacedActor );
	pGeometry->SetPrimitiveType( D3D11_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST );
	m_pDisplacedActor->GetBody()->SetMaterial( MaterialGeneratorDX11::GenerateSkinnedSolid( *m_pRenderer11 ) );
	m_pDisplacedActor->GetNode()->Position() = Vector3f( 0.0f, 0.0f, 0.0f );

	
	// Create the skinned actor without displacement

	m_pSkinnedActor = new SkinnedActor();
	GeometryDX11* pSkinnedGeometry = new GeometryDX11();
	GeometryGeneratorDX11::GenerateWeightedSkinnedCone( pSkinnedGeometry, 10, 20, 2.0f, 40.0f, 6, m_pSkinnedActor );
	pSkinnedGeometry->SetPrimitiveType( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
	m_pSkinnedActor->GetBody()->SetMaterial( MaterialGeneratorDX11::GenerateSkinnedTextured( *m_pRenderer11 ) );
	m_pSkinnedActor->GetNode()->Position() = Vector3f( 0.0f, 0.0f, 40.0f );

	
	// Generate the static mesh, and attach a texture to its entity

	m_pStaticActor = new Actor();
	m_pStaticActor->GetBody()->Position() = Vector3f( 0.0f, 0.0f, 20.0f );
	GeometryDX11* pStaticGeometry = GeometryLoaderDX11::loadMS3DFile2( std::wstring( L"../Data/Models/box.ms3d" ) );
	pStaticGeometry->LoadToBuffers();
	MaterialDX11* pStaticMaterial = MaterialGeneratorDX11::GenerateStaticTextured(*RendererDX11::Get());
	m_pStaticActor->GetBody()->SetGeometry( pStaticGeometry );
	m_pStaticActor->GetBody()->SetMaterial( pStaticMaterial );
	
	ResourcePtr ColorTexture = RendererDX11::Get()->LoadTexture( L"../Data/Textures/Tiles.png" );
	ShaderResourceParameterWriterDX11* pTextureParameter = new ShaderResourceParameterWriterDX11();
	pTextureParameter->SetRenderParameterRef(
		(RenderParameterDX11*)m_pRenderer11->m_pParamMgr->GetShaderResourceParameterRef( std::wstring( L"ColorTexture" ) ) );
	pTextureParameter->SetValue( ColorTexture );
	m_pStaticActor->GetBody()->AddRenderParameter( pTextureParameter );


	// Attach the actors to the scene, so that they will be rendered all together.

	m_pNode->AttachChild( m_pDisplacedActor->GetNode() );
	m_pNode->AttachChild( m_pSkinnedActor->GetNode() );
	m_pNode->AttachChild( m_pStaticActor->GetNode() );

	m_pScene->AddEntity( m_pNode );
	m_pScene->AddCamera( m_pCamera );


	// Setup the skinned actors' bind poses and start their animations.

	m_pDisplacedActor->SetBindPose();
	m_pDisplacedActor->SetSkinningMatrices( *m_pRenderer11 );
	m_pDisplacedActor->PlayAllAnimations();

	m_pSkinnedActor->SetBindPose();
	m_pSkinnedActor->SetSkinningMatrices( *m_pRenderer11 );
	m_pSkinnedActor->PlayAllAnimations();

	m_pRenderer11->SetMultiThreadingState( false );
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


	// Update the scene, and then render all cameras within the scene.

	m_pScene->Update( m_pTimer->Elapsed() );

	m_pDisplacedActor->SetSkinningMatrices( *m_pRenderer11 );
	m_pSkinnedActor->SetSkinningMatrices( *m_pRenderer11 );

	m_pScene->Render( m_pRenderer11 );


	// Perform the rendering and presentation for the window.

	m_pRenderer11->Present( m_pWindow->GetHandle(), m_pWindow->GetSwapChain() );


	// Save a screenshot if desired.  This is done by pressing the 's' key, which
	// demonstrates how an event is sent and handled by an event listener (which
	// in this case is the application object itself).

	//if ( m_bSaveScreenshot  )
	{
		m_bSaveScreenshot = false;
		m_pRenderer11->pImmPipeline->SaveTextureScreenShot( 0, std::wstring( L"SkinAndBones_" ), D3DX11_IFF_BMP );
	}
}
//--------------------------------------------------------------------------------
void App::Shutdown()
{
	SAFE_DELETE( m_pStaticActor );
	SAFE_DELETE( m_pSkinnedActor );
	SAFE_DELETE( m_pDisplacedActor );
	
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
	return( std::wstring( L"SkinAndBones" ) );
}
//--------------------------------------------------------------------------------