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
// IRenderer
//
// This interface class provides the basis of which all of the renderer classes
// are built upon.  
//--------------------------------------------------------------------------------
#ifndef IRenderer_h
#define IRenderer_h
//--------------------------------------------------------------------------------
#include "PCH.h"
//--------------------------------------------------------------------------------
namespace Glyph3
{
	enum RendererType
	{
		DX11
	};

	class IRenderer
	{
	public:
		virtual ~IRenderer() {};

		virtual bool Initialize( int w, int h, bool windowed, HWND handle ) { return false; };
		virtual void Shutdown() = 0;
		virtual void Present( HWND hWnd = 0, int SwapChain = -1 ) = 0;

	protected:
		IRenderer& operator=(const IRenderer&);
	};
};
//--------------------------------------------------------------------------------
#endif // IRenderer_h
//--------------------------------------------------------------------------------

