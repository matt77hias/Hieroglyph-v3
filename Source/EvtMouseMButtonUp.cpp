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
#include "EvtMouseMButtonUp.h"
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
EvtMouseMButtonUp::EvtMouseMButtonUp( HWND hwnd, unsigned int wparam, long lparam )
: EvtMouseMsg( hwnd, wparam, lparam )
{
}
//--------------------------------------------------------------------------------
EvtMouseMButtonUp::~EvtMouseMButtonUp( )
{
}
//--------------------------------------------------------------------------------
std::wstring EvtMouseMButtonUp::GetEventName( )
{
	return( std::wstring( L"mbutton_up" ) );
}
//--------------------------------------------------------------------------------
eEVENT EvtMouseMButtonUp::GetEventType( )
{
	return( SYSTEM_MBUTTON_UP );
}
//--------------------------------------------------------------------------------
