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
// EvtMouseMButtonUp
//
//--------------------------------------------------------------------------------
#ifndef EvtMouseMButtonUp_h
#define EvtMouseMButtonUp_h
//--------------------------------------------------------------------------------
#include "EvtMouseMsg.h"
//--------------------------------------------------------------------------------
namespace Glyph3
{
	class EvtMouseMButtonUp : public EvtMouseMsg
	{
	public:
		EvtMouseMButtonUp( HWND hwnd, unsigned int wparam, long lparam );
		virtual ~EvtMouseMButtonUp( );

		virtual std::wstring GetEventName( );
		virtual eEVENT GetEventType( );
	};

};
//--------------------------------------------------------------------------------
#endif // EvtMouseMButtonUp_h
