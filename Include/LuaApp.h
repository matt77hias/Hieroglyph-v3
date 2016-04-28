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
// LuaApp
//
//--------------------------------------------------------------------------------
#ifndef LuaApp_h
#define LuaApp_h
//--------------------------------------------------------------------------------
#include "ScriptManager.h"
#include "Scene.h"
//--------------------------------------------------------------------------------
namespace Glyph3
{
	class LuaApp
	{
	public:
		LuaApp();
		virtual ~LuaApp();

		static void Register( lua_State* L );

		static int Log( lua_State* L );
		static int Framerate( lua_State* L );
	};
};
//--------------------------------------------------------------------------------
#endif // LuaApp_h
