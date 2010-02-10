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
// ShaderResourceViewDX11
//
//--------------------------------------------------------------------------------
#include "ResourceViewDX11.h"
//--------------------------------------------------------------------------------
#ifndef ShaderResourceViewDX11_h
#define ShaderResourceViewDX11_h
//--------------------------------------------------------------------------------
namespace Glyph3
{
	class ShaderResourceViewDX11 : public ResourceViewDX11
	{
	public:
		ShaderResourceViewDX11( ID3D11ShaderResourceView* pView );
		virtual ~ShaderResourceViewDX11();

	protected:
		ID3D11ShaderResourceView*			m_pShaderResourceView;
		friend RendererDX11;
	};
};
//--------------------------------------------------------------------------------
#endif // ShaderResourceViewDX11_h
//--------------------------------------------------------------------------------

