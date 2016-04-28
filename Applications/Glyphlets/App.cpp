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

#include "SpriteFontLoaderDX11.h"

#include "VectorParameterDX11.h"
#include "ParticleStormGlyphlet.h"
#include "ScriptedGlyphlet.h"

using namespace Glyph3;
//--------------------------------------------------------------------------------
App AppInstance; // Provides an instance of the application
//--------------------------------------------------------------------------------


//--------------------------------------------------------------------------------
App::App()
{
	m_bSaveScreenshot = false;
	m_uiInputTarget = 1;

	// These are handled by the subclass, but declared here.
    RequestEvent( SYSTEM_RBUTTON_DOWN );
    RequestEvent( SYSTEM_RBUTTON_UP );
    RequestEvent( SYSTEM_MOUSE_MOVE );
    RequestEvent( SYSTEM_MOUSE_LEAVE );
    RequestEvent( RENDER_FRAME_START );
}
//--------------------------------------------------------------------------------
bool App::ConfigureEngineComponents()
{
	if ( !ConfigureRenderingEngineComponents( 512, 768, D3D_FEATURE_LEVEL_11_0 ) ) {
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

	m_pCamera->Spatial().SetTranslation( Vector3f( 0.0f, 5.0f, -10.0f ) );
	m_pRenderView->SetBackColor( Vector4f( 0.2f, 0.2f, 0.4f, 0.0f ) );

	// Create the actor to hold the immediate geometry.  

	SpriteFontPtr pSpriteFont = SpriteFontLoaderDX11::LoadFont( std::wstring( L"Consolas" ), 20.0f, 0, true );
	

	m_pHeadline1 = new TextActor();
	m_pHeadline1->SetFont( pSpriteFont );
	m_pHeadline1->SetCharacterHeight( 0.8f );
	m_pHeadline1->GetNode()->Transform.Position() = Vector3f( -5.0f, 12.0f, 10.1f );
	m_pHeadline1->DrawString( L"Scripted Glyphlet:" );
	m_pScene->AddActor( m_pHeadline1 );

	m_pBodyText1 = new TextActor();
	m_pBodyText1->SetCharacterHeight( 0.5f );
	m_pBodyText1->GetNode()->Transform.Position() = Vector3f( -5.0f, 11.0f, 10.1f );
	m_pBodyText1->DrawString( L"This scene is generated and controlled through Lua!" );
	m_pScene->AddActor( m_pBodyText1 );

	m_pActor1 = new GlyphletActor();
	SingleWindowGlyphletPtr pScriptedGlyphlet( new ScriptedGlyphlet() );
	pScriptedGlyphlet->Initialize();
	m_pActor1->SetGlyphlet( pScriptedGlyphlet );
	m_pActor1->GetNode()->Transform.Position() = Vector3f( 0.0f, 7.5f, 10.0f );
	m_pScene->AddActor( m_pActor1 );

	m_pHeadline2 = new TextActor();
	m_pHeadline2->SetFont( pSpriteFont );
	m_pHeadline2->SetCharacterHeight( 0.8f );
	m_pHeadline2->GetNode()->Transform.Position() = Vector3f( -5.0f, 4.0f, 10.1f );
	m_pHeadline2->DrawString( L"Particle Storm Glyphlet:" );
	m_pScene->AddActor( m_pHeadline2 );

	m_pBodyText2 = new TextActor();
	m_pBodyText2->SetCharacterHeight( 0.5f );
	m_pBodyText2->GetNode()->Transform.Position() = Vector3f( -5.0f, 3.0f, 10.1f );
	m_pBodyText2->DrawString( L"This scene is generated and controlled through C++!" );
	m_pScene->AddActor( m_pBodyText2 );

	m_pActor2 = new GlyphletActor();
	SingleWindowGlyphletPtr pParticleStormGlyphlet( new ParticleStormGlyphlet() );
	pParticleStormGlyphlet->Initialize();
	m_pActor2->SetGlyphlet( pParticleStormGlyphlet );
	m_pActor2->GetNode()->Transform.Position() = Vector3f( 0.0f, -0.5f, 10.0f );
	m_pScene->AddActor( m_pActor2 );



	// Do a one time initialization of the scene lighting parameters here
	Vector4f Ka( 0.2f, 0.2f, 0.2f, 0.2f );
	VectorParameterDX11* FactorKa = m_pRenderer11->m_pParamMgr->GetVectorParameterRef( std::wstring( L"Ka" ) );
	FactorKa->InitializeParameterData( &Ka );

	Vector4f Kd( 0.5f, 0.5f, 0.5f, 0.5f );
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

	m_pActor1->m_pGlyphlet->Update( m_pTimer->Elapsed() * 1.0f );
	m_pActor2->m_pGlyphlet->Update( m_pTimer->Elapsed() * 0.5f );

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
	// Print the framerate out for the log before shutting down.

	std::wstringstream out;
	out << L"Max FPS: " << m_pTimer->MaxFramerate();
	Log::Get().Write( out.str() );
}
//--------------------------------------------------------------------------------
bool App::HandleEvent( EventPtr pEvent )
{
	eEVENT e = pEvent->GetEventType();

	if ( e == SYSTEM_KEYBOARD_KEYUP )
	{
		EvtKeyUpPtr pKeyUp = std::static_pointer_cast<EvtKeyUp>( pEvent );

		unsigned int key = pKeyUp->GetCharacterCode();

		
		if ( key == 0x31 ) // Pressing the '1' key will pass events to the main application
		{
			m_uiInputTarget = 1;
			return( true );
		}
		else if ( key == 0x32 ) // Pressing the '2' key will pass events to the first glyphlet
		{
			m_uiInputTarget = 2;
			return( true );
		}
		else if ( key == 0x33 ) // Pressing the '3' key will pass events to the first glyphlet
		{
			m_uiInputTarget = 3;
			return( true );
		}
	}

	// All other key up events will be forwarded appropriately:

	bool bHandled = false;

	if ( m_uiInputTarget == 2 ) {
		bHandled = m_pActor1->m_pGlyphlet->EvtManager.ProcessEvent( pEvent );
	}
	else if ( m_uiInputTarget == 3 ) {
		bHandled = m_pActor2->m_pGlyphlet->EvtManager.ProcessEvent( pEvent );
	}

	// Call the parent class's event handler if we haven't handled the event.

	if ( bHandled ) 
		return( true );
	else
		return( RenderApplication::HandleEvent( pEvent ) );
}
//--------------------------------------------------------------------------------
std::wstring App::GetName( )
{
	return( std::wstring( L"Glyphlets" ) );
}
//--------------------------------------------------------------------------------