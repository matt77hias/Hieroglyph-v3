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
// EvtChar
//
//--------------------------------------------------------------------------------
#ifndef EvtChar_h
#define EvtChar_h
//--------------------------------------------------------------------------------
#include "EvtKeyboardMsg.h"
//--------------------------------------------------------------------------------
namespace Glyph3
{
	class EvtChar : public EvtKeyboardMsg
	{
	public:
		EvtChar( HWND hwnd, unsigned int wparam, long lparam );
		virtual ~EvtChar( );

		virtual std::wstring GetEventName( );
		virtual eEVENT GetEventType( );
	};

	typedef std::shared_ptr<EvtChar> EvtCharPtr;
};
//--------------------------------------------------------------------------------
#endif // EvtChar_h
