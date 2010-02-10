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
// EvtWindowMsg
//
//--------------------------------------------------------------------------------
#ifndef EvtWindowMsg_h
#define EvtWindowMsg_h
//--------------------------------------------------------------------------------
#include "IEvent.h"
#include "windows.h"
//--------------------------------------------------------------------------------
namespace Glyph3
{
	class EvtWindowMsg : public IEvent
	{
	public:
		EvtWindowMsg( HWND hwnd, unsigned int wparam, long lparam );
		virtual ~EvtWindowMsg( );

		HWND GetWindowHandle();
		unsigned int GetWParam();
		unsigned int GetLParam();

	protected:
		HWND			m_hWnd;
		unsigned int	m_wparam;
		unsigned int	m_lparam;
	};

};
//--------------------------------------------------------------------------------
#endif // EvtWindowMsg_h
