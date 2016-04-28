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
#include "Win32Window.h"
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
LRESULT CALLBACK InternalWindowProc2( HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam )
{
	LONG_PTR ObjPtr = GetWindowLongPtr(hwnd, 0);

	//if (ObjPtr == 0) {
        return( DefWindowProc( hwnd, msg, wparam, lparam ) );
	//} else {
	//    return( ((IWindowProc*)ObjPtr)->WindowProc(hwnd, msg, wparam, lparam) );
	//}
}
//--------------------------------------------------------------------------------
Win32Window::Win32Window( unsigned int w, unsigned int h, const std::wstring& caption  )
{
	m_hWnd = 0;
	DWORD style = WS_OVERLAPPEDWINDOW;
	DWORD exstyle = 0;

	m_iSwapChain = -1;



	WNDCLASSEX wc;
	
	// Setup the window class
	memset(&wc,0, sizeof(wc));
	wc.cbSize			= sizeof(WNDCLASSEX);
	wc.style			= CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc		= InternalWindowProc2;
	wc.cbClsExtra		= 0;
	wc.cbWndExtra		= sizeof(void*);//sizeof(WindowProcObj);
	wc.hInstance		= 0;
	wc.hIcon			= LoadIcon(NULL, IDI_APPLICATION);
	wc.hCursor			= LoadCursor(NULL, IDC_ARROW); 
	wc.hbrBackground	= (HBRUSH)GetStockObject(BLACK_BRUSH);
	wc.lpszMenuName		= NULL;
	wc.lpszClassName	= L"HieroglyphWin32-2";
	wc.hIconSm			= LoadIcon(NULL, IDI_APPLICATION);
	
	//if ( WindowProcObj != nullptr ) {
	//	WindowProcObj->BeforeRegisterWindowClass(wc);
	//}

	// Register the window class
	RegisterClassEx(&wc);

	// Record the desired device size
	RECT rc;
	rc.top = rc.left = 0;
	rc.right = w;
	rc.bottom = h;
	
	// Adjust the window size for correct device size
	AdjustWindowRectEx(&rc, style, false, exstyle);

	long lwidth = rc.right - rc.left;
	long lheight = rc.bottom - rc.top;

	long lleft = (long)10;	
	long ltop = (long)10;


	// Create an instance of the window
	m_hWnd = CreateWindowEx(
		exstyle,						// extended style
		wc.lpszClassName, 				// class name
		caption.c_str(),				// instance title
		0,								// window style
		lleft, ltop,					// initial x, y
		lwidth,							// initial width
		lheight,						// initial height
		NULL,							// handle to parent 
		NULL,							// handle to menu
		NULL,							// instance of this application
		NULL );							// extra creation parms
	
	if (m_hWnd) {
		// Set in the "extra" bytes the pointer to the IWindowProc object
		// which handles messages for the window
//        SetWindowLongPtr(m_hWnd, 0, (LONG_PTR)WindowProcObj);
    	// Initially display the window
		ShowWindow( m_hWnd, SW_SHOWNORMAL );
		UpdateWindow( m_hWnd );
	}
}
//--------------------------------------------------------------------------------
Win32Window::~Win32Window()
{
	if ( m_hWnd != 0 ) {
		DestroyWindow( m_hWnd );
		m_hWnd = 0;
	}
}
//--------------------------------------------------------------------------------
HWND Win32Window::GetHandle()
{
	return( m_hWnd );
}
//--------------------------------------------------------------------------------
void Win32Window::SetWidth( unsigned int width )
{
	if ( m_hWnd != 0 )
	{
		RECT old_client;
		GetClientRect( m_hWnd, &old_client );

		SetSize( width, old_client.bottom );
	}
}
//--------------------------------------------------------------------------------
void Win32Window::SetHeight( unsigned int height )
{
	if ( m_hWnd != 0 )
	{
		RECT old_client;
		GetClientRect( m_hWnd, &old_client );

		SetSize( old_client.right, height );
	}
}
//--------------------------------------------------------------------------------
//int Win32Window::GetWidth()
//{
//	RECT rect;
//	GetClientRect( m_hWnd, &rect );
//	
//	return( rect.right - rect.left );
//}
////--------------------------------------------------------------------------------
//int Win32Window::GetHeight()
//{
//	RECT rect;
//	GetClientRect( m_hWnd, &rect );
//
//	return( rect.bottom - rect.top );
//}
////--------------------------------------------------------------------------------
//int Win32Window::GetLeft()
//{
//	POINT point;
//	point.x = 0;
//	point.y = 0;
//
//	ClientToScreen( m_hWnd, &point );
//
//	return( point.x );
//}
////--------------------------------------------------------------------------------
//int Win32Window::GetTop()
//{
//	POINT point;
//	point.x = 0;
//	point.y = 0;
//
//	ClientToScreen( m_hWnd, &point );
//
//	return( point.y );
//}
//--------------------------------------------------------------------------------
void Win32Window::SetSize( unsigned int width, unsigned int height )
{
	if ( m_hWnd != 0 )
	{
		RECT old_client, new_client, old_window, new_window;

		// Get the current window style
		LONG style = GetWindowLongPtr( m_hWnd, GWL_STYLE );
		LONG exstyle = GetWindowLongPtr( m_hWnd, GWL_EXSTYLE );

		// Record the existing window state
		GetClientRect( m_hWnd, &old_client );
		GetWindowRect( m_hWnd, &old_window );

		// Update the desired new client info.  The client rect is referenced to
		// (0,0) so its top and left are always zero and the right and bottom are
		// equivalent to the width and height.
		new_client = old_client;
		new_client.right = new_client.left + width;
		new_client.bottom = new_client.top + height;

		// Adjust the new window size for the window style
		new_window = new_client;
		AdjustWindowRectEx( &new_window, style, FALSE, exstyle );

		int new_width = new_window.right - new_window.left;
		int new_height = new_window.bottom - new_window.top;

		// Apply the existing window position to the new window state
		new_window.left = old_window.left;
		new_window.right = old_window.left + new_width;
		new_window.top = old_window.top;
		new_window.bottom = old_window.top + new_height;

		MoveWindow( m_hWnd, 
			new_window.left, 
			new_window.top, 
			new_window.right - new_window.left,
			new_window.bottom - new_window.top, true );
	}
}
//--------------------------------------------------------------------------------
void Win32Window::SetPosition( unsigned int left, unsigned int top )
{
	if ( m_hWnd != 0 )
	{
		RECT old_client, new_client, old_window, new_window;

		// Get the current window style
		LONG style = GetWindowLongPtr( m_hWnd, GWL_EXSTYLE );

		// Record the existing window state
		GetClientRect( m_hWnd, &old_client );
		GetWindowRect( m_hWnd, &old_window );

		// Update the desired new client info
		new_client = old_client;

		// Adjust the new window size for the window style
		new_window = new_client;
		AdjustWindowRect( &new_window, style, FALSE );

		// Apply the existing window position to the new window state
		new_window.left = left;
		new_window.top = top;

		MoveWindow( m_hWnd, 
			new_window.left, 
			new_window.right, 
			new_window.right - new_window.left,
			new_window.bottom - new_window.top, true );
	}
}
//--------------------------------------------------------------------------------
//void Win32Window::SetCaption( std::wstring& caption )
//{
//	m_sCaption = caption;
//
//	if ( m_hWnd != 0 )
//		SetWindowText( m_hWnd, m_sCaption.c_str() );
//}
////--------------------------------------------------------------------------------
//void Win32Window::ResizeWindow( int width, int height )
//{
//	// This method is called for a WM_SIZE event, so we just update our local
//	// state instead of also calling UpdateWindowState().
//
//	m_iWidth = width;
//	m_iHeight = height;
//}
////--------------------------------------------------------------------------------
//std::wstring Win32Window::GetCaption()
//{
//	return( m_sCaption );
//}
////--------------------------------------------------------------------------------
//int Win32Window::GetSwapChain()
//{
//	return( m_iSwapChain );
//}
////--------------------------------------------------------------------------------
//void Win32Window::SetSwapChain( int swapchain )
//{
//	m_iSwapChain = swapchain;
//}
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
//void Win32Window::SetStyle( DWORD dStyle )
//{
//	m_dStyle = dStyle;
//
//	SetWindowLongPtr( m_hWnd, GWL_EXSTYLE, m_dStyle );
//}
////--------------------------------------------------------------------------------
//DWORD Win32Window::GetStyle()
//{
//	return( m_dStyle );
//}
//--------------------------------------------------------------------------------
// This free function is used as a hook into the window's messaging system.  When
// the window is initialized, the owner passes a pointer to a IWindowProc 
// implementation.  When Windows sends a message to this window, we call this 
// handler function - which will either call the default processor (when no valid
// pointer is there) or call the IWindowProc method (when a valid one is 
// available).
//--------------------------------------------------------------------------------
