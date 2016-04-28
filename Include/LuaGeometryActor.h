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
// LuaGeometryActor
//
//--------------------------------------------------------------------------------
#ifndef LuaGeometryActor_h
#define LuaGeometryActor_h
//--------------------------------------------------------------------------------
#include "ScriptManager.h"
#include "GeometryActor.h"
#include "Scene.h"
//--------------------------------------------------------------------------------
namespace Glyph3
{
	class LuaGeometryActor
	{
	public:
		LuaGeometryActor();
		virtual ~LuaGeometryActor();

		static void Register( lua_State* L );
		static GeometryActor* CheckType( lua_State* L, int n );

		static int Creator( lua_State* L );
		static int Destroyer( lua_State* L );

		static int SetColor( lua_State* L );

		static int DrawSphere( lua_State* L );
		static int DrawDisc( lua_State* L );
		static int DrawCylinder( lua_State* L );
		static int DrawBox( lua_State* L );
		static int DrawRect( lua_State* L );

	};
};
//--------------------------------------------------------------------------------
#endif // LuaGeometryActor_h
