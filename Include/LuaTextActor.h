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
// LuaTextActor
//
//--------------------------------------------------------------------------------
#ifndef LuaTextActor_h
#define LuaTextActor_h
//--------------------------------------------------------------------------------
#include "ScriptManager.h"
#include "TextActor.h"
#include "Scene.h"
//--------------------------------------------------------------------------------
namespace Glyph3
{
	class LuaTextActor
	{
	public:
		LuaTextActor();
		virtual ~LuaTextActor();

		static void Register( lua_State* L );
		static TextActor* CheckType( lua_State* L, int n );

		static int Creator( lua_State* L );
		static int Destroyer( lua_State* L );

		static int SetColor( lua_State* L );
		static int ClearText( lua_State* L );
		static int SetText( lua_State* L );
		static int AppendText( lua_State* L );
		static int SetTextOrigin( lua_State* L );
		static int SetTextOrientation( lua_State* L );
		static int SetFont( lua_State* L );
		static int SetCharacterHeight( lua_State* L );
	};
};
//--------------------------------------------------------------------------------
#endif // LuaTextActor_h
