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
#include "ScriptManager.h"
#include "EventManager.h"
#include "EvtErrorMessage.h"
#include "GlyphString.h"
#include "Log.h"
//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
ScriptManager* ScriptManager::ms_pScriptManager = NULL;
//--------------------------------------------------------------------------------
ScriptManager::ScriptManager()
{
	ms_pScriptManager = this;
	
	m_pLuaState = 0;
	m_pLuaState = lua_open();

	luaL_openlibs( m_pLuaState );

	// Initialize the number of classes currently registered.
	m_iClassIndex = 1;
}
//--------------------------------------------------------------------------------
ScriptManager::~ScriptManager()
{
	lua_close( m_pLuaState );
	m_pLuaState = NULL;
}
//--------------------------------------------------------------------------------
ScriptManager* ScriptManager::Get()
{
	return( ms_pScriptManager );
}
//--------------------------------------------------------------------------------
void ScriptManager::Run( const char* FileName )
{
//	char AsciiFileName[1024];
//	WideCharToMultiByte( CP_ACP, 0, FileName, -1, AsciiFileName, 1024, NULL, NULL);
 
	// TODO: Update this function to reference the script folder specified in the
	//       FileSystem class!!!

	if ( luaL_dofile( m_pLuaState, FileName ) )
	{
		ReportErrors( );
	}
}
//--------------------------------------------------------------------------------
void ScriptManager::ExecuteChunk( char* chunk )
{
	luaL_dostring( m_pLuaState, chunk );
}
//--------------------------------------------------------------------------------
void ScriptManager::RegisterFunction( const char* name, lua_CFunction function )
{
	lua_register( m_pLuaState, name, function );
}
//--------------------------------------------------------------------------------
void ScriptManager::RegisterClassFunction( const char* classname, const char* funcname, lua_CFunction function )
{
	lua_getfield( m_pLuaState, LUA_GLOBALSINDEX, classname );	// Get the class's table
	lua_pushcfunction( m_pLuaState, function );		// Push the function onto the stack
	lua_setfield( m_pLuaState, -2, funcname );		// Store the function in the class
	lua_pop( m_pLuaState, 1 );						// Pop the table off of the stack
}
//--------------------------------------------------------------------------------
lua_State* ScriptManager::GetState( )
{
	return( m_pLuaState );
}
//--------------------------------------------------------------------------------
unsigned int ScriptManager::RegisterEngineClass( const char* name )
{
	sClassData data = m_kClassRegistry[name];

	if ( data.name == "" )
	{
		data.name = name;
		data.id = m_iClassIndex << 16;	// upper 16 bits are for class ID
		data.count = 0;

		m_kClassRegistry[name] = data;	// store the name
		m_iClassIndex++;				// increment the class count

		// Create a new table, and store it at the class name of the global
		// lua environment.
		lua_newtable( m_pLuaState );
		lua_setfield( m_pLuaState, LUA_GLOBALSINDEX, name );
	}

	return( data.id );
}
//--------------------------------------------------------------------------------
unsigned int ScriptManager::RegisterEngineObject( const char* name, void* pObject )
{
	// Test if the object has already been registered
	if ( m_kPointerRegistry[pObject].handle != 0xffffffff ) 
	{
		Log::Get().Write( L"Script Manager: An object that was already registered was re-registered!" );
		return( m_kPointerRegistry[pObject].handle );
	}

	// Ensure that the class has been previously registered.
	if ( m_kClassRegistry[name].name == "" )
		RegisterEngineClass( name );

	// Calculate the handle based on the class ID and the number of objects
	// of that type already registered.  Then increment the number of objects.
	unsigned int handle = m_kClassRegistry[name].ReserveHandle();
	
	// Store the pointer according to the handle, and then store the handle according to the pointer.
	m_kObjectRegistry[handle].pointer = pObject;
	m_kPointerRegistry[pObject].handle = handle;

	return( handle );
}
//--------------------------------------------------------------------------------
bool ScriptManager::UnRegisterObjectByHandle( unsigned int handle )
{
	
	// TODO: Possibly add some marker to the object data structure to indicate
	//       that the object has been unregistered...

	if ( m_kObjectRegistry[handle].pointer )
	{
		m_kPointerRegistry[m_kObjectRegistry[handle].pointer].handle = 0xffffffff;
		m_kObjectRegistry[handle].pointer = 0;
		return( true );
	}

	return( false );
}
//--------------------------------------------------------------------------------
bool ScriptManager::UnRegisterObjectByPointer( void* pObject )
{
	// If the object has been registered, remove its pointer and handle, then 
	// return true, otherwise return false.

	if ( m_kPointerRegistry[pObject].handle )
	{
		m_kObjectRegistry[m_kPointerRegistry[pObject].handle].pointer = 0;
		m_kPointerRegistry[pObject].handle = 0xffffffff;
		return( true );
	}

	return( false );
}
//--------------------------------------------------------------------------------
void* ScriptManager::GetObjectPointer( unsigned int handle )
{
	// Return the pointer.  If anything is incorrect in the handle, a null will
	// be returned.
	return( m_kObjectRegistry[handle].pointer );
}
//--------------------------------------------------------------------------------
unsigned int ScriptManager::GetObjectHandle( void* pObject )
{
	return( m_kPointerRegistry[pObject].handle );
}
//--------------------------------------------------------------------------------
void ScriptManager::ReportErrors( )
{

	std::string errormsg = lua_tostring( m_pLuaState, -1 );
	lua_pop( m_pLuaState, 1 );

	std::wstring werrormsg = GlyphString::ToUnicode( errormsg );

	EventManager::Get()->ProcessEvent( EvtErrorMessagePtr( new EvtErrorMessage( werrormsg ) ) );
}
//--------------------------------------------------------------------------------
