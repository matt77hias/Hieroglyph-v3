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
// EvtMouseMButtonDown
//
//--------------------------------------------------------------------------------
#ifndef EvtMouseMButtonDown_h
#define EvtMouseMButtonDown_h
//--------------------------------------------------------------------------------
#include "EvtMouseMsg.h"
//--------------------------------------------------------------------------------
namespace Glyph3
{
	class EvtMouseMButtonDown : public EvtMouseMsg
	{
	public:
		EvtMouseMButtonDown( HWND hwnd, unsigned int wparam, long lparam );
		virtual ~EvtMouseMButtonDown( );

		virtual std::wstring GetEventName( );
		virtual eEVENT GetEventType( );
	};

	typedef std::shared_ptr<EvtMouseMButtonDown> EvtMouseMButtonDownPtr;
};
//--------------------------------------------------------------------------------
#endif // EvtMouseMButtonDown_h
