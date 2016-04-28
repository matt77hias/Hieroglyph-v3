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
#include "LuaTextActor.h"
#include "TextActor.h"
#include "SpriteFontLoaderDX11.h"
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
LuaTextActor::LuaTextActor()
{
}
//--------------------------------------------------------------------------------
LuaTextActor::~LuaTextActor()
{
}
//--------------------------------------------------------------------------------
int LuaTextActor::Creator( lua_State* L )
{
	// Create a user data in the size of a pointer to a geometry actor.
	TextActor** udata = (TextActor**)lua_newuserdata( L, sizeof(TextActor*) );
	*udata = new TextActor();

	// TODO: This is only temporary until a proper scene connection interface is
	//       available on the Lua side!
	//LuaTextActor::pScene->AddActor( *udata );

	// Get the meta-table for the TextActor class in Lua.
	luaL_getmetatable( L, "luaL_TextActor" );

	// Lua stack looks like this now:
	// 2 | -1 | metatable "luaL_TextActor"
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
TextActor* LuaTextActor::CheckType( lua_State* L, int n )
{
	return( *(TextActor**)luaL_checkudata( L, n, "luaL_TextActor" ) );
}
//--------------------------------------------------------------------------------
int LuaTextActor::Destroyer( lua_State* L )
{
	TextActor* pActor = CheckType( L, 1 );
	
	// TODO: This is only temporary until it gets sorted out.  The scene will 
	//       delete the actor, but we should probably know about it here anyway...
	//delete pActor;

	return( 0 );
}
//--------------------------------------------------------------------------------
int LuaTextActor::SetColor( lua_State* L )
{
	TextActor* pActor = CheckType( L, 1 );
	
	Vector4f c;
	c.x = static_cast<float>( luaL_checknumber( L, 2 ) );
	c.y = static_cast<float>( luaL_checknumber( L, 3 ) );
	c.z = static_cast<float>( luaL_checknumber( L, 4 ) );
	c.w = static_cast<float>( luaL_checknumber( L, 5 ) );
	
	pActor->SetColor( c );

	return( 0 );
}
//--------------------------------------------------------------------------------
int LuaTextActor::ClearText( lua_State* L )
{
	TextActor* pActor = CheckType( L, 1 );

	pActor->ClearText();

	return( 0 );
}
//--------------------------------------------------------------------------------
int LuaTextActor::SetText( lua_State* L )
{
	TextActor* pActor = CheckType( L, 1 );
	const char* text = lua_tostring( L, 2 );

	pActor->SetText( GlyphString::ToUnicode( std::string( text ) ) );

	return( 0 );
}
//--------------------------------------------------------------------------------
int LuaTextActor::AppendText( lua_State* L )
{
	TextActor* pActor = CheckType( L, 1 );
	const char* text = lua_tostring( L, 2 );

	pActor->AppendText( GlyphString::ToUnicode( std::string( text ) ) );

	return( 0 );
}
//--------------------------------------------------------------------------------
int LuaTextActor::SetTextOrigin( lua_State* L )
{
	TextActor* pActor = CheckType( L, 1 );
	
	Vector3f p;
	p.x = static_cast<float>( luaL_checknumber( L, 2 ) );
	p.y = static_cast<float>( luaL_checknumber( L, 3 ) );
	p.z = static_cast<float>( luaL_checknumber( L, 4 ) );
	
	pActor->GetNode()->Transform.Position() = p;

	return( 0 );
}
//--------------------------------------------------------------------------------
int LuaTextActor::SetTextOrientation( lua_State* L )
{
	TextActor* pActor = CheckType( L, 1 );
	
	Vector3f xdir, ydir;
	xdir.x = static_cast<float>( luaL_checknumber( L, 2 ) );
	xdir.y = static_cast<float>( luaL_checknumber( L, 3 ) );
	xdir.z = static_cast<float>( luaL_checknumber( L, 4 ) );

	ydir.x = static_cast<float>( luaL_checknumber( L, 5 ) );
	ydir.y = static_cast<float>( luaL_checknumber( L, 6 ) );
	ydir.z = static_cast<float>( luaL_checknumber( L, 7 ) );

	pActor->SetTextOrientation( xdir, ydir );

	return( 0 );
}
//--------------------------------------------------------------------------------
int LuaTextActor::SetFont( lua_State* L )
{
	TextActor* pActor = CheckType( L, 1 );

	std::string fontName = luaL_checkstring( L, 2 );
	float fontSize = static_cast<float>( luaL_checknumber( L, 3 ) );
	unsigned int fontStyle = static_cast<unsigned int>( luaL_checknumber( L, 4 ) );
	bool antialiased = ( lua_toboolean( L, 5 ) );

	SpriteFontPtr pFont = SpriteFontLoaderDX11::LoadFont( GlyphString::ToUnicode(fontName), fontSize, fontStyle, antialiased );

	pActor->SetFont( pFont );

	return( 0 );
}
//--------------------------------------------------------------------------------
int LuaTextActor::SetCharacterHeight( lua_State* L )
{
	TextActor* pActor = CheckType( L, 1 );
	float fHeight = static_cast<float>( luaL_checknumber( L, 2 ) );
		
	pActor->SetCharacterHeight( fHeight );

	return( 0 );
}
//--------------------------------------------------------------------------------
void LuaTextActor::Register( lua_State* L )
{
	luaL_Reg sTextActorRegs[] =
	{
		{ "new",				LuaTextActor::Creator },
		{ "SetColor",			LuaTextActor::SetColor },
		{ "ClearText",			LuaTextActor::ClearText },
		{ "SetText",			LuaTextActor::SetText },
		{ "AppendText",			LuaTextActor::AppendText },
		{ "SetTextOrigin",		LuaTextActor::SetTextOrigin },
		{ "SetTextOrientation",	LuaTextActor::SetTextOrientation },
		{ "SetFont",			LuaTextActor::SetFont },
		{ "SetCharacterHeight",	LuaTextActor::SetCharacterHeight },
		{ "__gc",				LuaTextActor::Destroyer },
		{ NULL,	NULL }
	};

	// Create the metatable for this class.  The metatable is not exposed to Lua,
	// but is used by luaL internally for identification purposes.
	luaL_newmetatable( L, "luaL_TextActor" );

	// Register the functions for this class into the metatable we just created.
	luaL_register( L, NULL, sTextActorRegs );
	
	// Now the stack looks like this:
	// 1 | -1 | metatable "luaL_TextActor
	// Put another copy of the metatable onto the stack.
	lua_pushvalue( L, -1 );

	// Now the stack looks like this:
	// 2 | -1 | metatable "luaL_TextActor"
	// 1 | -2 | metatable "luaL_TextActor"
	//
	// Set the "__index" field of the metatable to point to itself.  This pops
	// the stack one element.
	lua_setfield( L, -1, "__index" );

	// Now we use setglobal to officially expose the luaL_TextActor metatable
	// to Lua, except that we use the name "TextActor".
	//
	// This allows Lua scripts to override the metatable of TextActor.
	lua_setglobal( L, "TextActor" );
}
//--------------------------------------------------------------------------------
