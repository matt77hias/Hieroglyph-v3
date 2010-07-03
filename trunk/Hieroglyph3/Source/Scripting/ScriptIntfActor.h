//--------------------------------------------------------------------------------
// ScriptIntfActor
//
// Copyright (C) 2003-2009 Jason Zink
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
