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
#include "FirstPersonCamera.h"

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
	if ( !ConfigureRenderingEngineComponents( 640, 480, D3D_FEATURE_LEVEL_11_0 ) ) {
		return( false );
	}

	if ( !ConfigureRenderingSetup() ) {
		return( false );
	}

	return( true );
}
//--------------------------------------------------------------------------------
bool App::ConfigureRenderingSetup()
{
	// Create the camera, and the render view that will produce an image of the 
	// from the camera's point of view of the scene.

	m_pRenderView = new ViewAmbientOcclusion( *m_pRenderer11, m_BackBuffer );
	m_pRenderView->SetBackColor( Vector4f( 0.6f, 0.6f, 0.9f, 1.0f ) );

	m_pTextOverlayView = new ViewTextOverlay( *m_pRenderer11, m_BackBuffer );


	m_pCamera = new FirstPersonCamera();
	m_pCamera->GetNode()->Rotation().Rotation( Vector3f( 0.5f, 0.0f, 0.0f ) );
	m_pCamera->GetNode()->Position() = Vector3f( 0.0f, 6.0f, -8.0f );
	m_pCamera->SetCameraView( m_pRenderView );
	m_pCamera->SetOverlayView( m_pTextOverlayView );
	m_pCamera->SetProjectionParams( 0.1f, 1000.0f, static_cast<float>(m_iWidth) / static_cast<float>(m_iHeight), static_cast<float>( D3DX_PI ) / 4.0f );

	m_pScene->AddCamera( m_pCamera );

	return( true );
}
//--------------------------------------------------------------------------------
void App::ShutdownEngineComponents()
{
	ShutdownRenderingSetup();
	ShutdownRenderingEngineComponents();
}
//--------------------------------------------------------------------------------
void App::Initialize()
{
	// Create and initialize the geometry to be rendered.  

	GeometryPtr pGeometry = GeometryLoaderDX11::loadMS3DFile2( std::wstring( L"Sample_Scene.ms3d" ) );
	pGeometry->LoadToBuffers();
	pGeometry->SetPrimitiveType( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );

	// Create the material for use by the entity.
	MaterialPtr pMaterial = MaterialPtr( new MaterialDX11() );

	// Create and fill the effect that will be used for this view type
	RenderEffectDX11* pDepthEffect = new RenderEffectDX11();

	pDepthEffect->m_iVertexShader = 
		m_pRenderer11->LoadShader( VERTEX_SHADER,
		std::wstring( L"DepthVS.hlsl" ),
		std::wstring( L"VSMAIN" ),
		std::wstring( L"vs_5_0" ) );
	pDepthEffect->m_iPixelShader = 
		m_pRenderer11->LoadShader( PIXEL_SHADER,
		std::wstring( L"DepthPS.hlsl" ),
		std::wstring( L"PSMAIN" ),
		std::wstring( L"ps_5_0" ) );

	// Enable the material to render the given view type, and set its effect.
	pMaterial->Params[VT_LINEAR_DEPTH_NORMAL].bRender = true;
	pMaterial->Params[VT_LINEAR_DEPTH_NORMAL].pEffect = pDepthEffect;

	// Create and fill the effect that will be used for this view type
	RenderEffectDX11* pEffect = new RenderEffectDX11();

	pEffect->m_iVertexShader = 
		m_pRenderer11->LoadShader( VERTEX_SHADER,
		std::wstring( L"FinalVS.hlsl" ),
		std::wstring( L"VSMAIN" ),
		std::wstring( L"vs_5_0" ) );
	pEffect->m_iPixelShader = 
		m_pRenderer11->LoadShader( PIXEL_SHADER,
		std::wstring( L"FinalPS.hlsl" ),
		std::wstring( L"PSMAIN" ),
		std::wstring( L"ps_5_0" ) );

	// Enable the material to render the given view type, and set its effect.
	pMaterial->Params[VT_PERSPECTIVE].bRender = true;
	pMaterial->Params[VT_PERSPECTIVE].pEffect = pEffect;

	// Create the scene and add the entities to it.  Then add the camera to the
	// scene so that it will be updated via the scene interface instead of 
	// manually manipulating it.

	m_pNode = new Node3D();
	m_pEntity = new Entity3D();
	m_pEntity->SetGeometry( pGeometry );
	m_pEntity->SetMaterial( pMaterial );
	m_pEntity->Position() = Vector3f( 0.0f, 0.0f, 0.0f );  

	m_pNode->AttachChild( m_pEntity );

	m_pScene->AddEntity( m_pNode );

	// Get a handle to the render parameters that the application will be setting.
	m_pTimeFactors = m_pRenderer11->m_pParamMgr->GetVectorParameterRef( std::wstring( L"TimeFactors" ) );

	Vector4f FinalColor = Vector4f( 0.5f, 1.0f, 0.5f, 1.0f );
	m_pFinalColor = m_pRenderer11->m_pParamMgr->GetVectorParameterRef( std::wstring( L"FinalColor" ) );
	m_pFinalColor->InitializeParameterData( &m_pFinalColor );

	Vector4f LightParams = Vector4f( 1.0f, 1.0f, 1.0f, 1.0f );
	m_pLightColor = m_pRenderer11->m_pParamMgr->GetVectorParameterRef( std::wstring( L"LightColor" ) );
	m_pLightColor->InitializeParameterData( &LightParams );

	Vector4f LightPosition = Vector4f( 10.0f, 20.0f, -20.0f, 0.0f );
	m_pLightPosition = m_pRenderer11->m_pParamMgr->GetVectorParameterRef( std::wstring( L"LightPositionWS" ) );
	m_pLightPosition->InitializeParameterData( &LightPosition );
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

	Vector4f TimeFactors = Vector4f( m_pTimer->Elapsed(), (float)m_pTimer->Framerate(), 
		m_pTimer->Runtime(), (float)m_pTimer->FrameCount() );

	m_pTimeFactors->InitializeParameterData( &TimeFactors );


	// Send an event to everyone that a new frame has started.  This will be used
	// in later examples for using the material system with render views.

	EventManager::Get()->ProcessEvent( new EvtFrameStart( *m_pTimer ) );


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

	// Call the parent class's event handler if we haven't handled the event.

	return( Application::HandleEvent( pEvent ) );
}
//--------------------------------------------------------------------------------
std::wstring App::GetName( )
{
	return( std::wstring( L"AmbientOcclusionI" ) );
}
//--------------------------------------------------------------------------------