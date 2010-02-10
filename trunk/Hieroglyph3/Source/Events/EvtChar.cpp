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
#include "EvtChar.h"
#include "windows.h"
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
EvtChar::EvtChar( HWND hwnd, unsigned int wparam, long lparam )
	: EvtKeyboardMsg( hwnd, wparam, lparam )
{
}
//--------------------------------------------------------------------------------
EvtChar::~EvtChar( )
{
}
//--------------------------------------------------------------------------------
std::wstring EvtChar::GetEventName( )
{
	return( std::wstring( L"keyboard_char" ) );
}
//--------------------------------------------------------------------------------
eEVENT EvtChar::GetEventType( )
{
	return( SYSTEM_KEYBOARD_CHAR );
}
//--------------------------------------------------------------------------------
