//--------------------------------------------------------------------------------
#include "ScriptIntfActor.h"
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
		pActor->GetNode()->Position() = Vector3f( x, y, z );
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
		lua_pushnumber( pLuaState, pActor->GetNode()->Position().x );
		lua_pushnumber( pLuaState, pActor->GetNode()->Position().y );
		lua_pushnumber( pLuaState, pActor->GetNode()->Position().z );
	
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
		pActor->GetNode()->Rotation().Rotation( Vector3f( x, y, z ) );
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
}
//--------------------------------------------------------------------------------