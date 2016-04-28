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
#include "ScriptIntfActor.h"
#include "VectorParameterDX11.h"
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
ScriptIntfActor::ScriptIntfActor()
{
	InitializeInterface();
}
//--------------------------------------------------------------------------------
ScriptIntfActor::~ScriptIntfActor()
{
}
//--------------------------------------------------------------------------------
int ScriptIntfActor::SetPosition( lua_State* pLuaState )
{
	int iNumArgs			= lua_gettop( pLuaState );

	unsigned int handle		= lua_tointeger( pLuaState, 1 );
	float x					= static_cast<float>( lua_tonumber( pLuaState, 2 ) );
	float y					= static_cast<float>( lua_tonumber( pLuaState, 3 ) );
	float z					= static_cast<float>( lua_tonumber( pLuaState, 4 ) );
	
	Actor* pActor = (Actor*)ScriptManager::Get()->GetObjectPointer( handle );

	if ( pActor )
	{
		pActor->GetNode()->Transform.Position() = Vector3f( x, y, z );
		return( 0 );
	}

	return( 0 );
}
//--------------------------------------------------------------------------------
int ScriptIntfActor::GetPosition( lua_State* pLuaState )
{
	int iNumArgs			= lua_gettop( pLuaState );

	unsigned int handle		= lua_tointeger( pLuaState, -1 );
	
	Actor* pActor = (Actor*)ScriptManager::Get()->GetObjectPointer( handle );

	if ( pActor )
	{
		lua_pushnumber( pLuaState, pActor->GetNode()->Transform.Position().x );
		lua_pushnumber( pLuaState, pActor->GetNode()->Transform.Position().y );
		lua_pushnumber( pLuaState, pActor->GetNode()->Transform.Position().z );
	
		return( 3 );
	}

	// TODO: Add error logging here to indicate actor wasn't found

	return( 0 );
}
//--------------------------------------------------------------------------------
int ScriptIntfActor::SetRotation( lua_State* pLuaState )
{
	int iNumArgs				= lua_gettop( pLuaState );

	unsigned int handle			= lua_tointeger( pLuaState, 1 );
	
	float x						= static_cast<float>( lua_tonumber( pLuaState, 2 ) );
	float y						= static_cast<float>( lua_tonumber( pLuaState, 3 ) );
	float z						= static_cast<float>( lua_tonumber( pLuaState, 4 ) );
	
	Actor* pActor = (Actor*)ScriptManager::Get()->GetObjectPointer( handle );

	if ( pActor )
	{
		pActor->GetNode()->Transform.Rotation().Rotation( Vector3f( x, y, z ) );
		return( 0 );
	}

	// TODO: Add error logging here to indicate actor wasn't found

	return( 0 );
}
//--------------------------------------------------------------------------------
int ScriptIntfActor::AttachChild( lua_State* pLuaState )
{
	int iNumArgs				= lua_gettop( pLuaState );

	unsigned int handle1		= lua_tointeger( pLuaState, 1 );
	unsigned int handle2		= lua_tointeger( pLuaState, 2 );
	
	Actor* pParent = (Actor*)ScriptManager::Get()->GetObjectPointer( handle1 );
	Actor* pChild = (Actor*)ScriptManager::Get()->GetObjectPointer( handle2 );

	if ( pParent != NULL && pChild != NULL )
	{
		pChild->GetNode()->DetachParent();
		pParent->GetNode()->AttachChild( pChild->GetNode() );
		return( 0 );
	}

	// TODO: Add error logging here to indicate actor wasn't found

	return( 0 );
}
//--------------------------------------------------------------------------------
int ScriptIntfActor::DetachChild( lua_State* pLuaState )
{
	int iNumArgs				= lua_gettop( pLuaState );

	unsigned int handle1		= lua_tointeger( pLuaState, 1 );
	unsigned int handle2		= lua_tointeger( pLuaState, 2 );
	
	Actor* pParent = (Actor*)ScriptManager::Get()->GetObjectPointer( handle1 );
	Actor* pChild = (Actor*)ScriptManager::Get()->GetObjectPointer( handle2 );

	if ( pParent != NULL && pChild != NULL )
	{
		pParent->GetNode()->DetachChild( pChild->GetNode() );
		return( 0 );
	}

	// TODO: Add error logging here to indicate actor wasn't found

	return( 0 );
}
//--------------------------------------------------------------------------------
int ScriptIntfActor::SetEntityVectorParameter( lua_State* pLuaState )
{
	int iNumArgs				= lua_gettop( pLuaState );

	unsigned int handle			= lua_tointeger( pLuaState, 1 );
	
	std::wstring name			= GlyphString::ToUnicode( std::string( lua_tostring( pLuaState, 2 ) ) );
	float x						= static_cast<float>( lua_tonumber( pLuaState, 3 ) );
	float y						= static_cast<float>( lua_tonumber( pLuaState, 4 ) );
	float z						= static_cast<float>( lua_tonumber( pLuaState, 5 ) );
	float w						= static_cast<float>( lua_tonumber( pLuaState, 6 ) );
	
	Actor* pActor = (Actor*)ScriptManager::Get()->GetObjectPointer( handle );

	if ( pActor )
	{
		VectorParameterDX11 Vector;
		Vector.SetName( name );
		Vector.SetVector( Vector4f( x,y,z,w ) );
		// TODO: Add this ability back into the system!
		//pActor->GetBody()->UpdateRenderParameter( &Vector );
		return( 0 );
	}

	// TODO: Add error logging here to indicate actor wasn't found

	return( 0 );
}
//--------------------------------------------------------------------------------
int ScriptIntfActor::SetMaterialVectorParameter( lua_State* pLuaState )
{
	int iNumArgs				= lua_gettop( pLuaState );

	unsigned int handle			= lua_tointeger( pLuaState, 1 );
	
	std::wstring name			= GlyphString::ToUnicode( std::string( lua_tostring( pLuaState, 2 ) ) );
	float x						= static_cast<float>( lua_tonumber( pLuaState, 3 ) );
	float y						= static_cast<float>( lua_tonumber( pLuaState, 4 ) );
	float z						= static_cast<float>( lua_tonumber( pLuaState, 5 ) );
	float w						= static_cast<float>( lua_tonumber( pLuaState, 6 ) );
	
	Actor* pActor = (Actor*)ScriptManager::Get()->GetObjectPointer( handle );

	if ( pActor )
	{

		VectorParameterDX11 Vector;
		Vector.SetName( name );
		Vector.SetVector( Vector4f( x,y,z,w ) );
		// TODO: Add this ability back into the system...
		//pActor->GetBody()->GetMaterial()->UpdateRenderParameter( &Vector );
		return( 0 );
	}

	// TODO: Add error logging here to indicate actor wasn't found

	return( 0 );
}
//--------------------------------------------------------------------------------
void ScriptIntfActor::InitializeInterface()
{
	ScriptManager::Get()->RegisterEngineClass( "Actor" );
	ScriptManager::Get()->RegisterClassFunction( "Actor", "_SetPosition", ScriptIntfActor::SetPosition );
	ScriptManager::Get()->RegisterClassFunction( "Actor", "_GetPosition", ScriptIntfActor::GetPosition );
	ScriptManager::Get()->RegisterClassFunction( "Actor", "_SetRotation", ScriptIntfActor::SetRotation );
	ScriptManager::Get()->RegisterClassFunction( "Actor", "_AttachChild", ScriptIntfActor::AttachChild );
	ScriptManager::Get()->RegisterClassFunction( "Actor", "_DetachChild", ScriptIntfActor::DetachChild );
	ScriptManager::Get()->RegisterClassFunction( "Actor", "_SetEntityVectorParameter", ScriptIntfActor::SetEntityVectorParameter );
	ScriptManager::Get()->RegisterClassFunction( "Actor", "_SetMaterialVectorParameter", ScriptIntfActor::SetMaterialVectorParameter );
}
//--------------------------------------------------------------------------------