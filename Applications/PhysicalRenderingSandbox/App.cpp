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
#include "FileSystem.h"

#include <sstream>

#include "EventManager.h"
#include "EvtFrameStart.h"
#include "EvtKeyUp.h"
#include "EvtKeyDown.h"

#include "RotationController.h"

#include "VectorParameterDX11.h"

#include "TextureActor.h"
#include "SkyboxActor.h"

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
	if ( !ConfigureRenderingEngineComponents( 1280, 720, D3D_FEATURE_LEVEL_11_0 ) ) {
		return( false );
	}

	if ( !ConfigureRenderingSetup() ) {
		return( false );
	}

	SetMultiThreadedMode( false );

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
	// Create the camera, and the render view that will produce an image of the 
	// from the camera's point of view of the scene.

	m_pCamera->Spatial().SetRotation( Vector3f( 0.0f, 0.0f, 0.0f ) );
	m_pCamera->Spatial().SetTranslation( Vector3f( 0.0f, 2.0f, -10.0f ) );
	m_pRenderView->SetColorClearValue( Vector4f( 0.2f, 0.2f, 0.4f, 0.0f ) );

	// Create the actor to hold the immediate geometry.  

	const float count = 10.0f;
	const float radius = 0.75f;
	const float spacing = 0.01f;
	const float step = radius * 2.0f + spacing;
	const float start = -9.0f * step * 0.5f;

	for ( size_t i = 0; i < 10; i++ )
	{
		actors[i] = new GeometryActor();
		m_pScene->AddActor( actors[i] );
		actors[i]->GetNode()->Transform.Position() = Vector3f( start + step*i, 0.0f, 0.0f );
		//actors[i]->SetColor( Vector4f( 0.5f, 0.5f, 0.5f, 1.0f ) );
		actors[i]->SetColor( Vector4f( 0.95f, 0.64f, 0.54f, 1.0f ) );
		//actors[i]->SetColor( Vector4f( 0.04f, 0.22f, 0.02f, 1.0f ) );
		actors[i]->DrawSphere( Vector3f(0.0f, 0.0f, 0.0f), radius, 12, 24 );
		actors[i]->SetRoughness( max(static_cast<float>(i) / 10.0f, 0.05f) );
		actors[i]->SetMetallic( 1.0f );
	}


	m_pTextActor = new TextActor();
	//m_pTextActor->SetTextOrientation( Vector3f( 1.0f, 0.0f, 0.0f ), Vector3f( 0.0f, 1.0f, 0.0f ) );
	//m_pTextActor->SetText( std::wstring( L"Hello World! This is some \nsample text!" ) );
	
	//m_pTextActor->SetColor( Vector4f( 0.0f, 1.0f, 0.0f, 0.5f ) );
	//m_pTextActor->NewLine();
	//m_pTextActor->SetCharacterHeight( 2.0f );
	//m_pTextActor->AppendText( L"Text is colored, scaled, and alpha blended..." );

	//m_pTextActor->GetNode()->Transform.Position() = Vector3f( 0.0f, 7.0f, 0.0f );
	//m_pScene->AddActor( m_pTextActor );

	// Add a single point light to the scene.
	m_pLight = new PointLight();
	m_pScene->AddLight( m_pLight );

	m_pLight->SetDiffuse( Vector4f( 1.0f, 1.0f, 1.0f, 1.0f ) * 1.0f );
	m_pLight->GetNode()->Controllers.Attach( new RotationController<Node3D>( Vector3f( 0.0f, 1.0f, 0.0f ), -0.2f ) );
	m_pLight->GetNode()->Transform.Position() = Vector3f( 0.0f, 25.0f, 0.0f );
	m_pLight->GetBody()->Transform.Position() = Vector3f( 0.0f, 0.0f, -25.0f );




	ResourcePtr skyboxTexture = m_pRenderer11->LoadTexture( L"TropicalSunnyDay.dds" );
	SkyboxActor* skybox = new SkyboxActor( skyboxTexture, 10.0f );
	m_pScene->AddActor( skybox );
}
//--------------------------------------------------------------------------------
void App::Update()
{
	// Update the timer to determine the elapsed time since last frame.  This can 
	// then used for animation during the frame.

	m_pTimer->Update();

	EvtManager.ProcessEvent( EvtFrameStartPtr( new EvtFrameStart( m_pTimer->Elapsed() ) ) );


	// Print a message to show the framerate and sample name.

	std::wstringstream out;

	out << L"Hieroglyph 3 : " << GetName() << std::endl;
	out << L"FPS: " << m_pTimer->Framerate();
	
	m_pTextOverlayView->WriteText( out.str(), Matrix4f::TranslationMatrix( 5.0f, 5.0f, 0.0f ),
		Vector4f( 1.0f, 1.0f, 1.0f, 1.0f ) );

	
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

		if ( key == '1' ) {
			m_pCamera->SetOffsetProjectionParams( -0.4f, 0.4f, -0.3f, 0.3f, 0.5f, 100.0f );
		}
		if ( key == '2' ) {
			m_pCamera->SetOffsetProjectionParams( 0.0f, 0.8f, -0.3f, 0.3f, 0.5f, 100.0f );
		}
		if ( key == '3' ) {
			m_pCamera->SetOffsetProjectionParams( -0.8f, 0.0f, -0.3f, 0.3f, 0.5f, 100.0f );
		}
	}

	// Call the parent class's event handler if we haven't handled the event.

	return( RenderApplication::HandleEvent( pEvent ) );
}
//--------------------------------------------------------------------------------
std::wstring App::GetName( )
{
	return( std::wstring( L"PhysicalRenderingSandbox" ) );
}
//--------------------------------------------------------------------------------