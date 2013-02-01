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
// ShaderResourceViewDX11
//
//--------------------------------------------------------------------------------
#ifndef ShaderResourceViewDX11_h
#define ShaderResourceViewDX11_h
//--------------------------------------------------------------------------------
#include "ResourceViewDX11.h"
#include "PipelineManagerDx11.h"
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
		
		friend PipelineManagerDX11;
		friend RendererDX11;
	};
};
//--------------------------------------------------------------------------------
#endif // ShaderResourceViewDX11_h
//--------------------------------------------------------------------------------

