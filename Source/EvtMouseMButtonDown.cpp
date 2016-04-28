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
#include "EvtMouseMButtonDown.h"
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
EvtMouseMButtonDown::EvtMouseMButtonDown( HWND hwnd, unsigned int wparam, long lparam )
: EvtMouseMsg( hwnd, wparam, lparam )
{
}
//--------------------------------------------------------------------------------
EvtMouseMButtonDown::~EvtMouseMButtonDown( )
{
}
//--------------------------------------------------------------------------------
std::wstring EvtMouseMButtonDown::GetEventName( )
{
	return( std::wstring( L"mbutton_down" ) );
}
//--------------------------------------------------------------------------------
eEVENT EvtMouseMButtonDown::GetEventType( )
{
	return( SYSTEM_MBUTTON_DOWN );
}
//--------------------------------------------------------------------------------
