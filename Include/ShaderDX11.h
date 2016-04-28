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
// ShaderDX11
//
// The shader base class provides the mechanics of linking state information to
// a given shader.  Each type of shader utilizes various resources which are 
// managed by the renderer.  The specific resources are determined at shader load
// time via the reflection API and stored in the lists shown below.
//
// When a shader is bound to the pipeline, it's needed resources are updated if
// needed and then loaded into the corresponding shader stage.
//--------------------------------------------------------------------------------
#ifndef ShaderDX11_h
#define ShaderDX11_h
//--------------------------------------------------------------------------------
#include "PCH.h"
#include "ShaderReflectionDX11.h"
//--------------------------------------------------------------------------------
namespace Glyph3
{

	class ShaderDX11
	{
	public:
		ShaderDX11();
		virtual ~ShaderDX11();

		virtual ShaderType GetType() = 0;
		

		void SetReflection( ShaderReflectionDX11* pReflection );
		ShaderReflectionDX11* GetReflection( );

		std::wstring ToString();

	public:
		std::wstring							FileName;
		std::wstring							Function;
		std::wstring							ShaderModel;
		std::string								ShaderText;
		ID3DBlob*								m_pCompiledShader;
		ShaderReflectionDX11*					m_pReflection;
	};
};
//--------------------------------------------------------------------------------
#endif // ShaderDX11_h
//--------------------------------------------------------------------------------

