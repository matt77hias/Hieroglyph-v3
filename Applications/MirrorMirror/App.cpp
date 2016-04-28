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

	m_pRenderer11->MultiThreadingConfig.SetConfiguration( false );

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

	m_pCamera->Spatial().SetRotation( Vector3f( static_cast<float>( GLYPH_PI ) / 6.0f, static_cast<float>( GLYPH_PI ) / 4.0f, 0.0f ) );
	m_pCamera->Spatial().SetTranslation( Vector3f( -5.0f, 5.0f, -5.0f ) );
	m_pRenderView->SetBackColor( Vector4f( 0.2f, 0.2f, 0.2f, 0.2f ) );


	// Create the scene and add the entities to it.  Then add the camera to the
	// scene so that it will be updated via the scene interface instead of 
	// manually manipulating it.

	static float radius = 20.0f;

	
	m_pDiffuseActor = new Actor();
	m_pDiffuseActor->GetNode()->Controllers.Attach( new RotationController<Node3D>( Vector3f::Random(), 0.05f ) );

    DiffuseSphereEntity::LoadResources();
	for ( int i = 0; i < 200; i++ )
	{
		Vector3f pos = Vector3f::Random();
		pos.Normalize();

		Entity3D* pEntity = new DiffuseSphereEntity();
		pEntity->Transform.Position() = pos * radius;
		pEntity->Controllers.Attach( new RotationController<Entity3D>( Vector3f::Random(), 1.0f ) );

		m_pDiffuseActor->GetNode()->AttachChild( pEntity );
		m_pDiffuseActor->AddElement( pEntity );
	}

	m_pActor = new Actor();

	for ( int i = 0; i < 3; i++ )
	{
		ReflectiveSphereEntity* pReflector = new ReflectiveSphereEntity();
		pReflector->Transform.Position() = Vector3f( -1.0, 1.0f+(i*3), 0.0f );
		pReflector->m_pParaboloidView->SetScene( m_pScene );
		m_vReflectors.push_back( pReflector );
		m_pActor->AddElement( pReflector );
		m_pActor->GetNode()->AttachChild( pReflector );
	}

	m_vReflectors[0]->Transform.Position() = Vector3f( -1.0, 1.0f, 1.0f );
	m_vReflectors[1]->Transform.Position() = Vector3f( 1.0, 1.0f, 1.0f );
	m_vReflectors[2]->Transform.Position() = Vector3f( 0.0, -1.0f, 1.0f );

	m_vReflectors[0]->m_pParaboloidView->SetBackColor( Vector4f( 0.75f, 0.0f, 0.0f, 1.0f ) );
	m_vReflectors[1]->m_pParaboloidView->SetBackColor( Vector4f( 0.0f, 0.75f, 0.0f, 1.0f ) );
	m_vReflectors[2]->m_pParaboloidView->SetBackColor( Vector4f( 0.0f, 0.0f, 0.75f, 1.0f ) );

	m_pScene->AddActor( m_pDiffuseActor );
	m_pScene->AddActor( m_pActor );
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

	Vector4f ViewPosition = Vector4f( m_pCamera->GetNode()->Transform.Position(), 1.0f );

	m_pViewPosition->InitializeParameterData( &ViewPosition );
	m_pTimeFactors->InitializeParameterData( &TimeFactors );


	// Send an event to everyone that a new frame has started.  This will be used
	// in later examples for using the material system with render views.

	EvtManager.ProcessEvent( EvtFrameStartPtr( new EvtFrameStart( m_pTimer->Elapsed() ) ) );


	// Manipulate the scene here - simply rotate the root of the scene in this
	// example.

	Matrix3f rotation;
	rotation.RotationY( m_pTimer->Elapsed() * 0.2f );
	m_pActor->GetNode()->Transform.Rotation() *= rotation;

	for ( int i = 0; i < 3; i++ ) {
		m_vReflectors[i]->m_pParaboloidView->ResetRecurrence();
	}


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

	return( RenderApplication::HandleEvent( pEvent ) );
}
//--------------------------------------------------------------------------------
std::wstring App::GetName( )
{
	return( std::wstring( L"MirrorMirror" ) );
}
//--------------------------------------------------------------------------------