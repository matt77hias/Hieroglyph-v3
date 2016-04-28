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
#include "RenderWindow.h"
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
RenderWindow::RenderWindow( )
{
	m_hWnd = 0;
	m_dStyle = (WS_OVERLAPPEDWINDOW | WS_VISIBLE);
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

	UpdateWindowState();
}
//--------------------------------------------------------------------------------
void RenderWindow::SetHeight( int height )
{
	m_iHeight = height;

	UpdateWindowState();
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
	POINT point;
	point.x = 0;
	point.y = 0;

	ClientToScreen( m_hWnd, &point );

	return( point.x );
}
//--------------------------------------------------------------------------------
int RenderWindow::GetTop()
{
	POINT point;
	point.x = 0;
	point.y = 0;

	ClientToScreen( m_hWnd, &point );

	return( point.y );
}
//--------------------------------------------------------------------------------
void RenderWindow::SetSize( int width, int height )
{
	m_iWidth = width;
	m_iHeight = height;

	UpdateWindowState();
}
//--------------------------------------------------------------------------------
void RenderWindow::SetPosition( int left, int top )
{
	// The position is specified in terms of the client area - so the actual 
	// Win32 function needs to use the modified screen coordinates that will put
	// the window's client area at the desired location.

	m_iLeft = left;
	m_iTop = top;

	UpdateWindowState();
}
//--------------------------------------------------------------------------------
void RenderWindow::SetCaption( std::wstring& caption )
{
	m_sCaption = caption;

	if ( m_hWnd != 0 )
		SetWindowText( m_hWnd, m_sCaption.c_str() );
}
//--------------------------------------------------------------------------------
void RenderWindow::ResizeWindow( int width, int height )
{
	// This method is called for a WM_SIZE event, so we just update our local
	// state instead of also calling UpdateWindowState().

	m_iWidth = width;
	m_iHeight = height;
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
void RenderWindow::UpdateWindowState()
{
	if ( m_hWnd != 0 )
	{
		RECT ClientRect;
		ClientRect.left = 0;
		ClientRect.top = 0;
		ClientRect.right = m_iWidth;
		ClientRect.bottom = m_iHeight;

		// Adjust the window size for correct device size
		RECT WindowRect = ClientRect;
		AdjustWindowRect( &WindowRect, m_dStyle, FALSE );

		int deltaX = ( WindowRect.right - ClientRect.right ) / 2;
		int deltaY = ( WindowRect.bottom - ClientRect.bottom ) / 2;

		MoveWindow( m_hWnd, m_iLeft - deltaX, m_iTop - deltaY, 
			m_iWidth + deltaX * 2, m_iHeight + deltaY * 2, true );
	}
}
//--------------------------------------------------------------------------------
void RenderWindow::SetStyle( DWORD dStyle )
{
	m_dStyle = dStyle;

	SetWindowLongPtr( m_hWnd, GWL_EXSTYLE, m_dStyle );
}
//--------------------------------------------------------------------------------
DWORD RenderWindow::GetStyle()
{
	return( m_dStyle );
}
//--------------------------------------------------------------------------------
