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
// Console
//
//--------------------------------------------------------------------------------
#ifndef Console_h
#define Console_h
//--------------------------------------------------------------------------------
#include "PCH.h"
#include "ScriptManager.h"
//--------------------------------------------------------------------------------
namespace Glyph3
{
	class Console
	{
	public:
		Console();
		~Console();

		void processKey( unsigned int code );

		// TODO: Add method here for loading libraries with std::function
		std::string line;
		std::vector<std::string> history;

		lua_State* getState();

	private:
		void execute();
		void reportErrors();


		lua_State* L;
	};
};
//--------------------------------------------------------------------------------
#endif // Console_h
//--------------------------------------------------------------------------------

