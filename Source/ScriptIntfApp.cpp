//--------------------------------------------------------------------------------
// This file is a portion of the Hieroglyph 3 Rendering Engine.  It is distributed
// under the MIT License, available in the root of this distribution and 
// at the following URL:
//
// http://www.opensource.org/licenses/mit-license.php
//
// Copyright (c) Jason Zink 
//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
#include "PCH.h"
#include "ScriptIntfApp.h"
#include "Log.h"
#include "GlyphString.h"
#include "Actor.h"
#include "GeometryLoaderDX11.h"
#include "MaterialGeneratorDX11.h"
#include "RenderApplication.h"
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
ScriptIntfApp::ScriptIntfApp()
{
	InitializeInterface();
}
//--------------------------------------------------------------------------------
ScriptIntfApp::~ScriptIntfApp()
{
}
//--------------------------------------------------------------------------------
int ScriptIntfApp::Log( lua_State* pLuaState )
{
	int iNumArgs			= lua_gettop( pLuaState );

	const char* message		= lua_tostring( pLuaState, 1 );

	Log::Get().Write( GlyphString::ToUnicode( std::string( message ) ) );

	return( 0 );
}
//--------------------------------------------------------------------------------
int ScriptIntfApp::Framerate( lua_State* pLuaState )
{
	int iNumArgs			= lua_gettop( pLuaState );

	lua_pushnumber( pLuaState, Application::GetApplication()->m_pTimer->Framerate() );

	return( 1 );
}
//--------------------------------------------------------------------------------
int ScriptIntfApp::CreateActor( lua_State* pLuaState )
{
	// The arguments to create actor are two strings - one for the geometry file
	// to use and the other for the material identifier.

	int iNumArgs			= lua_gettop( pLuaState );

	const char* geometry	= lua_tostring( pLuaState, 1 );
	const char* material	= lua_tostring( pLuaState, 2 );

	// TODO: utilize a common material for all of the objects created in this way!
	Actor* pActor = new Actor();
	GeometryPtr Geometry = GeometryLoaderDX11::loadMS3DFile2( GlyphString::ToUnicode( std::string( geometry ) ) );
	Geometry->LoadToBuffers();
	pActor->GetBody()->Visual.SetGeometry( Geometry );
	pActor->GetBody()->Visual.SetMaterial( MaterialGeneratorDX11::GeneratePhong( *RendererDX11::Get() ) );

	// Add this actor the application's scene.

	Application::GetApplication()->m_pScene->AddActor( pActor );

	// Register the object with the ScriptManager, and get a handle to the object.
	// This is the return value for this function.

	unsigned int handle = ScriptManager::Get()->RegisterEngineObject( "Actor", pActor );

	lua_pushinteger( pLuaState, handle );

	return( 1 );
}
//--------------------------------------------------------------------------------
void ScriptIntfApp::Initialize()
{
	lua_State* pLuaState = ScriptManager::Get()->GetState();
	lua_getfield( pLuaState, LUA_GLOBALSINDEX, "Initialize" );
    if ( lua_pcall( pLuaState, 0, 0, 0 ) )
	{
		ScriptManager::Get()->ReportErrors();
	}
}
//--------------------------------------------------------------------------------
void ScriptIntfApp::Update( float time )
{
	lua_State* pLuaState = ScriptManager::Get()->GetState();
	lua_getfield( pLuaState, LUA_GLOBALSINDEX, "Update" );
    lua_pushnumber( pLuaState, time );
    if ( lua_pcall( pLuaState, 1, 0, 0 ) )
	{
		ScriptManager::Get()->ReportErrors();
	}
}
//--------------------------------------------------------------------------------
void ScriptIntfApp::Render()
{
	lua_State* pLuaState = ScriptManager::Get()->GetState();
	lua_getfield( pLuaState, LUA_GLOBALSINDEX, "Render" );
    if ( lua_pcall( pLuaState, 0, 0, 0 ) )
	{
		ScriptManager::Get()->ReportErrors();
	}
}
//--------------------------------------------------------------------------------
void ScriptIntfApp::Shutdown()
{
	lua_State* pLuaState = ScriptManager::Get()->GetState();
	lua_getfield( pLuaState, LUA_GLOBALSINDEX, "Shutdown" );
    if ( lua_pcall( pLuaState, 0, 0, 0 ) )
	{
		ScriptManager::Get()->ReportErrors();
	}
}
//--------------------------------------------------------------------------------
void ScriptIntfApp::OnKeyDown( unsigned int key )
{
	lua_State* pLuaState = ScriptManager::Get()->GetState();
	lua_getfield( pLuaState, LUA_GLOBALSINDEX, "OnKeyDown" );
	lua_pushnumber( pLuaState, key );
    if ( lua_pcall( pLuaState, 1, 0, 0 ) )
	{
		ScriptManager::Get()->ReportErrors();
	}
}
//--------------------------------------------------------------------------------
void ScriptIntfApp::InitializeInterface()
{
	ScriptManager::Get()->RegisterEngineClass( "App" );
	ScriptManager::Get()->RegisterClassFunction( "App", "Log", ScriptIntfApp::Log );
	ScriptManager::Get()->RegisterClassFunction( "App", "Framerate", ScriptIntfApp::Framerate );
	ScriptManager::Get()->RegisterClassFunction( "App", "CreateActor", ScriptIntfApp::CreateActor );
}
//--------------------------------------------------------------------------------