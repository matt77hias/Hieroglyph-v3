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
#include "LuaApp.h"
#include "Application.h"
#include "LuaGeometryActor.h"
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
LuaApp::LuaApp()
{
}
//--------------------------------------------------------------------------------
LuaApp::~LuaApp()
{
}
//--------------------------------------------------------------------------------
int LuaApp::Log( lua_State* L )
{
	const char* message		= lua_tostring( L, 1 );

	Log::Get().Write( GlyphString::ToUnicode( std::string( message ) ) );

	return( 0 );
}
//--------------------------------------------------------------------------------
int LuaApp::Framerate( lua_State* L )
{
	int iNumArgs			= lua_gettop( L );

	lua_pushnumber( L, Application::GetApplication()->m_pTimer->Framerate() );

	return( 1 );
}
//--------------------------------------------------------------------------------
void LuaApp::Register( lua_State* L )
{
	// We create two versions of the registration structure - one for objects that
	// will be created directly by Lua, and another that will be created in C++
	// and only exposed to Lua.

	luaL_Reg sSceneRegsLUA[] =
	{
		{ "Log", LuaApp::Log },
		{ "Framerate", LuaApp::Framerate },
		{ NULL, NULL }
	};

	// Register the functions for this class into the metatable we just created as 
	// methods.
	luaL_register( L, "App", sSceneRegsLUA );
	//lua_pushvalue( l, -1 );
	//lua_setfield( l, -1, "__index" );
	
	// Clear off the stack.
	lua_pop( L, 1 );
}
//--------------------------------------------------------------------------------
