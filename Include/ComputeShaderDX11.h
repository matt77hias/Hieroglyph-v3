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
// ComputeShaderDX11
//
//--------------------------------------------------------------------------------
#ifndef ComputeShaderDX11_h
#define ComputeShaderDX11_h
//--------------------------------------------------------------------------------
#include "ShaderDX11.h"
#include "PipelineManagerDX11.h"
//--------------------------------------------------------------------------------
namespace Glyph3
{
	class ComputeShaderDX11 : public ShaderDX11
	{
	public:
		ComputeShaderDX11( ID3D11ComputeShader* pShader );
		virtual ~ComputeShaderDX11();

		virtual ShaderType GetType();

	protected:
		ID3D11ComputeShader*			m_pComputeShader;

		friend ComputeStageDX11;
	};
};
//--------------------------------------------------------------------------------
#endif // ComputeShaderDX11_h
//--------------------------------------------------------------------------------

