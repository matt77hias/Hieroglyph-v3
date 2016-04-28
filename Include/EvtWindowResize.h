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
// EvtWindowResize
//
//--------------------------------------------------------------------------------
#ifndef EvtWindowResize_h
#define EvtWindowResize_h
//--------------------------------------------------------------------------------
#include "EvtWindowMsg.h"
//--------------------------------------------------------------------------------
namespace Glyph3
{
	class EvtWindowResize : public EvtWindowMsg
	{
	public:
		EvtWindowResize( HWND hwnd, unsigned int wparam, long lparam );
		virtual ~EvtWindowResize( );

		virtual std::wstring GetEventName( );
		virtual eEVENT GetEventType( );

		int NewWidth();
		int NewHeight();
		bool IsMaxHide();	// Message is sent to all pop-up windows when some other window is maximized.
		bool IsMaximized();	// The window has been maximized.
		bool IsMaxShow();	// Message is sent to all pop-up windows when some other window has been restored to its former size.
		bool IsMinimized(); // The window has been minimized.
		bool IsRestored();	// The window has been resized, but neither the SIZE_MINIMIZED nor SIZE_MAXIMIZED value applies.
	};

	typedef std::shared_ptr<EvtWindowResize> EvtWindowResizePtr;
};
//--------------------------------------------------------------------------------
#endif // EvtWindowResize_h
