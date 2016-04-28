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
// DomainStageDX11
//
//--------------------------------------------------------------------------------
#ifndef DomainStageDX11_h
#define DomainStageDX11_h
//--------------------------------------------------------------------------------
#include "ShaderStageDX11.h"
//--------------------------------------------------------------------------------
namespace Glyph3
{
	class DomainStageDX11 : public ShaderStageDX11
	{
	public:
		DomainStageDX11();
		virtual ~DomainStageDX11();

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
#endif // DomainStageDX11_h
//--------------------------------------------------------------------------------

