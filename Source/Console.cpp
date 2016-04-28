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
#include "Console.h"
#include "RenderTargetViewDX11.h"
#include "DepthStencilViewDX11.h"
#include "UnorderedAccessViewDX11.h"
#include "Log.h"
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
Console::Console() : L( nullptr )
{
	L = lua_open();

	luaL_openlibs( L );
}
//--------------------------------------------------------------------------------
Console::~Console()
{
	lua_close( L );
}
//--------------------------------------------------------------------------------
void Console::processKey( unsigned int code )
{
	if ( code == VK_RETURN ) {
		if (line.size() > 0) {
			execute();
		}
	} else if ( code == VK_BACK ) {
		if (line.size() > 0) {
			line.pop_back();
		}
	} else if (32 <= code && code <= 127) {
		line.push_back(code);
	}
}
//--------------------------------------------------------------------------------
void Console::execute()
{
	history.push_back( ">> " + line );

	// Remove newline if it is the line ending character.
	if ( line[line.size()-1] == '\n' )
		line[line.size()-1] = '\0';

	// If the statement starts with an '=' then patch the statement to return it
	// instead.  
	if ( line[0] == '=' ) {
		line = "return " + line.substr( 1, line.size()-1 );
	}

	// Compile the line into a Lua function and push it onto the stack.
	if ( luaL_loadbuffer(L, line.c_str(), line.size(), "=stdin") ) {
		line.clear();
		reportErrors();
		lua_settop( L, 0 );
		return;
	}

	// Call the function, and return args on the stack.
	if ( lua_pcall( L, 0, LUA_MULTRET, 0 ) ) {
		line.clear();
		reportErrors( );
		lua_settop( L, 0 );
		return;
	} else {
		
		// If there are results to print, print them all!
		if ( lua_gettop(L) > 0) { 
			lua_getglobal(L, "tostring");
			lua_insert(L, 1);
			
			if (lua_pcall(L, lua_gettop(L)-1, 1, 0) != 0) {
				history.emplace_back( "error calling print..." );
			} else {
				std::string result = lua_tostring(L, lua_gettop(L));
				history.emplace_back( result );
			}
		}
	}

	// Clear the stack for next time.
	lua_settop(L, 0);

	line.clear();
}
//--------------------------------------------------------------------------------
void Console::reportErrors()
{
	std::string errormsg = lua_tostring( L, -1 );
	lua_pop( L, 1 );

	history.emplace_back( errormsg );
}
//--------------------------------------------------------------------------------
lua_State* Console::getState()
{
	return L;
}
//--------------------------------------------------------------------------------