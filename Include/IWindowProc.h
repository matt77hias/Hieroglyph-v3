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
// IWindowProc
//
// 06.02.2012 File added by Francois Piette.
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
		virtual void BeforeRegisterWindowClass(WNDCLASSEX &wc) = 0;
	};
};
//--------------------------------------------------------------------------------
#endif // IWindowProc_h
