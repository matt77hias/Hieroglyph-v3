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
#include "LuaScene.h"
#include "GeometryActor.h"
#include "LuaGeometryActor.h"
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
LuaScene::LuaScene()
{
}
//--------------------------------------------------------------------------------
LuaScene::~LuaScene()
{
}
//--------------------------------------------------------------------------------
int LuaScene::Creator( lua_State* l )
{
	// Create a user data in the size of a pointer.
	Scene** udata = (Scene**)lua_newuserdata( l, sizeof(Scene*) );
	*udata = new Scene();


	// Get the meta-table for the Scene class in Lua.
	luaL_getmetatable( l, "luaL_SceneLUA" );

	// Lua stack looks like this now:
	// 2 | -1 | metatable "luaL_SceneLUA"
	// 1 | -2 | userdata
	//
	lua_setmetatable( l, -2 );

	// Now the stack looks like this: 
	// 1 | -1 | userdata
	//
	// We just return the user data to the calling function like so:

	return( 1 );
}
//--------------------------------------------------------------------------------
int LuaScene::CreateExisting( lua_State* l, std::string& name, Scene* pScene )
{
	// This method takes an existing object from C++, and then registers it for 
	// use through Lua.  The object is created as a userdata that is a pointer to
	// the object, and the provided name is used to set a global reference to that
	// object.

	// Create a user data in the size of a pointer.
	Scene** udata = (Scene**)lua_newuserdata( l, sizeof(Scene*) );
	*udata = pScene;


	// Get the meta-table for the Scene class in Lua.
	luaL_getmetatable( l, "luaL_SceneCPP" );

	// Lua stack looks like this now:
	// 2 | -1 | metatable "luaL_Scene"
	// 1 | -2 | userdata
	//
	lua_setmetatable( l, -2 );

	// Now the stack looks like this: 
	// 1 | -1 | userdata
	//
	// Just set the global field to the provided name from above.
	lua_setglobal( l, name.c_str() );

	return( 1 );
}
//--------------------------------------------------------------------------------
Scene* LuaScene::CheckType( lua_State* l, int n )
{
	// Check if the udata has the metatable from either of our two object types.
	// This is done in the following manner: 
	//
	// 1. Check if the object is a userdata.
	// 2. Check if it has a metatable.
	// 3. Check if that metatable is the same as one of our two types.

	void *p = lua_touserdata( l, n );
		
	if ( p != NULL )
	{  
		if ( lua_getmetatable( l, n ) )
		{
			lua_getfield( l, LUA_REGISTRYINDEX, "luaL_SceneLUA" );  
			lua_getfield( l, LUA_REGISTRYINDEX, "luaL_SceneCPP" );
			
			// Verify that it has one of the correct mt's.
			if ( lua_rawequal( l, -1, -3 ) || lua_rawequal( l, -2, -3 ) ) {  
				lua_pop( l, 3 );  /* remove all three metatables */
				return *(Scene**)p;
			}
		}
	}

	luaL_typerror( l, n, "luaL_Scene" );  /* else error */
	
	return( nullptr );  /* to avoid warnings */
}
//--------------------------------------------------------------------------------
int LuaScene::Destroyer( lua_State* l )
{
	Scene* pScene = CheckType( l, 1 );
	
	delete pScene;

	return( 0 );
}
//--------------------------------------------------------------------------------
//int LuaScene::CreateGeometryActor( lua_State* l )
//{
//	Scene* pScene =				LuaScene::CheckType( l, 1 );
//
//	// Create a user data in the size of a pointer to a geometry actor.
//	GeometryActor** udata = (GeometryActor**)lua_newuserdata( l, sizeof(GeometryActor*) );
//	*udata = new GeometryActor();
//
//	// Add the actor to the scene.
//	pScene->AddActor( *udata );
//
//	// Get the meta-table for the GeometryActor class in Lua.
//	luaL_getmetatable( l, "luaL_GeometryActor" );
//
//	// Lua stack looks like this now:
//	// 2 | -1 | metatable "luaL_GeometryActor"
//	// 1 | -2 | userdata
//	//
//	lua_setmetatable( l, -2 );
//
//	// Now the stack looks like this: 
//	// 1 | -1 | userdata
//	//
//	// We just return the user data to the calling function like so:
//
//	return( 1 );
//}
//--------------------------------------------------------------------------------
int LuaScene::AddActor( lua_State* l )
{
	Scene* pScene =	LuaScene::CheckType( l, 1 );
	
	// TODO: There is no error checking here - need to determine the best way
	//       to check if an object is a subclass of Actor...
	void *p = lua_touserdata( l, 2 );
	Actor* pActor = *(Actor**)p;

	// Add the actor to the scene.
	pScene->AddActor( pActor );

	return( 0 );
}
//--------------------------------------------------------------------------------
void LuaScene::Register( lua_State* l )
{
	// We create two versions of the registration structure - one for objects that
	// will be created directly by Lua, and another that will be created in C++
	// and only exposed to Lua.

	luaL_Reg sSceneRegsLUA[] =
	{
		{ "new", Creator },
		{ "AddActor", AddActor },
		{ "__gc", Destroyer },
		{ NULL, NULL }
	};

	// Create the metatable for this class.  The metatable is not exposed to Lua,
	// but is used by luaL internally for identification purposes.
	luaL_newmetatable( l, "luaL_SceneLUA" );

	// Register the functions for this class into the metatable we just created as 
	// methods.
	luaL_register( l, NULL, sSceneRegsLUA );
	lua_pushvalue( l, -1 );
	lua_setfield( l, -1, "__index" );
	
	// Clear off the stack.
	lua_pop( l, 1 );


	// Now we repeat the process, except that we perform this for with the second
	// metatable from above.  In this case, we don't expose the metatable to the
	// global environment, since we don't want to allow modification of it.

	luaL_Reg sSceneRegsCPP[] =
	{
		{ "AddActor", AddActor },
		{ NULL, NULL }
	};

	// Create the metatable for this class.  The metatable is not exposed to Lua,
	// but is used by luaL internally for identification purposes.
	luaL_newmetatable( l, "luaL_SceneCPP" );

	// Register the functions for this class into the metatable we just created as
	// methods.
	luaL_register( l, NULL, sSceneRegsCPP );
	lua_pushvalue( l, -1 );
	lua_setfield( l, -1, "__index" );
	
	// Now the stack looks like this:
	// 1 | -1 | metatable "luaL_Scene
	// Pop the metatable off the stack.
	lua_pop( l, 1 );
}
//--------------------------------------------------------------------------------
