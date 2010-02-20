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
#include "Win32RenderWindow.h"
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
extern LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
//--------------------------------------------------------------------------------
Win32RenderWindow::Win32RenderWindow( )
{
	m_hWnd = 0;
}
//--------------------------------------------------------------------------------
Win32RenderWindow::~Win32RenderWindow()
{
	Shutdown();
}
//--------------------------------------------------------------------------------
void Win32RenderWindow::Initialize()
{
	WNDCLASSEX wc;
	
	// Setup the window class
	wc.cbSize			= sizeof(WNDCLASSEX);
	wc.style			= CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc		= WindowProc;
	wc.cbClsExtra		= 0;
	wc.cbWndExtra		= 0;
	wc.hInstance		= 0;
	wc.hIcon			= LoadIcon(NULL, IDI_APPLICATION);
	wc.hCursor			= LoadCursor(NULL, IDC_ARROW); 
	wc.hbrBackground	= (HBRUSH)GetStockObject(BLACK_BRUSH);
	wc.lpszMenuName		= NULL;
	wc.lpszClassName	= L"HeiroglyphWin32";
	wc.hIconSm			= LoadIcon(NULL, IDI_APPLICATION);
	
	// Register the window class
	RegisterClassEx(&wc);

	// Specify the window style
	m_dStyle = (WS_OVERLAPPEDWINDOW | WS_VISIBLE) & ~WS_THICKFRAME;
	

	// Record the desired device size
	RECT rc;
	rc.top = rc.left = 0;
	rc.right = m_iWidth;
	rc.bottom = m_iHeight;
	
	// Adjust the window size for correct device size
	AdjustWindowRect(&rc, m_dStyle, FALSE);

	long lwidth = rc.right - rc.left;
	long lheight = rc.bottom - rc.top;

	long lleft = (long)m_iLeft;	
	long ltop = (long)m_iTop;


	// Create an instance of the window
	m_hWnd = CreateWindowEx(
		NULL,							// extended style
		L"HeiroglyphWin32",				// class name
		m_sCaption.c_str(),				// instance title
		m_dStyle,						// window style
		lleft,ltop,						// initial x,y
		lwidth,							// initial width
		lheight,						// initial height
		NULL,							// handle to parent 
		NULL,							// handle to menu
		NULL,							// instance of this application
		NULL );							// extra creation parms
	
	// Initially display the window
	ShowWindow( m_hWnd, SW_SHOWNORMAL );
    UpdateWindow( m_hWnd );
}
//--------------------------------------------------------------------------------
void Win32RenderWindow::Shutdown()
{
	if ( m_hWnd )
		DestroyWindow( m_hWnd );

	m_hWnd = 0;
}
//--------------------------------------------------------------------------------
void Win32RenderWindow::Paint()
{
	
}
//--------------------------------------------------------------------------------