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
// DomainShaderDX11
//
//--------------------------------------------------------------------------------
#ifndef DomainShaderDX11_h
#define DomainShaderDX11_h
//--------------------------------------------------------------------------------
#include "ShaderDX11.h"
#include "PipelineManagerDX11.h"
//--------------------------------------------------------------------------------
namespace Glyph3
{
	class DomainShaderDX11 : public ShaderDX11
	{
	public:
		DomainShaderDX11( ID3D11DomainShader* pShader );
		virtual ~DomainShaderDX11();

		virtual ShaderType GetType();

	protected:
		ID3D11DomainShader*			m_pDomainShader;

		friend DomainStageDX11;
	};
};
//--------------------------------------------------------------------------------
#endif // DomainShaderDX11_h
//--------------------------------------------------------------------------------

