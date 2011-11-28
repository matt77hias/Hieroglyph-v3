//--------------------------------------------------------------------------------
#include "App.h"
#include "Log.h"

#include <sstream>

#include "EventManager.h"
#include "EvtFrameStart.h"
#include "EvtChar.h"
#include "EvtKeyUp.h"
#include "EvtKeyDown.h"

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
}
//--------------------------------------------------------------------------------
bool App::ConfigureEngineComponents()
{
	if ( !ConfigureRenderingEngineComponents( 800, 600, D3D_FEATURE_LEVEL_11_0 ) ) {
		return( false );
	}

	if ( !ConfigureRenderingSetup() ) {
		return( false );
	}

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
	// Set any parameters that will be needed by the shaders used above.
	
	m_LightParams = Vector4f( 1.0f, 1.0f, 1.0f, 1.0f );
	m_pLightColor = m_pRenderer11->m_pParamMgr->GetVectorParameterRef( std::wstring( L"LightColor" ) );
	m_pLightColor->InitializeParameterData( &m_LightParams );

	m_LightPosition = Vector4f( 20.0f, 20.0f, -20.0f, 0.0f );
	m_pLightPosition = m_pRenderer11->m_pParamMgr->GetVectorParameterRef( std::wstring( L"LightPositionWS" ) );
	m_pLightPosition->InitializeParameterData( &m_LightPosition );

	m_pTimeFactors = m_pRenderer11->m_pParamMgr->GetVectorParameterRef( std::wstring( L"TimeFactors" ) );
	m_pViewPosition = m_pRenderer11->m_pParamMgr->GetVectorParameterRef( std::wstring( L"ViewPosition" ) );


	// Create the camera, and the render view that will produce an image of the 
	// from the camera's point of view of the scene.

	m_pCamera->GetNode()->Rotation().Rotation( Vector3f( static_cast<float>( D3DX_PI ) / 6.0f, static_cast<float>( D3DX_PI ) / 4.0f, 0.0f ) );
	m_pCamera->GetNode()->Position() = Vector3f( -5.0f, 5.0f, -5.0f );
	m_pRenderView->SetBackColor( Vector4f( 0.2f, 0.2f, 0.2f, 0.2f ) );


	// Create the scene and add the entities to it.  Then add the camera to the
	// scene so that it will be updated via the scene interface instead of 
	// manually manipulating it.

	static float radius = 20.0f;

	m_pNode = new Node3D();
	
	m_pDiffuseActor = new Actor();
	m_pDiffuseActor->GetNode()->AttachController( new RotationController( Vector3f::Random(), 0.05f ) );

    DiffuseSphereEntity::LoadResources();
	for ( int i = 0; i < 200; i++ )
	{
		Vector3f pos = Vector3f::Random();
		pos.Normalize();

		Entity3D* pEntity = new DiffuseSphereEntity();
		pEntity->Position() = pos * radius;
		pEntity->AttachController( new RotationController( Vector3f::Random(), 1.0f ) );

		m_pDiffuseActor->GetNode()->AttachChild( pEntity );
		m_pDiffuseActor->AddElement( pEntity );
	}

	m_pReflectiveActor = new Actor();		
	

	for ( int i = 0; i < 3; i++ )
	{
		m_pReflector[i] = new ReflectiveSphereEntity();
		m_pReflector[i]->Position() = Vector3f( -1.0, 1.0f+(i*3), 0.0f );
		m_pReflector[i]->m_pParaboloidView->SetRoot( m_pScene->GetRoot() );
		
		m_pNode->AttachChild( m_pReflector[i] );
	}

	m_pReflector[0]->Position() = Vector3f( -1.0, 1.0f, 1.0f );
	m_pReflector[1]->Position() = Vector3f( 1.0, 1.0f, 1.0f );
	m_pReflector[2]->Position() = Vector3f( 0.0, -1.0f, 1.0f );

	m_pReflector[0]->m_pParaboloidView->SetBackColor( Vector4f( 0.75f, 0.0f, 0.0f, 1.0f ) );
	m_pReflector[1]->m_pParaboloidView->SetBackColor( Vector4f( 0.0f, 0.75f, 0.0f, 1.0f ) );
	m_pReflector[2]->m_pParaboloidView->SetBackColor( Vector4f( 0.0f, 0.0f, 0.75f, 1.0f ) );

	m_pNode->AttachChild( m_pDiffuseActor->GetNode() );
	

	m_pScene->AddEntity( m_pNode );
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

	m_pViewPosition->InitializeParameterData( &ViewPosition );
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

	std::wstringstream out;
	out << L"Hieroglyph 3 : " << GetName() << std::endl;
	out << L"FPS: " << m_pTimer->Framerate();
	m_pTextOverlayView->WriteText( out.str(), Matrix4f::TranslationMatrix( 5.0f, 5.0f, 0.0f ),
		Vector4f( 1.0f, 1.0f, 1.0f, 1.0f ) );

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

	// TODO: Delete the remaining entities and actors...

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

	return( RenderApplication::HandleEvent( pEvent ) );
}
//--------------------------------------------------------------------------------
std::wstring App::GetName( )
{
	return( std::wstring( L"MirrorMirror" ) );
}
//--------------------------------------------------------------------------------