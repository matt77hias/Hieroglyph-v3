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
// EvtMouseWheel
//
//--------------------------------------------------------------------------------
#ifndef EvtMouseWheel_h
#define EvtMouseWheel_h
//--------------------------------------------------------------------------------
#include "EvtMouseMsg.h"
//--------------------------------------------------------------------------------
namespace Glyph3
{
	class EvtMouseWheel : public EvtMouseMsg
	{
	public:
		EvtMouseWheel( HWND hwnd, unsigned int wparam, long lparam );
		virtual ~EvtMouseWheel( );

		virtual std::wstring GetEventName( );
		virtual eEVENT GetEventType( );

		int GetWheelDelta();
	};

};
//--------------------------------------------------------------------------------
#endif // EvtMouseWheel_h
