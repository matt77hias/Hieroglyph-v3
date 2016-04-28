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
// PixelShaderDX11
//
//--------------------------------------------------------------------------------
#ifndef PixelShaderDX11_h
#define PixelShaderDX11_h
//--------------------------------------------------------------------------------
#include "ShaderDX11.h"
#include "PipelineManagerDX11.h"
//--------------------------------------------------------------------------------
namespace Glyph3
{
	class PixelShaderDX11 : public ShaderDX11
	{
	public:
		PixelShaderDX11( ID3D11PixelShader* pShader );
		virtual ~PixelShaderDX11();

		virtual ShaderType GetType();

	protected:
		ID3D11PixelShader*			m_pPixelShader;

		friend PixelStageDX11;
	};
};
//--------------------------------------------------------------------------------
#endif // PixelShaderDX11_h
//--------------------------------------------------------------------------------

