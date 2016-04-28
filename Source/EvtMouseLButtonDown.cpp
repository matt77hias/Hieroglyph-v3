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
#include "EvtMouseLButtonDown.h"
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
EvtMouseLButtonDown::EvtMouseLButtonDown( HWND hwnd, unsigned int wparam, long lparam )
: EvtMouseMsg( hwnd, wparam, lparam )
{
}
//--------------------------------------------------------------------------------
EvtMouseLButtonDown::~EvtMouseLButtonDown( )
{
}
//--------------------------------------------------------------------------------
std::wstring EvtMouseLButtonDown::GetEventName( )
{
	return( std::wstring( L"lbutton_down" ) );
}
//--------------------------------------------------------------------------------
eEVENT EvtMouseLButtonDown::GetEventType( )
{
	return( SYSTEM_LBUTTON_DOWN );
}
//--------------------------------------------------------------------------------
