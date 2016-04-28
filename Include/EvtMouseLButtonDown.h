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
// EvtMouseLButtonDown
//
//--------------------------------------------------------------------------------
#ifndef EvtMouseLButtonDown_h
#define EvtMouseLButtonDown_h
//--------------------------------------------------------------------------------
#include "EvtMouseMsg.h"
//--------------------------------------------------------------------------------
namespace Glyph3
{
	class EvtMouseLButtonDown : public EvtMouseMsg
	{
	public:
		EvtMouseLButtonDown( HWND hwnd, unsigned int wparam, long lparam );
		virtual ~EvtMouseLButtonDown( );

		virtual std::wstring GetEventName( );
		virtual eEVENT GetEventType( );
	};

	typedef std::shared_ptr<EvtMouseLButtonDown> EvtMouseLButtonDownPtr;
};
//--------------------------------------------------------------------------------
#endif // EvtMouseLButtonDown_h
