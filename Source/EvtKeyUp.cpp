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
#include "EvtKeyUp.h"
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
EvtKeyUp::EvtKeyUp( HWND hwnd, unsigned int wparam, long lparam )
	: EvtKeyboardMsg( hwnd, wparam, lparam )
{
}
//--------------------------------------------------------------------------------
EvtKeyUp::~EvtKeyUp( )
{
}
//--------------------------------------------------------------------------------
std::wstring EvtKeyUp::GetEventName( )
{
	return( std::wstring( L"keyboard_keyup" ) );
}
//--------------------------------------------------------------------------------
eEVENT EvtKeyUp::GetEventType( )
{
	return( SYSTEM_KEYBOARD_KEYUP );
}
//--------------------------------------------------------------------------------
