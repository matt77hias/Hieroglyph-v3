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
#include "EvtKeyDown.h"
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
EvtKeyDown::EvtKeyDown( HWND hwnd, unsigned int wparam, long lparam )
	: EvtKeyboardMsg( hwnd, wparam, lparam )
{
}
//--------------------------------------------------------------------------------
EvtKeyDown::~EvtKeyDown( )
{
}
//--------------------------------------------------------------------------------
std::wstring EvtKeyDown::GetEventName( )
{
	return( std::wstring( L"keyboard_keydown" ) );
}
//--------------------------------------------------------------------------------
eEVENT EvtKeyDown::GetEventType( )
{
	return( SYSTEM_KEYBOARD_KEYDOWN );
}
//--------------------------------------------------------------------------------