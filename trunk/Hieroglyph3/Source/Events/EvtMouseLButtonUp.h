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
// EvtMouseLButtonUp
//
//--------------------------------------------------------------------------------
#ifndef EvtMouseLButtonUp_h
#define EvtMouseLButtonUp_h
//--------------------------------------------------------------------------------
#include "EvtMouseMsg.h"
//--------------------------------------------------------------------------------
namespace Glyph3
{
	class EvtMouseLButtonUp : public EvtMouseMsg
	{
	public:
		EvtMouseLButtonUp( HWND hwnd, unsigned int wparam, long lparam );
		virtual ~EvtMouseLButtonUp( );

		virtual std::wstring GetEventName( );
		virtual eEVENT GetEventType( );
	};

};
//--------------------------------------------------------------------------------
#endif // EvtMouseLButtonUp_h
