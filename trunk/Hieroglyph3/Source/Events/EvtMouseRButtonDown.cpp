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
#include "EvtMouseRButtonDown.h"
#include "windows.h"
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
EvtMouseRButtonDown::EvtMouseRButtonDown( HWND hwnd, unsigned int wparam, long lparam )
: EvtMouseMsg( hwnd, wparam, lparam )
{
}
//--------------------------------------------------------------------------------
EvtMouseRButtonDown::~EvtMouseRButtonDown( )
{
}
//--------------------------------------------------------------------------------
std::wstring EvtMouseRButtonDown::GetEventName( )
{
	return( std::wstring( L"rbutton_down" ) );
}
//--------------------------------------------------------------------------------
eEVENT EvtMouseRButtonDown::GetEventType( )
{
	return( SYSTEM_RBUTTON_DOWN );
}
//--------------------------------------------------------------------------------
