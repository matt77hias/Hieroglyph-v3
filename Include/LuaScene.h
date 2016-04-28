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
// LuaScene
//
//--------------------------------------------------------------------------------
#ifndef LuaScene_h
#define LuaScene_h
//--------------------------------------------------------------------------------
#include "ScriptManager.h"
#include "Scene.h"
//--------------------------------------------------------------------------------
namespace Glyph3
{
	class LuaScene
	{
	public:
		LuaScene();
		virtual ~LuaScene();

		static void Register( lua_State* L );
		static Scene* CheckType( lua_State* L, int n );

		static int Creator( lua_State* L );
		static int CreateExisting( lua_State* L, std::string& name, Scene* pScene );
		static int Destroyer( lua_State* L );

		static int AddActor( lua_State* L );
	};
};
//--------------------------------------------------------------------------------
#endif // LuaScene_h
