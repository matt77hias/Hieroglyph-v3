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
// ScriptIntfApp
//
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

