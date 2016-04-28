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
// EvtMouseMove
//
//--------------------------------------------------------------------------------
#ifndef EvtMouseMove_h
#define EvtMouseMove_h
//--------------------------------------------------------------------------------
#include "EvtMouseMsg.h"
//--------------------------------------------------------------------------------
namespace Glyph3
{
	class EvtMouseMove : public EvtMouseMsg
	{
	public:
		EvtMouseMove( HWND hwnd, unsigned int wparam, long lparam );
		virtual ~EvtMouseMove( );

		virtual std::wstring GetEventName( );
		virtual eEVENT GetEventType( );
	};

	typedef std::shared_ptr<EvtMouseMove> EvtMouseMovePtr;
};
//--------------------------------------------------------------------------------
#endif // EvtMouseMove_h
