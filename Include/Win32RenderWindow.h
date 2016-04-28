//--------------------------------------------------------------------------------
// This file is a portion of the Hieroglyph 3 Rendering Engine.  It is distributed
// under the MIT License, available in the root of this distribution and 
// at the following URL:
//
// http://www.opensource.org/licenses/mit-license.php
//
// Copyright (c) Jason Zink 
//--------------------------------------------------------------------------------
// History:
// Nov 15, 2011  F. Piette added argument IWindowProc to Initialize method

//--------------------------------------------------------------------------------
// Win32RenderWindow
//
//--------------------------------------------------------------------------------
#ifndef Win32RenderWindow_h
#define Win32RenderWindow_h
//--------------------------------------------------------------------------------
#include "RenderWindow.h"
#include "IWindowProc.h"
//--------------------------------------------------------------------------------
namespace Glyph3
{
	class Vector2f;

	class Win32RenderWindow : public RenderWindow
	{
	public:
		Win32RenderWindow( );
		virtual ~Win32RenderWindow( );

		virtual void Initialize( IWindowProc* WindowProcObj );
		virtual void Shutdown();
		virtual void Paint();

		Vector2f GetCursorPosition();
	};
};
//--------------------------------------------------------------------------------
#endif // Win32RenderWindow_h
