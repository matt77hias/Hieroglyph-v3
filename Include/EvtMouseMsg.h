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
// EvtMouseMsg
//
//--------------------------------------------------------------------------------
#ifndef EvtMouseMsg_h
#define EvtMouseMsg_h
//--------------------------------------------------------------------------------
#include "EvtWindowMsg.h"
//--------------------------------------------------------------------------------
namespace Glyph3
{
	class EvtMouseMsg : public EvtWindowMsg
	{
	public:
		EvtMouseMsg( HWND hwnd, unsigned int wparam, long lparam );
		virtual ~EvtMouseMsg( );

		bool ControlKeyDown();
		bool LButtonDown();
		bool MButtonDown();
		bool RButtonDown();
		bool ShiftKeyDown();

		int GetX();
		int GetY();
	};

	typedef std::shared_ptr<EvtMouseMsg> EvtMouseMsgPtr;
};
//--------------------------------------------------------------------------------
#endif // EvtMouseMsg_h
