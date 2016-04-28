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
#include "EvtKeyUp.h"
#include "EvtKeyDown.h"

#include "GeometryGeneratorDX11.h"
#include "MaterialGeneratorDX11.h"
#include "RotationController.h"

#include "VectorParameterDX11.h"

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

	m_pCamera->Spatial().SetRotation( Vector3f( 0.5f, 0.0f, 0.0f ) );
	m_pCamera->Spatial().SetTranslation( Vector3f( 0.0f, 2.0f, -4.0f ) );
	m_pRenderView->SetBackColor( Vector4f( 0.2f, 0.2f, 0.4f, 0.0f ) );

	// Create the actor to hold the immediate geometry.  

	m_pActor1 = new VolumeActor();
	m_pScene->AddActor( m_pActor1 );

	// Throw a rotation onto the actor to slowly rotate it about the Y-axis.
	RotationController<Node3D>* pRotController = new RotationController<Node3D>( Vector3f( -1.0f, 1.0f, 0.0f ), 0.2f );
	m_pActor1->GetNode()->Controllers.Attach( pRotController );



	m_pActor2 = new VolumeActor();
	m_pScene->AddActor( m_pActor2 );

	// Throw a rotation onto the actor to slowly rotate it about the Y-axis.
	RotationController<Node3D>* pRotController2 = new RotationController<Node3D>( Vector3f( 1.0f, 1.0f, 0.0f ), -0.1f );
	m_pActor2->GetNode()->Controllers.Attach( pRotController2 );


	m_pActor1->GetNode()->Transform.Position() = Vector3f( -1.2f, 0.0f, 0.0f );
	m_pActor2->GetNode()->Transform.Position() = Vector3f( 1.2f, 0.0f, 0.0f );

	// Do a one time initialization of the scene lighting parameters here
	Vector4f Ka( 0.2f, 0.2f, 0.2f, 0.2f );
	VectorParameterDX11* FactorKa = m_pRenderer11->m_pParamMgr->GetVectorParameterRef( std::wstring( L"Ka" ) );
	FactorKa->InitializeParameterData( &Ka );

	Vector4f Kd( 0.1f, 1.0f, 0.1f, 0.5f );
	VectorParameterDX11* FactorKd = m_pRenderer11->m_pParamMgr->GetVectorParameterRef( std::wstring( L"Kd" ) );
	FactorKd->InitializeParameterData( &Kd );

	Vector4f Ks( 1.0f, 1.0f, 1.0f, 1.0f );
	VectorParameterDX11* FactorKs = m_pRenderer11->m_pParamMgr->GetVectorParameterRef( std::wstring( L"Ks" ) );
	FactorKs->InitializeParameterData( &Ks );

	Vector4f LightPosition( 100.0f, 100.0f, -100.0f, 0.0f );
	VectorParameterDX11* FactorLightPosition = m_pRenderer11->m_pParamMgr->GetVectorParameterRef( std::wstring( L"LightPosition" ) );
	FactorLightPosition->InitializeParameterData( &LightPosition );

	Vector4f Ia( 0.25f, 0.25f, 0.25f, 0.25f );
	VectorParameterDX11* FactorIa = m_pRenderer11->m_pParamMgr->GetVectorParameterRef( std::wstring( L"Ia" ) );
	FactorIa->InitializeParameterData( &Ia );

	Vector4f Id( 0.5f, 0.5f, 0.5f, 1.0f );
	VectorParameterDX11* FactorId = m_pRenderer11->m_pParamMgr->GetVectorParameterRef( std::wstring( L"Id" ) );
	FactorId->InitializeParameterData( &Id );

	Vector4f Is( 1.0f, 1.0f, 1.0f, 1.0f );
	VectorParameterDX11* FactorIs = m_pRenderer11->m_pParamMgr->GetVectorParameterRef( std::wstring( L"Is" ) );
	FactorIs->InitializeParameterData( &Is );
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
	
	m_pTextOverlayView->WriteText( out.str(), 
		Matrix4f::TranslationMatrix( 5.0f, 5.0f, 0.0f ),
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
	Log::Get().Write( m_pScene->GetRoot()->toString() );

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
	return( std::wstring( L"VolumeRenderer" ) );
}
//--------------------------------------------------------------------------------