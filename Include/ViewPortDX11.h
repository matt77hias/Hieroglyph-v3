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
		ViewPortDX11();
		ViewPortDX11( D3D11_VIEWPORT viewport );
		~ViewPortDX11();

		float GetWidth() const;
		float GetHeight() const;
		Vector2f GetClipSpacePosition( const Vector2f& screen ) const;
		Vector2f GetScreenSpacePosition( const Vector2f& clip ) const;

	protected:
		D3D11_VIEWPORT			m_ViewPort;

		friend RasterizerStageDX11;
		friend RendererDX11;
	};
};
//--------------------------------------------------------------------------------
#endif // ViewPortDX11_h
//--------------------------------------------------------------------------------

