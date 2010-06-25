//--------------------------------------------------------------------------------
// This file is a portion of the Hieroglyph 3 Rendering Engine.  It is distributed
// under the MIT License, available in the root of this distribution and 
// at the following URL:
//
// http://www.opensource.org/licenses/mit-license.php
//
// Copyright (c) 2003-2010 Jason Zink 
//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
#include "ScriptManager.h"
#include "Log.h"
#include <windows.h>
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
ScriptManager* ScriptManager::ms_pScriptManager = NULL;
//ScriptManager ScriptMgr;
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
void ScriptManager::Run( char* FileName )
{
//	char AsciiFileName[1024];
//	WideCharToMultiByte( CP_ACP, 0, FileName, -1, AsciiFileName, 1024, NULL, NULL);
 
	luaL_dofile( m_pLuaState, FileName );
}
//--------------------------------------------------------------------------------
void ScriptManager::ExecuteChunk( char* chunk )
{
//	char AsciiChunk[1024];
//	WideCharToMultiByte( CP_ACP, 0, chunk, -1, AsciiChunk, 1024, NULL, NULL);

	luaL_dostring( m_pLuaState, chunk );
}
//--------------------------------------------------------------------------------
void ScriptManager::RegisterFunction( const char* name, lua_CFunction function )
{
//	char AsciiName[1024];
//	WideCharToMultiByte( CP_ACP, 0, name, -1, AsciiName, 1024, NULL, NULL);

	lua_register( m_pLuaState, name, function );
}
//--------------------------------------------------------------------------------
void ScriptManager::RegisterClassFunction( const char* classname, const char* funcname, lua_CFunction function )
{
//	char AsciiClassName[1024];
//	char AsciiFuncName[1024];
//	WideCharToMultiByte( CP_ACP, 0, classname, -1, AsciiClassName, 1024, NULL, NULL);
//	WideCharToMultiByte( CP_ACP, 0, funcname, -1, AsciiFuncName, 1024, NULL, NULL);

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
//	char AsciiName[1024];
//	WideCharToMultiByte( CP_ACP, 0, name, -1, AsciiName, 1024, NULL, NULL);

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
//void ScriptManager::RegisterClassMethod( int ClassID, const char* name, lua_CFunction function )
//{
//
//}
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

	// If the object has been registered, remove its pointer and handle, then 
	// return true, otherwise return false.

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
//std::wstring& ScriptManager::GetClass( int ClassID )
//{
//
//}
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


//iStatus = lua_pcall( lState, 0, LUA_MULTRET, 0 );
//if( iStatus )
//{
//    std::cout << "Error: " << lua_tostring( lState, -1 );
//    return 1;
//}