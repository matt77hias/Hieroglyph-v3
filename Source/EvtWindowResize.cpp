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
#include "PCH.h"
#include "EvtWindowResize.h"
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
EvtWindowResize::EvtWindowResize( HWND hwnd, unsigned int wparam, long lparam )
: EvtWindowMsg( hwnd, wparam, lparam )
{
}
//--------------------------------------------------------------------------------
EvtWindowResize::~EvtWindowResize( )
{
}
//--------------------------------------------------------------------------------
std::wstring EvtWindowResize::GetEventName( )
{
	return( std::wstring( L"window_resize" ) );
}
//--------------------------------------------------------------------------------
eEVENT EvtWindowResize::GetEventType( )
{
	return( WINDOW_RESIZE );
}
//--------------------------------------------------------------------------------
int EvtWindowResize::NewWidth()
{
	return( m_lparam & 0xFFFF );
}
//--------------------------------------------------------------------------------
int EvtWindowResize::NewHeight()
{
	return( ( m_lparam & 0xFFFF0000 ) >> 16 );
}
//--------------------------------------------------------------------------------
bool EvtWindowResize::IsMaxHide()	// Message is sent to all pop-up windows when some other window is maximized.
{
	if ( m_wparam == SIZE_MAXHIDE )
		return( true );
	else
		return( false );
}
//--------------------------------------------------------------------------------
bool EvtWindowResize::IsMaximized()	// The window has been maximized.
{
	if ( m_wparam == SIZE_MAXIMIZED )
		return( true );
	else
		return( false );
}
//--------------------------------------------------------------------------------
bool EvtWindowResize::IsMaxShow()	// Message is sent to all pop-up windows when some other window has been restored to its former size.
{
	if ( m_wparam == SIZE_MAXSHOW )
		return( true );
	else
		return( false );
}
//--------------------------------------------------------------------------------
bool EvtWindowResize::IsMinimized() // The window has been minimized.
{
	if ( m_wparam == SIZE_MINIMIZED )
		return( true );
	else
		return( false );
}
//--------------------------------------------------------------------------------
bool EvtWindowResize::IsRestored()	// The window has been resized, but neither the SIZE_MINIMIZED nor SIZE_MAXIMIZED value applies.
{
	if ( m_wparam == SIZE_RESTORED )
		return( true );
	else
		return( false );
}
//--------------------------------------------------------------------------------