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
// EvtKeyboardMsg
//
//--------------------------------------------------------------------------------
#ifndef EvtKeyboardMsg_h
#define EvtKeyboardMsg_h
//--------------------------------------------------------------------------------
#include "EvtWindowMsg.h"
//--------------------------------------------------------------------------------
namespace Glyph3
{
	class EvtKeyboardMsg : public EvtWindowMsg
	{
	public:
		EvtKeyboardMsg( HWND hwnd, unsigned int wparam, long lparam );
		virtual ~EvtKeyboardMsg( );

		unsigned int GetCharacterCode();
		unsigned int GetRepeatCount();
		unsigned int GetScanCode();
		bool GetExtendedKey();
		bool GetContextCode();
		bool GetPreviousKeyState();
		bool GetTransitionState();
	};

	typedef std::shared_ptr<EvtKeyboardMsg> EvtKeyboardMsgPtr;
};
//--------------------------------------------------------------------------------
#endif // EvtKeyboardMsg_h
