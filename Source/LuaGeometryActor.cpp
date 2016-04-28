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
#include "LuaGeometryActor.h"
#include "GeometryActor.h"
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
LuaGeometryActor::LuaGeometryActor()
{
}
//--------------------------------------------------------------------------------
LuaGeometryActor::~LuaGeometryActor()
{
}
//--------------------------------------------------------------------------------
int LuaGeometryActor::Creator( lua_State* L )
{
	// Create a user data in the size of a pointer to a geometry actor.
	GeometryActor** udata = (GeometryActor**)lua_newuserdata( L, sizeof(GeometryActor*) );
	*udata = new GeometryActor();

	// TODO: This is only temporary until a proper scene connection interface is
	//       available on the Lua side!
	//LuaGeometryActor::pScene->AddActor( *udata );

	// Get the meta-table for the GeometryActor class in Lua.
	luaL_getmetatable( L, "luaL_GeometryActor" );

	// Lua stack looks like this now:
	// 2 | -1 | metatable "luaL_GeometryActor"
	// 1 | -2 | userdata
	//
	lua_setmetatable( L, -2 );

	// Now the stack looks like this: 
	// 1 | -1 | userdata
	//
	// We just return the user data to the calling function like so:

	return( 1 );
}
//--------------------------------------------------------------------------------
GeometryActor* LuaGeometryActor::CheckType( lua_State* L, int n )
{
	return( *(GeometryActor**)luaL_checkudata( L, n, "luaL_GeometryActor" ) );
}
//--------------------------------------------------------------------------------
int LuaGeometryActor::Destroyer( lua_State* L )
{
	GeometryActor* pActor = CheckType( L, 1 );
	
	// TODO: This is only temporary until it gets sorted out.  The scene will 
	//       delete the actor, but we should probably know about it here anyway...
	//delete pActor;

	return( 0 );
}
//--------------------------------------------------------------------------------
int LuaGeometryActor::SetColor( lua_State* L )
{
	GeometryActor* pActor = CheckType( L, 1 );
	
	Vector4f c;
	c.x = static_cast<float>( luaL_checknumber( L, 2 ) );
	c.y = static_cast<float>( luaL_checknumber( L, 3 ) );
	c.z = static_cast<float>( luaL_checknumber( L, 4 ) );
	c.w = static_cast<float>( luaL_checknumber( L, 5 ) );
	
	pActor->SetColor( c );

	return( 0 );
}
//--------------------------------------------------------------------------------
int LuaGeometryActor::DrawSphere( lua_State* L )
{
	GeometryActor* pActor = CheckType( L, 1 );
	
	Vector3f p;
	p.x = static_cast<float>( luaL_checknumber( L, 2 ) );
	p.y = static_cast<float>( luaL_checknumber( L, 3 ) );
	p.z = static_cast<float>( luaL_checknumber( L, 4 ) );
	float r = static_cast<float>( luaL_checknumber( L, 5 ) );
	
	pActor->DrawSphere( p, r );

	return( 0 );
}
//--------------------------------------------------------------------------------
int LuaGeometryActor::DrawDisc( lua_State* L )
{
	GeometryActor* pActor = CheckType( L, 1 );

	Vector3f center;
	Vector3f normal;
	float radius;
	
	center.x = static_cast<float>( luaL_checknumber( L, 2 ) );
	center.y = static_cast<float>( luaL_checknumber( L, 3 ) );
	center.z = static_cast<float>( luaL_checknumber( L, 4 ) );

	normal.x = static_cast<float>( luaL_checknumber( L, 5 ) );
	normal.y = static_cast<float>( luaL_checknumber( L, 6 ) );
	normal.z = static_cast<float>( luaL_checknumber( L, 7 ) );

	radius = static_cast<float>( luaL_checknumber( L, 8 ) );

	pActor->DrawDisc( center, normal, radius );

	return( 0 );
}
//--------------------------------------------------------------------------------
int LuaGeometryActor::DrawCylinder( lua_State* L )
{
	GeometryActor* pActor = CheckType( L, 1 );

	Vector3f p1;
	Vector3f p2;
	float r1;
	float r2;
	
	p1.x = static_cast<float>( luaL_checknumber( L, 2 ) );
	p1.y = static_cast<float>( luaL_checknumber( L, 3 ) );
	p1.z = static_cast<float>( luaL_checknumber( L, 4 ) );

	p2.x = static_cast<float>( luaL_checknumber( L, 5 ) );
	p2.y = static_cast<float>( luaL_checknumber( L, 6 ) );
	p2.z = static_cast<float>( luaL_checknumber( L, 7 ) );

	r1 = static_cast<float>( luaL_checknumber( L, 8 ) );
	r2 = static_cast<float>( luaL_checknumber( L, 9 ) );

	pActor->DrawCylinder( p1, p2, r1, r2 );

	return( 0 );
}
//--------------------------------------------------------------------------------
int LuaGeometryActor::DrawBox( lua_State* L )
{
	GeometryActor* pActor = CheckType( L, 1 );

	Vector3f center;
	Vector3f xdir;
	Vector3f ydir;
	Vector3f zdir;
	Vector3f extents;
	
	center.x = static_cast<float>( luaL_checknumber( L, 2 ) );
	center.y = static_cast<float>( luaL_checknumber( L, 3 ) );
	center.z = static_cast<float>( luaL_checknumber( L, 4 ) );

	xdir.x = static_cast<float>( luaL_checknumber( L, 5 ) );
	xdir.y = static_cast<float>( luaL_checknumber( L, 6 ) );
	xdir.z = static_cast<float>( luaL_checknumber( L, 7 ) );

	ydir.x = static_cast<float>( luaL_checknumber( L, 8 ) );
	ydir.y = static_cast<float>( luaL_checknumber( L, 9 ) );
	ydir.z = static_cast<float>( luaL_checknumber( L, 10 ) );

	zdir.x = static_cast<float>( luaL_checknumber( L, 11 ) );
	zdir.y = static_cast<float>( luaL_checknumber( L, 12 ) );
	zdir.z = static_cast<float>( luaL_checknumber( L, 13 ) );

	extents.x = static_cast<float>( luaL_checknumber( L, 14 ) );
	extents.y = static_cast<float>( luaL_checknumber( L, 15 ) );
	extents.z = static_cast<float>( luaL_checknumber( L, 16 ) );

	pActor->DrawBox( center, xdir, ydir, zdir, extents );

	return( 0 );
}
//--------------------------------------------------------------------------------
int LuaGeometryActor::DrawRect( lua_State* L )
{
	GeometryActor* pActor = CheckType( L, 1 );

	Vector3f center;
	Vector3f xdir;
	Vector3f ydir;
	Vector2f extents;
	
	center.x = static_cast<float>( luaL_checknumber( L, 2 ) );
	center.y = static_cast<float>( luaL_checknumber( L, 3 ) );
	center.z = static_cast<float>( luaL_checknumber( L, 4 ) );

	xdir.x = static_cast<float>( luaL_checknumber( L, 5 ) );
	xdir.y = static_cast<float>( luaL_checknumber( L, 6 ) );
	xdir.z = static_cast<float>( luaL_checknumber( L, 7 ) );

	ydir.x = static_cast<float>( luaL_checknumber( L, 8 ) );
	ydir.y = static_cast<float>( luaL_checknumber( L, 9 ) );
	ydir.z = static_cast<float>( luaL_checknumber( L, 10 ) );

	extents.x = static_cast<float>( luaL_checknumber( L, 11 ) );
	extents.y = static_cast<float>( luaL_checknumber( L, 12 ) );

	pActor->DrawRect( center, xdir, ydir, extents );

	return( 0 );
}
//--------------------------------------------------------------------------------
void LuaGeometryActor::Register( lua_State* L )
{
	luaL_Reg sGeometryActorRegs[] =
	{
		{ "new",			LuaGeometryActor::Creator },
		{ "SetColor",		LuaGeometryActor::SetColor },
		{ "DrawSphere",		LuaGeometryActor::DrawSphere },
		{ "DrawDisc",		LuaGeometryActor::DrawDisc },
		{ "DrawCylinder",	LuaGeometryActor::DrawCylinder },
		{ "DrawBox",		LuaGeometryActor::DrawBox },
		{ "DrawRect",		LuaGeometryActor::DrawRect },
		{ "__gc",			LuaGeometryActor::Destroyer },
		{ NULL,				NULL }
	};

	// Create the metatable for this class.  The metatable is not exposed to Lua,
	// but is used by luaL internally for identification purposes.
	luaL_newmetatable( L, "luaL_GeometryActor" );

	// Register the functions for this class into the metatable we just created.
	luaL_register( L, NULL, sGeometryActorRegs );
	
	// Now the stack looks like this:
	// 1 | -1 | metatable "luaL_GeometryActor
	// Put another copy of the metatable onto the stack.
	lua_pushvalue( L, -1 );

	// Now the stack looks like this:
	// 2 | -1 | metatable "luaL_GeometryActor"
	// 1 | -2 | metatable "luaL_GeometryActor"
	//
	// Set the "__index" field of the metatable to point to itself.  This pops
	// the stack one element.
	lua_setfield( L, -1, "__index" );

	// Now we use setglobal to officially expose the luaL_GeometryActor metatable
	// to Lua, except that we use the name "GeometryActor".
	//
	// This allows Lua scripts to override the metatable of GeometryActor.
	lua_setglobal( L, "GeometryActor" );
}
//--------------------------------------------------------------------------------
