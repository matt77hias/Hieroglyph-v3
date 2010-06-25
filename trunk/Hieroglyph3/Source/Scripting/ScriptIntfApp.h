//--------------------------------------------------------------------------------
// ScriptIntfApp
//
// Copyright (C) 2003-2009 Jason Zink
//--------------------------------------------------------------------------------
#ifndef ScriptIntfApp_h
#define ScriptIntfApp_h
//--------------------------------------------------------------------------------
#include "ScriptManager.h"
#include "IScriptInterface.h"
#include "Application.h"
//--------------------------------------------------------------------------------
namespace Glyph3
{
	class ScriptIntfApp : public IScriptInterface
	{
	public:
		ScriptIntfApp();
		virtual ~ScriptIntfApp();

		virtual void InitializeInterface();

		// For C++ to call Lua
		static void Initialize();
		static void Update( float time );
		static void Render();
		static void Shutdown();
		static void OnKeyDown( unsigned int key );

		// For Lua to call C++
		static int Log( lua_State* pLuaState );
		static int Framerate( lua_State* pLuaState );
		static int CreateActor( lua_State* pLuaState );
	};
};
//--------------------------------------------------------------------------------
#endif // ScriptIntfApp_h

