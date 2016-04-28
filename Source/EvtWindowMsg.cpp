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
#include "EvtWindowMsg.h"
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
EvtWindowMsg::EvtWindowMsg( HWND hwnd, unsigned int wparam, long lparam )
{
	m_hWnd = hwnd;
	m_wparam = wparam;
	m_lparam = lparam;
}
//--------------------------------------------------------------------------------
EvtWindowMsg::~EvtWindowMsg( )
{
}
//--------------------------------------------------------------------------------
HWND EvtWindowMsg::GetWindowHandle()
{
	return( m_hWnd );
}
//--------------------------------------------------------------------------------
unsigned int EvtWindowMsg::GetWParam()
{
	return( m_wparam );
}
//--------------------------------------------------------------------------------
unsigned int EvtWindowMsg::GetLParam()
{
	return( m_lparam );
}
//--------------------------------------------------------------------------------