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
// ConsoleActor
//
//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
#ifndef ConsoleActor_h
#define ConsoleActor_h
//--------------------------------------------------------------------------------
#include "Actor.h"
#include "Console.h"
#include "IEventListener.h"
#include "TextActor.h"
//--------------------------------------------------------------------------------
namespace Glyph3
{    
	class ConsoleActor : public Actor, public IEventListener
    {
    public:
        ConsoleActor();
		virtual ~ConsoleActor();
        
        virtual bool HandleEvent( EventPtr pEvent );
        virtual std::wstring GetName();

		Console console;
		TextActor textActor;

		static const size_t MAX_ENTRY_DISPLAY = 10;

	private:
		void printText();
    };
};
//--------------------------------------------------------------------------------
#endif // ConsoleActor_h
//--------------------------------------------------------------------------------