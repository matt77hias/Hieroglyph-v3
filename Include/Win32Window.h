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
// Win32Window
//--------------------------------------------------------------------------------
#ifndef Win32Window_h
#define Win32Window_h
//--------------------------------------------------------------------------------
#include "PCH.h"
//--------------------------------------------------------------------------------
namespace Glyph3
{
	class Win32Window
	{
	public:
		Win32Window( unsigned int w, unsigned int h, const std::wstring& caption );
		~Win32Window( );

		//virtual void Initialize( IWindowProc* WindowProcObj ) = 0;
		//virtual void Shutdown() = 0;
		//virtual void Paint() = 0;

		HWND GetHandle();

		void SetWidth( unsigned int width );
		void SetHeight( unsigned int height );

		//unsigned int GetWidth();
		//unsigned int GetHeight();
		//unsigned int GetLeft();
		//unsigned int GetTop();

		void SetSize( unsigned int width, unsigned int height );
		void SetPosition( unsigned int left, unsigned int top );

		//void ResizeWindow( int width, int height );

		//int GetSwapChain();
		//void SetSwapChain( int swapchain );

		//void SetCaption( std::wstring& caption );
		//std::wstring GetCaption();

		//void SetStyle( DWORD dStyle );
		//DWORD GetStyle(); 

	protected:

		//void UpdateWindowState();

		HWND			m_hWnd;
		//std::wstring	m_sCaption;
		//int				m_iWidth;
		//int				m_iHeight;
		//int				m_iLeft;
		//int				m_iTop;

		int				m_iSwapChain;

		//DWORD			m_dStyle;
	};
};
//--------------------------------------------------------------------------------
#endif // Win32Window_h
