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
	int width = 640;
	int height = 480;

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

	m_pBruteForceGaussian = new RenderEffectDX11();
	m_pBruteForceGaussian->m_iComputeShader = 
		m_pRenderer11->LoadShader( COMPUTE_SHADER, 
		std::wstring( L"../Data/Shaders/GaussianBruteForceCS.hlsl" ),
		std::wstring( L"CSMAIN" ),
		std::wstring( L"cs_5_0" ) );

	m_pSeparableGaussianX = new RenderEffectDX11();
	m_pSeparableGaussianX->m_iComputeShader = 
		m_pRenderer11->LoadShader( COMPUTE_SHADER, 
		std::wstring( L"../Data/Shaders/GaussianSeparableCS.hlsl" ),
		std::wstring( L"CSMAINX" ),
		std::wstring( L"cs_5_0" ) );

	m_pSeparableGaussianY = new RenderEffectDX11();
	m_pSeparableGaussianY->m_iComputeShader = 
		m_pRenderer11->LoadShader( COMPUTE_SHADER, 
		std::wstring( L"../Data/Shaders/GaussianSeparableCS.hlsl" ),
		std::wstring( L"CSMAINY" ),
		std::wstring( L"cs_5_0" ) );

	m_pCachedGaussianX = new RenderEffectDX11();
	m_pCachedGaussianX->m_iComputeShader = 
		m_pRenderer11->LoadShader( COMPUTE_SHADER, 
		std::wstring( L"../Data/Shaders/GaussianCachedCS.hlsl" ),
		std::wstring( L"CSMAINX" ),
		std::wstring( L"cs_5_0" ) );

	m_pCachedGaussianY = new RenderEffectDX11();
	m_pCachedGaussianY->m_iComputeShader = 
		m_pRenderer11->LoadShader( COMPUTE_SHADER, 
		std::wstring( L"../Data/Shaders/GaussianCachedCS.hlsl" ),
		std::wstring( L"CSMAINY" ),
		std::wstring( L"cs_5_0" ) );

	// Create the material for use by the entity.
	MaterialDX11* pMaterial = new MaterialDX11();

	RenderEffectDX11* pEffect = new RenderEffectDX11();
	pEffect->m_iVertexShader = 
		m_pRenderer11->LoadShader( VERTEX_SHADER,
		std::wstring( L"../Data/Shaders/TextureVS.hlsl" ),
		std::wstring( L"VSMAIN" ),
		std::wstring( L"vs_5_0" ) );
	pEffect->m_iPixelShader = 
		m_pRenderer11->LoadShader( PIXEL_SHADER,
		std::wstring( L"../Data/Shaders/TexturePS.hlsl" ),
		std::wstring( L"PSMAIN" ),
		std::wstring( L"ps_5_0" ) );

	// Enable the material to render the given view type, and set its effect.
	pMaterial->Params[VT_PERSPECTIVE].bRender = true;
	pMaterial->Params[VT_PERSPECTIVE].pEffect = pEffect;

	// Create the geometry that will fill the screen with our results
	GeometryDX11* pGeometry = new GeometryDX11();
	GeometryGeneratorDX11::GenerateFullScreenQuad( pGeometry );
	pGeometry->LoadToBuffers();


	// Here we load our desired texture, and create a shader resource view to 
	// use for input to the compute shader.  By specifying null for the 
	// configuration, the view is created from the default resource configuration.

	m_Texture = m_pRenderer11->LoadTexture( L"../Data/Textures/Outcrop.png" );

	

	// Create the texture for output of the compute shader.
	Texture2dConfigDX11 FilteredConfig;
	FilteredConfig.SetColorBuffer( 640, 480 ); 
	FilteredConfig.SetBindFlags( D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE );

	m_Intermediate = m_pRenderer11->CreateTexture2D( &FilteredConfig, 0 );
	m_Output = m_pRenderer11->CreateTexture2D( &FilteredConfig, 0 );


	// Specify the bindings for the resources.  These take as input a parameter
	// name and a resource proxy object created above.  This will connect these
	// resources with the appropriate shader variables at rendering time.  The
	// resource proxy object contains the needed 'ResourceView' instances.

	m_pRenderer11->m_pParamMgr->SetShaderResourceParameter( L"ColorMap00", m_Output );


	// Create the camera, and the render view that will produce an image of the 
	// from the camera's point of view of the scene.

	m_pCamera = new Camera();
	m_pCamera->GetNode()->Rotation().Rotation( Vector3f( 0.307f, 0.707f, 0.0f ) );
	m_pCamera->GetNode()->Position() = Vector3f( -70.0f, 30.5f, -75.0f );
	m_pRenderView = new ViewPerspective( *m_pRenderer11, m_RenderTarget, m_DepthTarget );
	m_pRenderView->SetBackColor( Vector4f( 0.2f, 0.2f, 0.2f, 0.2f ) );
	m_pCamera->SetCameraView( m_pRenderView );
	m_pCamera->SetProjectionParams( 0.1f, 1000.0f, static_cast<float>( D3DX_PI ) / 2.0f, 640.0f / 480.0f );

	// Create the scene and add the entities to it.  Then add the camera to the
	// scene so that it will be updated via the scene interface instead of 
	// manually manipulating it.

	m_pNode = new Node3D();
	m_pEntity = new Entity3D();
	m_pEntity->SetGeometry( pGeometry );
	m_pEntity->SetMaterial( pMaterial, false );
	//m_pEntity->Position() = Vector3f( -8.0f * DispatchSizeX, 0.0f, -8.0f * DispatchSizeZ );  

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


	// Perform the filtering with the compute shader.  The assumption in this case
	// is that the texture is 640x480 - if there is a different size then the 
	// dispatch call can be modified accordingly.

	//m_pRenderer11->m_pParamMgr->SetShaderResourceParameter( L"InputMap", m_Texture );
	//m_pRenderer11->m_pParamMgr->SetUnorderedAccessParameter( L"OutputMap", m_Output );
	//m_pRenderer11->pImmPipeline->Dispatch( *m_pBruteForceGaussian, 20, 15, 1, m_pRenderer11->m_pParamMgr );



	//m_pRenderer11->m_pParamMgr->SetShaderResourceParameter( L"InputMap", m_Texture );
	//m_pRenderer11->m_pParamMgr->SetUnorderedAccessParameter( L"OutputMap", m_Intermediate );
	//m_pRenderer11->pImmPipeline->Dispatch( *m_pSeparableGaussianX, 1, 480, 1, m_pRenderer11->m_pParamMgr );

	//m_pRenderer11->m_pParamMgr->SetShaderResourceParameter( L"InputMap", m_Intermediate );
	//m_pRenderer11->m_pParamMgr->SetUnorderedAccessParameter( L"OutputMap", m_Output );
	//m_pRenderer11->pImmPipeline->Dispatch( *m_pSeparableGaussianY, 640, 1, 1, m_pRenderer11->m_pParamMgr );



	m_pRenderer11->m_pParamMgr->SetShaderResourceParameter( L"InputMap", m_Texture );
	m_pRenderer11->m_pParamMgr->SetUnorderedAccessParameter( L"OutputMap", m_Intermediate );
	m_pRenderer11->pImmPipeline->Dispatch( *m_pCachedGaussianX, 1, 480, 1, m_pRenderer11->m_pParamMgr );

	m_pRenderer11->m_pParamMgr->SetShaderResourceParameter( L"InputMap", m_Intermediate );
	m_pRenderer11->m_pParamMgr->SetUnorderedAccessParameter( L"OutputMap", m_Output );
	m_pRenderer11->pImmPipeline->Dispatch( *m_pCachedGaussianY, 640, 1, 1, m_pRenderer11->m_pParamMgr );


	//m_pRenderer11->StartPipelineStatistics();

	m_pScene->Update( m_pTimer->Elapsed() );
	m_pScene->Render( m_pRenderer11 );

	//m_pRenderer11->EndPipelineStatistics();
	//Log::Get().Write( m_pRenderer11->PrintPipelineStatistics() );

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