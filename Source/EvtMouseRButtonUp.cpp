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
#include "EvtMouseRButtonUp.h"
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
EvtMouseRButtonUp::EvtMouseRButtonUp( HWND hwnd, unsigned int wparam, long lparam )
: EvtMouseMsg( hwnd, wparam, lparam )
{
}
//--------------------------------------------------------------------------------
EvtMouseRButtonUp::~EvtMouseRButtonUp( )
{
}
//--------------------------------------------------------------------------------
std::wstring EvtMouseRButtonUp::GetEventName( )
{
	return( std::wstring( L"rbutton_up" ) );
}
//--------------------------------------------------------------------------------
eEVENT EvtMouseRButtonUp::GetEventType( )
{
	return( SYSTEM_RBUTTON_UP );
}
//--------------------------------------------------------------------------------
