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
#include "EvtMouseWheel.h"
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
EvtMouseWheel::EvtMouseWheel( HWND hwnd, unsigned int wparam, long lparam )
: EvtMouseMsg( hwnd, wparam, lparam )
{
}
//--------------------------------------------------------------------------------
EvtMouseWheel::~EvtMouseWheel( )
{
}
//--------------------------------------------------------------------------------
std::wstring EvtMouseWheel::GetEventName( )
{
	return( std::wstring( L"mouse_wheel" ) );
}
//--------------------------------------------------------------------------------
eEVENT EvtMouseWheel::GetEventType( )
{
	return( SYSTEM_MOUSE_WHEEL );
}
//--------------------------------------------------------------------------------
int EvtMouseWheel::GetWheelDelta()
{
	return( (short)(( 0xffff0000 & m_wparam ) >> 16 ) );
}
//--------------------------------------------------------------------------------