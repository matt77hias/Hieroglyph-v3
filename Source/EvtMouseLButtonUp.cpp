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
#include "EvtMouseLButtonUp.h"
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
EvtMouseLButtonUp::EvtMouseLButtonUp( HWND hwnd, unsigned int wparam, long lparam )
: EvtMouseMsg( hwnd, wparam, lparam )
{
}
//--------------------------------------------------------------------------------
EvtMouseLButtonUp::~EvtMouseLButtonUp( )
{
}
//--------------------------------------------------------------------------------
std::wstring EvtMouseLButtonUp::GetEventName( )
{
	return( std::wstring( L"lbutton_up" ) );
}
//--------------------------------------------------------------------------------
eEVENT EvtMouseLButtonUp::GetEventType( )
{
	return( SYSTEM_LBUTTON_UP );
}
//--------------------------------------------------------------------------------
