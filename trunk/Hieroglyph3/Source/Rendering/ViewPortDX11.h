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
// ViewPortDX11
//
//--------------------------------------------------------------------------------
#ifndef ViewPortDX11_h
#define ViewPortDX11_h
//--------------------------------------------------------------------------------
#include "RendererDX11.h"
#include "RasterizerStageDX11.h"
//--------------------------------------------------------------------------------
namespace Glyph3
{
	class ViewPortDX11
	{
	public:
		ViewPortDX11( D3D11_VIEWPORT viewport );
		virtual ~ViewPortDX11();

		float GetWidth() const;
		float GetHeight() const;

	protected:
		D3D11_VIEWPORT			m_ViewPort;

		friend RasterizerStageDX11;
		friend RendererDX11;
	};
};
//--------------------------------------------------------------------------------
#endif // ViewPortDX11_h
//--------------------------------------------------------------------------------

