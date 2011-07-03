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

#include "GeometryLoaderDX11.h"
#include "GeometryGeneratorDX11.h"
#include "MaterialGeneratorDX11.h"
#include "RasterizerStateConfigDX11.h"
#include "Texture2dConfigDX11.h"

#include "IParameterManager.h"

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
	return( ConfigureRenderingEngineComponents( 640, 480, D3D_FEATURE_LEVEL_11_0 ) );
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
	m_iImage = 0;
	m_iAlgorithm = 0;

	// Create render effects for each of the required compute shaders for all of
	// our various algorithms.

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


	m_pBruteForceBilateral = new RenderEffectDX11();
	m_pBruteForceBilateral->m_iComputeShader = 
		m_pRenderer11->LoadShader( COMPUTE_SHADER, 
		std::wstring( L"../Data/Shaders/BilateralBruteForceCS.hlsl" ),
		std::wstring( L"CSMAIN" ),
		std::wstring( L"cs_5_0" ) );

	m_pSeparableBilateralX = new RenderEffectDX11();
	m_pSeparableBilateralX->m_iComputeShader = 
		m_pRenderer11->LoadShader( COMPUTE_SHADER, 
		std::wstring( L"../Data/Shaders/BilateralSeparableCS.hlsl" ),
		std::wstring( L"CSMAINX" ),
		std::wstring( L"cs_5_0" ) );

	m_pSeparableBilateralY = new RenderEffectDX11();
	m_pSeparableBilateralY->m_iComputeShader = 
		m_pRenderer11->LoadShader( COMPUTE_SHADER, 
		std::wstring( L"../Data/Shaders/BilateralSeparableCS.hlsl" ),
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


	// Create the geometry that will fill the screen with our results - just a full
	// screen quad in this case.

	GeometryDX11* pGeometry = new GeometryDX11();
	GeometryGeneratorDX11::GenerateFullScreenQuad( pGeometry );
	pGeometry->LoadToBuffers();


	// Here we load our desired texture.

	
	m_Texture[0] = m_pRenderer11->LoadTexture( L"../Data/Textures/Outcrop.png" );
	m_Texture[1] = m_pRenderer11->LoadTexture( L"../Data/Textures/fruit.png" );
	m_Texture[2] = m_pRenderer11->LoadTexture( L"../Data/Textures/Hex.png" );
	

	// Create the texture for output of the compute shader.
	
	Texture2dConfigDX11 FilteredConfig;
	FilteredConfig.SetColorBuffer( 640, 480 ); 
	FilteredConfig.SetBindFlags( D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE );

	m_Intermediate = m_pRenderer11->CreateTexture2D( &FilteredConfig, 0 );
	m_Output = m_pRenderer11->CreateTexture2D( &FilteredConfig, 0 );


	// Get references to the parameters that will be used in rendering.

	m_pColorMapParameter = m_pRenderer11->m_pParamMgr->GetShaderResourceParameterRef( std::wstring( L"ColorMap00" ) );
	m_pInputParameter = m_pRenderer11->m_pParamMgr->GetShaderResourceParameterRef( std::wstring( L"InputMap" ) );
	m_pOutputParameter = m_pRenderer11->m_pParamMgr->GetUnorderedAccessParameterRef( std::wstring( L"OutputMap" ) );


	// Since this one won't be changing, it can be initialized here once, and then
	// not updated again later.

	m_pColorMapParameter->InitializeParameterData( &m_Output->m_iResourceSRV );


	// Create the camera, and the render view that will produce an image of the 
	// from the camera's point of view of the scene.

	m_pCamera->GetNode()->Rotation().Rotation( Vector3f( 0.307f, 0.707f, 0.0f ) );
	m_pCamera->GetNode()->Position() = Vector3f( -70.0f, 30.5f, -75.0f );
	m_pRenderView->SetBackColor( Vector4f( 0.2f, 0.2f, 0.2f, 0.2f ) );


	// Create the scene and add the entities to it.  Then add the camera to the
	// scene so that it will be updated via the scene interface instead of 
	// manually manipulating it.

	m_pNode = new Node3D();
	m_pEntity = new Entity3D();
	m_pEntity->SetGeometry( pGeometry );
	m_pEntity->SetMaterial( pMaterial, false );

	m_pNode->AttachChild( m_pEntity );
	m_pScene->AddEntity( m_pNode );
}
//--------------------------------------------------------------------------------
void App::Update()
{
	// Update the timer to determine the elapsed time since last frame.  This can 
	// then used for animation during the frame.

	m_pTimer->Update();

	// Send an event to everyone that a new frame has started.  This will be used
	// in later examples for using the material system with render views.

	EventManager::Get()->ProcessEvent( new EvtFrameStart( *m_pTimer ) );


	// Setup the text rendering for the onscreen UI.

	std::wstringstream out;
	out << L"Hieroglyph 3 : ImageProcessor\nFPS: " << m_pTimer->Framerate() << std::endl;
	out << L"Press I to select the next image" << std::endl;
	out << L"Press N to select the next algorithm" << std::endl;
	out << L"Current Algorithm: ";


	// Perform the filtering with the compute shader.  The assumption in this case
	// is that the texture is 640x480 - if there is a different size then the 
	// dispatch call can be modified accordingly.

	// Brute force Gaussian
	if ( m_iAlgorithm == 0 )
	{
		out << L"Brute Force Gaussian" << std::endl;
		m_pInputParameter->InitializeParameterData( &m_Texture[m_iImage]->m_iResourceSRV );
		m_pOutputParameter->InitializeParameterData( &m_Output->m_iResourceUAV );
		m_pRenderer11->pImmPipeline->Dispatch( *m_pBruteForceGaussian, 20, 15, 1, m_pRenderer11->m_pParamMgr );
	}

	// Separable Gaussian
	if ( m_iAlgorithm == 1 )
	{
		out << L"Separable Gaussian" << std::endl;
		m_pInputParameter->InitializeParameterData( &m_Texture[m_iImage]->m_iResourceSRV );
		m_pOutputParameter->InitializeParameterData( &m_Intermediate->m_iResourceUAV );
		m_pRenderer11->pImmPipeline->Dispatch( *m_pSeparableGaussianX, 1, 480, 1, m_pRenderer11->m_pParamMgr );

		m_pInputParameter->InitializeParameterData( &m_Intermediate->m_iResourceSRV );
		m_pOutputParameter->InitializeParameterData( &m_Output->m_iResourceUAV );
		m_pRenderer11->pImmPipeline->Dispatch( *m_pSeparableGaussianY, 640, 1, 1, m_pRenderer11->m_pParamMgr );
	}

	// Cached Gaussian
	if ( m_iAlgorithm == 2 )
	{
		out << L"Cached Separable Gaussian" << std::endl;
		m_pInputParameter->InitializeParameterData( &m_Texture[m_iImage]->m_iResourceSRV );
		m_pOutputParameter->InitializeParameterData( &m_Intermediate->m_iResourceUAV );
		m_pRenderer11->pImmPipeline->Dispatch( *m_pCachedGaussianX, 1, 480, 1, m_pRenderer11->m_pParamMgr );
	
		m_pInputParameter->InitializeParameterData( &m_Intermediate->m_iResourceSRV );
		m_pOutputParameter->InitializeParameterData( &m_Output->m_iResourceUAV );
		m_pRenderer11->pImmPipeline->Dispatch( *m_pCachedGaussianY, 640, 1, 1, m_pRenderer11->m_pParamMgr );
	}

	// Brute force Bilateral
	if ( m_iAlgorithm == 3 )
	{
		out << L"Brute Force Bilateral" << std::endl;
		m_pInputParameter->InitializeParameterData( &m_Texture[m_iImage]->m_iResourceSRV );
		m_pOutputParameter->InitializeParameterData( &m_Output->m_iResourceUAV );
		m_pRenderer11->pImmPipeline->Dispatch( *m_pBruteForceBilateral, 20, 15, 1, m_pRenderer11->m_pParamMgr );

		m_pInputParameter->InitializeParameterData( &m_Output->m_iResourceSRV );
		m_pOutputParameter->InitializeParameterData( &m_Intermediate->m_iResourceUAV );
		m_pRenderer11->pImmPipeline->Dispatch( *m_pBruteForceBilateral, 20, 15, 1, m_pRenderer11->m_pParamMgr );

		m_pInputParameter->InitializeParameterData( &m_Intermediate->m_iResourceSRV );
		m_pOutputParameter->InitializeParameterData( &m_Output->m_iResourceUAV );
		m_pRenderer11->pImmPipeline->Dispatch( *m_pBruteForceBilateral, 20, 15, 1, m_pRenderer11->m_pParamMgr );
	}

	// Separable Bilateral
	if ( m_iAlgorithm == 4 )
	{
		out << L"Separable Bilateral" << std::endl;
		m_pInputParameter->InitializeParameterData( &m_Texture[m_iImage]->m_iResourceSRV );
		m_pOutputParameter->InitializeParameterData( &m_Intermediate->m_iResourceUAV );
		m_pRenderer11->pImmPipeline->Dispatch( *m_pSeparableBilateralX, 1, 480, 1, m_pRenderer11->m_pParamMgr );

		m_pInputParameter->InitializeParameterData( &m_Intermediate->m_iResourceSRV );
		m_pOutputParameter->InitializeParameterData( &m_Output->m_iResourceUAV );
		m_pRenderer11->pImmPipeline->Dispatch( *m_pSeparableBilateralY, 640, 1, 1, m_pRenderer11->m_pParamMgr );
	}

	m_pTextOverlayView->WriteText( out.str(), Matrix4f::TranslationMatrix( 10.0f, 10.0f, 0.0f ), Vector4f( 1.0f, 1.0f, 0.0f, 1.0f ) );

	m_pScene->Update( m_pTimer->Elapsed() );
	m_pScene->Render( m_pRenderer11 );


	// Perform the rendering and presentation for the window.

	m_pRenderer11->Present( m_pWindow->GetHandle(), m_pWindow->GetSwapChain() );


	// Save a screenshot if desired.  This is done by pressing the 's' key, which
	// demonstrates how an event is sent and handled by an event listener (which
	// in this case is the application object itself).

	if ( m_bSaveScreenshot  )
	{
		m_bSaveScreenshot = false;
		m_pRenderer11->pImmPipeline->SaveTextureScreenShot( 0, GetName(), D3DX11_IFF_BMP );
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
		else if ( key == 0x4E ) // 'N' Key - Save a screen shot for the next frame
		{
			m_iAlgorithm++;
			if ( m_iAlgorithm > 4 ) m_iAlgorithm = 0;

			return( true );
		}
		else if ( key == 0x49 ) // 'I' Key - Save a screen shot for the next frame
		{
			m_iImage++;
			if ( m_iImage > 2 ) m_iImage = 0;

			return( true );
		}
		else
		{
			return( false );
		}
	}

	// Call the parent class's event handler if we haven't handled the event.

	return( RenderApplication::HandleEvent( pEvent ) );
}
//--------------------------------------------------------------------------------
std::wstring App::GetName( )
{
	return( std::wstring( L"ImageProcessor" ) );
}
//--------------------------------------------------------------------------------