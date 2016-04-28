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
#include "EvtMouseMove.h"
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
EvtMouseMove::EvtMouseMove( HWND hwnd, unsigned int wparam, long lparam )
: EvtMouseMsg( hwnd, wparam, lparam )
{
}
//--------------------------------------------------------------------------------
EvtMouseMove::~EvtMouseMove( )
{
}
//--------------------------------------------------------------------------------
std::wstring EvtMouseMove::GetEventName( )
{
	return( std::wstring( L"mouse_move" ) );
}
//--------------------------------------------------------------------------------
eEVENT EvtMouseMove::GetEventType( )
{
	return( SYSTEM_MOUSE_MOVE );
}
//--------------------------------------------------------------------------------
