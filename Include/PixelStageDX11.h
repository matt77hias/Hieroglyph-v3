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
// PixelStageDX11
//
//--------------------------------------------------------------------------------
#ifndef PixelStageDX11_h
#define PixelStageDX11_h
//--------------------------------------------------------------------------------
#include "ShaderStageDX11.h"
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

		virtual void BindShaderProgram( ID3D11DeviceContext* );
		virtual void BindConstantBuffers( ID3D11DeviceContext* pContext, int count );
		virtual void BindSamplerStates( ID3D11DeviceContext* pContext, int count );
		virtual void BindShaderResourceViews( ID3D11DeviceContext* pContext, int count );
		virtual void BindUnorderedAccessViews( ID3D11DeviceContext* pContext, int count );
	};
};
//--------------------------------------------------------------------------------
#endif // PixelStageDX11_h
//--------------------------------------------------------------------------------

