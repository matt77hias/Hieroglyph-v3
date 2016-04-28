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
// EvtKeyUp
//
//--------------------------------------------------------------------------------
#ifndef EvtKeyUp_h
#define EvtKeyUp_h
//--------------------------------------------------------------------------------
#include "EvtKeyboardMsg.h"
//--------------------------------------------------------------------------------
namespace Glyph3
{
	class EvtKeyUp : public EvtKeyboardMsg
	{
	public:
		EvtKeyUp( HWND hwnd, unsigned int wparam, long lparam );
		virtual ~EvtKeyUp( );

		virtual std::wstring GetEventName( );
		virtual eEVENT GetEventType( );
	};

	typedef std::shared_ptr<EvtKeyUp> EvtKeyUpPtr;
};
//--------------------------------------------------------------------------------
#endif // EvtKeyUp_h
