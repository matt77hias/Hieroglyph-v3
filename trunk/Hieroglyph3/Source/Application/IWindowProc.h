//
// http://www.opensource.org/licenses/mit-license.php
//
// Copyright (c) 2011 Jason Zink and Francois Piette
//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
// IWindowProc
//
//--------------------------------------------------------------------------------
#ifndef IWindowProc_h
#define IWindowProc_h
//--------------------------------------------------------------------------------
namespace Glyph3
{
	// This class is the interface for window's message handling
	class IWindowProc
	{
	public:
		virtual LRESULT WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) = 0;
	};
};
//--------------------------------------------------------------------------------
#endif // IWindowProc_h
