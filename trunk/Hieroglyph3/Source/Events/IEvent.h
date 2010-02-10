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
// IEvent
//
// Copyright (C) 2003-2007 Jason Zink
//--------------------------------------------------------------------------------
#ifndef IEvent_h
#define IEvent_h
//--------------------------------------------------------------------------------
#include <string>
//--------------------------------------------------------------------------------
namespace Glyph3
{
	enum eEVENT
	{
		//GUI_CURSOR_OVER,
		//GUI_CURSOR_ENTER,
		//GUI_CURSOR_LEAVE,
		//GUI_CURSOR_BUTTON_DOWN,
		//GUI_CURSOR_BUTTON_PRESSED,
		//GUI_CURSOR_BUTTON_UP,
		//GUI_UI_BUTTON_PRESSED,
		RENDER_FRAME_START,
		SYSTEM_LBUTTON_DOWN,
		SYSTEM_LBUTTON_UP,
		SYSTEM_MBUTTON_DOWN,
		SYSTEM_MBUTTON_UP,
		SYSTEM_RBUTTON_DOWN,
		SYSTEM_RBUTTON_UP,
		SYSTEM_MOUSE_LEAVE,
		SYSTEM_MOUSE_WHEEL,
		SYSTEM_MOUSE_MOVE,
		SYSTEM_KEYBOARD_KEYDOWN,
		SYSTEM_KEYBOARD_KEYUP,
		SYSTEM_KEYBOARD_CHAR,
		LOG_MESSAGE,
		//TEXT_CONTROLLER_STATE_REQUEST,
		//ENTITY3D_CONTROLLER_STATE_REQUEST,
		NUM_EVENTS
	};

	class IEvent
	{
	public:
		virtual std::wstring GetEventName( ) = 0;
		virtual eEVENT GetEventType( ) = 0;
	};
};
//--------------------------------------------------------------------------------
#endif // IEvent_h
