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
// ShaderFactoryDX11
//
//--------------------------------------------------------------------------------
#ifndef ShaderFactoryDX11_h
#define ShaderFactoryDX11_h
//--------------------------------------------------------------------------------
#include "PCH.h"
#include "ShaderDX11.h"
//--------------------------------------------------------------------------------
namespace Glyph3
{
	class ShaderFactoryDX11
	{
	public:
		~ShaderFactoryDX11();

		static ID3DBlob* GenerateShader( ShaderType type, std::wstring& filename, std::wstring& function,
            std::wstring& model, const D3D_SHADER_MACRO* pDefines, bool enablelogging = true );

		static ID3DBlob* GeneratePrecompiledShader( std::wstring& filename, std::wstring& function,
            std::wstring& model );

	private:
		ShaderFactoryDX11();
	};

};
//--------------------------------------------------------------------------------
#endif // ShaderFactoryDX11_h
//--------------------------------------------------------------------------------

