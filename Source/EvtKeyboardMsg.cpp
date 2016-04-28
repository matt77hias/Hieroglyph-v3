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
#include "EvtKeyboardMsg.h"
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
EvtKeyboardMsg::EvtKeyboardMsg( HWND hwnd, unsigned int wparam, long lparam ) 
	: EvtWindowMsg( hwnd, wparam, lparam )
{
}
//--------------------------------------------------------------------------------
EvtKeyboardMsg::~EvtKeyboardMsg( )
{
}
//--------------------------------------------------------------------------------
unsigned int EvtKeyboardMsg::GetCharacterCode()
{
	return( m_wparam );
}
//--------------------------------------------------------------------------------
unsigned int EvtKeyboardMsg::GetRepeatCount()
{
	return( 0xffff & m_lparam );
}
//--------------------------------------------------------------------------------
unsigned int EvtKeyboardMsg::GetScanCode()
{
	return( ( 0xf0000 & m_lparam ) >> 16 );
}
//--------------------------------------------------------------------------------
bool EvtKeyboardMsg::GetExtendedKey()
{
	return( ( 0x01000000 & m_lparam ) > 0 );
}
//--------------------------------------------------------------------------------
bool EvtKeyboardMsg::GetContextCode()
{
	return( ( 0x20000000 & m_lparam ) > 0 );
}
//--------------------------------------------------------------------------------
bool EvtKeyboardMsg::GetPreviousKeyState()
{
	return( ( 0x40000000 & m_lparam ) > 0 );
}
//--------------------------------------------------------------------------------
bool EvtKeyboardMsg::GetTransitionState()
{
	return( ( 0x80000000 & m_lparam ) > 0 );
}
//--------------------------------------------------------------------------------