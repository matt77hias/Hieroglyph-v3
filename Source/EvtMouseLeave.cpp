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
#include "EvtMouseLeave.h"
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
EvtMouseLeave::EvtMouseLeave(  HWND hwnd, unsigned int wparam, long lparam  )
: EvtMouseMsg( hwnd, wparam, lparam )
{
}
//--------------------------------------------------------------------------------
EvtMouseLeave::~EvtMouseLeave( )
{
}
//--------------------------------------------------------------------------------
std::wstring EvtMouseLeave::GetEventName( )
{
	return( std::wstring( L"mouse_leave" ) );
}
//--------------------------------------------------------------------------------
eEVENT EvtMouseLeave::GetEventType( )
{
	return( SYSTEM_MOUSE_LEAVE );
}
//--------------------------------------------------------------------------------
