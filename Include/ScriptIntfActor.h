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
// ScriptIntfActor
//
//--------------------------------------------------------------------------------
#ifndef ScriptIntfActor_h
#define ScriptIntfActor_h
//--------------------------------------------------------------------------------
#include "ScriptManager.h"
#include "IScriptInterface.h"
#include "Actor.h"
//--------------------------------------------------------------------------------
namespace Glyph3
{
	class ScriptIntfActor : public IScriptInterface
	{
	public:
		ScriptIntfActor();
		virtual ~ScriptIntfActor();

		virtual void InitializeInterface();

		static int GetPosition( lua_State* pLuaState );
		static int SetPosition( lua_State* pLuaState );
		static int GetRotation( lua_State* pLuaState );
		static int SetRotation( lua_State* pLuaState );

		static int AttachChild( lua_State* pLuaState );
		static int DetachChild( lua_State* pLuaState );

		static int SetEntityVectorParameter( lua_State* pLuaState );
		static int SetMaterialVectorParameter( lua_State* pLuaState );
	};
};
//--------------------------------------------------------------------------------
#endif // ScriptIntfActor_h
