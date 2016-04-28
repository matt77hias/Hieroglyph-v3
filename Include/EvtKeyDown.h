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
// EvtKeyDown
//
//--------------------------------------------------------------------------------
#ifndef EvtKeyDown_h
#define EvtKeyDown_h
//--------------------------------------------------------------------------------
#include "EvtKeyboardMsg.h"
//--------------------------------------------------------------------------------
namespace Glyph3
{
	class EvtKeyDown : public EvtKeyboardMsg
	{
	public:
		EvtKeyDown( HWND hwnd, unsigned int wparam, long lparam );
		virtual ~EvtKeyDown( );

		virtual std::wstring GetEventName( );
		virtual eEVENT GetEventType( );
	};

	typedef std::shared_ptr<EvtKeyDown> EvtKeyDownPtr;
};
//--------------------------------------------------------------------------------
#endif // EvtKeyDown_h
