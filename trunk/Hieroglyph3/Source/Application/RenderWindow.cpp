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
#include "RenderWindow.h"
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
RenderWindow::RenderWindow( )
{
	m_hWnd = 0;
	m_sCaption = L"";
	m_iWidth = 640;
	m_iHeight = 480;
	m_iLeft = 100;
	m_iTop = 100;

	m_iSwapChain = -1;
}
//--------------------------------------------------------------------------------
RenderWindow::~RenderWindow()
{
}
//--------------------------------------------------------------------------------
HWND RenderWindow::GetHandle()
{
	return( m_hWnd );
}
//--------------------------------------------------------------------------------
void RenderWindow::SetWidth( int width )
{
	m_iWidth = width;

	if ( m_hWnd != 0 )
		MoveWindow( m_hWnd, m_iLeft, m_iTop, m_iWidth, m_iHeight, true );
}
//--------------------------------------------------------------------------------
void RenderWindow::SetHeight( int height )
{
	m_iHeight = height;

	if ( m_hWnd != 0 )
		MoveWindow( m_hWnd, m_iLeft, m_iTop, m_iWidth, m_iHeight, true );
}
//--------------------------------------------------------------------------------
int RenderWindow::GetWidth()
{
	RECT rect;
	GetClientRect( m_hWnd, &rect );
	
	return( rect.right - rect.left );
}
//--------------------------------------------------------------------------------
int RenderWindow::GetHeight()
{
	RECT rect;
	GetClientRect( m_hWnd, &rect );

	return( rect.bottom - rect.top );
}
//--------------------------------------------------------------------------------
int RenderWindow::GetLeft()
{
	RECT rect;
	GetClientRect( m_hWnd, &rect );

	return( rect.left );
}
//--------------------------------------------------------------------------------
int RenderWindow::GetTop()
{
	RECT rect;
	GetClientRect( m_hWnd, &rect );

	return( rect.top );
}
//--------------------------------------------------------------------------------
void RenderWindow::SetSize( int width, int height )
{
	m_iWidth = width;
	m_iHeight = height;

	if ( m_hWnd != 0 )
		MoveWindow( m_hWnd, m_iLeft, m_iTop, m_iWidth, m_iHeight, true );
}
//--------------------------------------------------------------------------------
void RenderWindow::SetPosition( int left, int top )
{
	m_iLeft = left;
	m_iTop = top;

	if ( m_hWnd != 0 )
		MoveWindow( m_hWnd, m_iLeft, m_iTop, m_iWidth, m_iHeight, true );
}
//--------------------------------------------------------------------------------
void RenderWindow::SetCaption( std::wstring& caption )
{
	m_sCaption = caption;

	if ( m_hWnd != 0 )
		SetWindowText( m_hWnd, m_sCaption.c_str() );
}
//--------------------------------------------------------------------------------
std::wstring RenderWindow::GetCaption()
{
	return( m_sCaption );
}
//--------------------------------------------------------------------------------
int RenderWindow::GetSwapChain()
{
	return( m_iSwapChain );
}
//--------------------------------------------------------------------------------
void RenderWindow::SetSwapChain( int swapchain )
{
	m_iSwapChain = swapchain;
}
//--------------------------------------------------------------------------------