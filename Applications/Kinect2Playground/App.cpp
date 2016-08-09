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
#include "EvtFrameStart.h"
//--------------------------------------------------------------------------------
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
	if ( !ConfigureRenderingEngineComponents( 1024, 800, D3D_FEATURE_LEVEL_11_0 ) ) {
		return( false );
	}

	if ( !ConfigureRenderingSetup() ) {
		return( false );
	}

	// Create the Kinect manager
	m_pKinectManager = std::make_unique<Kinect2Manager>();
	m_pKinectManager->Initialize();

	return( true );
}
//--------------------------------------------------------------------------------
void App::ShutdownEngineComponents()
{
	// Shutdown the Kinect manager, and the unique ptr will go out of scope with
	// when the application instance is destroyed.

	m_pKinectManager->Shutdown();

	ShutdownRenderingSetup();
	ShutdownRenderingEngineComponents();
}
//--------------------------------------------------------------------------------
void App::Initialize()
{
	// Configure the camera, and the render view that will produce an image of the 
	// from the camera's point of view of the scene.

	m_pCamera->Spatial().SetTranslation( Vector3f( 2.0f, 2.0f, -2.0f ) );
	m_pCamera->Spatial().SetRotation( Vector3f( 0.3f, -0.4f, 0.0f ) );
	m_pRenderView->SetBackColor( Vector4f( 0.1f, 0.1f, 0.3f, 0.0f ) );


	// Add a single point light to the scene.

	m_pLight = new PointLight();
	m_pLight->GetNode()->Transform.Position().y = 5.0f;
	m_pScene->AddLight( m_pLight );


	// Create a skeleton actor to show the joint tracking

	m_pSkeletonActor = new Kinect2SkeletonActor();
	m_pScene->AddActor( m_pSkeletonActor );
}
//--------------------------------------------------------------------------------
void App::Update()
{
	// Update the timer to determine the elapsed time since last frame.  This can 
	// then used for animation during the frame.

	m_pTimer->Update();

	// Update the skeleton actor based on the active bodies.  We assume that there
	// is only one person visible, but this can easily be accommodated with either
	// multiple SkeletonActor instances or you could modify the single SkeletonActor
	// to draw all of the available bodies.

	m_pKinectManager->Update();

	for ( unsigned int i = 0; i < 6; ++i ) {
		if ( m_pKinectManager->GetBody(i).tracked == true ) {
			m_pSkeletonActor->UpdateSkeleton( m_pKinectManager->GetBody(i) );
		}
	}

	// Send an event to everyone that a new frame has started.  This will be used
	// in later examples for using the material system with render views.

	EventManager::Get()->ProcessEvent( EvtFrameStartPtr( new EvtFrameStart( m_pTimer->Elapsed() ) ) );


	std::wstringstream out;
	out << L"Hieroglyph 3 : Kinect 2 Playground" << std::endl;
	out << L"FPS: " << m_pTimer->Framerate();

	m_pTextOverlayView->WriteText( out.str(), Matrix4f::Identity(), Vector4f( 1.0f, 1.0f, 1.0f, 1.0f ) );

	// Update the scene, and then render all cameras within the scene.

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
	return( std::wstring( L"Kinect2Playground" ) );
}
//--------------------------------------------------------------------------------