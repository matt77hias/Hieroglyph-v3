//--------------------------------------------------------------------------------
// This file is a portion of the Hieroglyph 3 Rendering Engine.  It is distributed
// under the MIT License, available in the root of this distribution and 
// at the following URL:
//
// http://www.opensource.org/licenses/mit-license.php
//
// Copyright (c) Jason Zink 
//--------------------------------------------------------------------------------
#include "ScriptedGlyphlet.h"
#include "LuaApp.h"
#include "LuaScene.h"
#include "LuaGeometryActor.h"
#include "LuaTextActor.h"
#include "ConsoleWindow.h"
#include "EvtFrameStart.h"
#include "EvtKeyDown.h"
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
ScriptedGlyphlet::ScriptedGlyphlet()
{

}
//--------------------------------------------------------------------------------
ScriptedGlyphlet::~ScriptedGlyphlet()
{
}
//--------------------------------------------------------------------------------
void ScriptedGlyphlet::Initialize()
{
	m_pScriptManager = new ScriptManager();

	// Create the camera, and the render view that will produce an image of the 
	// from the camera's point of view of the scene.

	m_pCamera->Spatial().SetRotation( Vector3f( 0.0f, 0.0f, 0.0f ) );
	m_pCamera->Spatial().SetTranslation( Vector3f( 0.0f, 0.0f, -10.0f ) );
	//m_pCamera->Spatial().SetRotation( Vector3f( 0.307f, 0.707f, 0.0f ) );
	//m_pCamera->Spatial().SetTranslation( Vector3f( -10.0f, 6.5f, -10.0f ) );

	m_pRenderView->SetBackColor( Vector4f( 0.0f, 0.0f, 0.0f, 0.0f ) );

	// Create the scene and add the entities to it.  Then add the camera to the
	// scene so that it will be updated via the scene interface instead of 
	// manually manipulating it.


	// Initialize the 'App' and 'Actor' interface
	//ScriptIntfApp AppIntf;
	//ScriptIntfActor ActorIntf;

	LuaApp::Register( m_pScriptManager->GetState() );
	LuaScene::Register( m_pScriptManager->GetState() );
	LuaGeometryActor::Register( m_pScriptManager->GetState() );
	LuaTextActor::Register( m_pScriptManager->GetState() );

	// Make this Glyphlet's scene object available to Lua.
	LuaScene::CreateExisting( m_pScriptManager->GetState(), std::string( "GlyphScene" ), m_pScene );


	// Load and process the main script file.
	m_pScriptManager->Run( "../Data/Scripts/GlyphletsTestScript.lua" );

	// Call the script-based initialization function.
	lua_getfield( m_pScriptManager->GetState(), LUA_GLOBALSINDEX, "Initialize" );
    if ( lua_pcall( m_pScriptManager->GetState(), 0, 0, 0 ) ) {
		ScriptManager::Get()->ReportErrors();
	}

	//ConsoleWindow::StartConsole( 0, m_pScriptManager );
}
//--------------------------------------------------------------------------------
void ScriptedGlyphlet::Update( float dt )
{
	// Send an event to everyone that a new frame has started.  This will be used
	// in later examples for using the material system with render views.

	EvtManager.ProcessEvent( EvtFrameStartPtr( new EvtFrameStart( dt ) ) );


	// Call the script-based update function.

	lua_getfield( m_pScriptManager->GetState(), LUA_GLOBALSINDEX, "Update" );
    lua_pushnumber( m_pScriptManager->GetState(), dt );
    if ( lua_pcall( m_pScriptManager->GetState(), 1, 0, 0 ) ) {
		ScriptManager::Get()->ReportErrors();
	}


	// After the application specific update, call the base class update method
	// to perform the rendering.

	SingleWindowGlyphlet::Update( dt );
}
//--------------------------------------------------------------------------------
void ScriptedGlyphlet::Shutdown()
{
	// Call the script-based shutdown function.
	lua_getfield( m_pScriptManager->GetState(), LUA_GLOBALSINDEX, "Shutdown" );
    if ( lua_pcall( m_pScriptManager->GetState(), 0, 0, 0 ) ) {
		ScriptManager::Get()->ReportErrors();
	}

	//ConsoleWindow::StopConsole();

	SAFE_DELETE( m_pScriptManager );
}
//--------------------------------------------------------------------------------
bool ScriptedGlyphlet::HandleEvent( EventPtr pEvent )
{
	// Handle events for your scene here...
	eEVENT e = pEvent->GetEventType();

	if ( e == SYSTEM_KEYBOARD_KEYDOWN )
	{
		EvtKeyDownPtr pKeyDown = std::static_pointer_cast<EvtKeyDown>( pEvent );

		unsigned int key = pKeyDown->GetCharacterCode();

		lua_getfield( m_pScriptManager->GetState(), LUA_GLOBALSINDEX, "OnKeyDown" );
		lua_pushnumber( m_pScriptManager->GetState(), key );
		if ( lua_pcall( m_pScriptManager->GetState(), 1, 0, 0 ) ) {
			ScriptManager::Get()->ReportErrors();
		}

	}



	return( false );
}
//--------------------------------------------------------------------------------
std::wstring ScriptedGlyphlet::GetName()
{
	return( L"ScriptedGlyphlet" );
}
//--------------------------------------------------------------------------------