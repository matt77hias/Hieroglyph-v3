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
// PixelStageDX11
//
//--------------------------------------------------------------------------------
#include "ShaderStageDX11.h"
//--------------------------------------------------------------------------------
#ifndef PixelStageDX11_h
#define PixelStageDX11_h
//--------------------------------------------------------------------------------
namespace Glyph3
{
	class PixelStageDX11 : public ShaderStageDX11
	{
	public:
		PixelStageDX11();
		virtual ~PixelStageDX11();

	protected:
		virtual ShaderType GetType();

		virtual void BindConstantBuffers( ID3D11DeviceContext* pContext, int count );
		virtual void BindSamplerStates( ID3D11DeviceContext* pContext, int count );
		virtual void BindShaderResourceViews( ID3D11DeviceContext* pContext, int count );
		virtual void BindUnorderedAccessViews( ID3D11DeviceContext* pContext, int count );

		friend RendererDX11;
	};
};
//--------------------------------------------------------------------------------
#endif // PixelStageDX11_h
//--------------------------------------------------------------------------------

