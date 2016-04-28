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
// EvtMouseRButtonUp
//
//--------------------------------------------------------------------------------
#ifndef EvtMouseRButtonUp_h
#define EvtMouseRButtonUp_h
//--------------------------------------------------------------------------------
#include "EvtMouseMsg.h"
//--------------------------------------------------------------------------------
namespace Glyph3
{
	class EvtMouseRButtonUp : public EvtMouseMsg
	{
	public:
		EvtMouseRButtonUp( HWND hwnd, unsigned int wparam, long lparam );
		virtual ~EvtMouseRButtonUp( );

		virtual std::wstring GetEventName( );
		virtual eEVENT GetEventType( );
	};

	typedef std::shared_ptr<EvtMouseRButtonUp> EvtMouseRButtonUpPtr;
};
//--------------------------------------------------------------------------------
#endif // EvtMouseRButtonUp_h
