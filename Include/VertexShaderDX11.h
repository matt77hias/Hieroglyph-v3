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
// VertexShaderDX11
//
//--------------------------------------------------------------------------------
#ifndef VertexShaderDX11_h
#define VertexShaderDX11_h
//--------------------------------------------------------------------------------
#include "ShaderDX11.h"
#include "PipelineManagerDX11.h"
//--------------------------------------------------------------------------------
namespace Glyph3
{
	class VertexShaderDX11 : public ShaderDX11
	{
	public:
		VertexShaderDX11( ID3D11VertexShader* pShader );
		virtual ~VertexShaderDX11();

		virtual ShaderType GetType();

	protected:
		ID3D11VertexShader*			m_pVertexShader;

		friend VertexStageDX11;
	};
};
//--------------------------------------------------------------------------------
#endif // VertexShaderDX11_h
//--------------------------------------------------------------------------------

