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
// EvtMouseRButtonDown
//
//--------------------------------------------------------------------------------
#ifndef EvtMouseRButtonDown_h
#define EvtMouseRButtonDown_h
//--------------------------------------------------------------------------------
#include "EvtMouseMsg.h"
//--------------------------------------------------------------------------------
namespace Glyph3
{
	class EvtMouseRButtonDown : public EvtMouseMsg
	{
	public:
		EvtMouseRButtonDown( HWND hwnd, unsigned int wparam, long lparam );
		virtual ~EvtMouseRButtonDown( );

		virtual std::wstring GetEventName( );
		virtual eEVENT GetEventType( );
	};

	typedef std::shared_ptr<EvtMouseRButtonDown> EvtMouseRButtonDownPtr;
};
//--------------------------------------------------------------------------------
#endif // EvtMouseRButtonDown_h
