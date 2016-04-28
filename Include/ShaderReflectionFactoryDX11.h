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
// ShaderReflectionFactoryDX11
//
//--------------------------------------------------------------------------------
#ifndef ShaderReflectionFactoryDX11_h
#define ShaderReflectionFactoryDX11_h
//--------------------------------------------------------------------------------
#include "PCH.h"
#include "ShaderDX11.h"
#include "ShaderReflectionDX11.h"
//--------------------------------------------------------------------------------
namespace Glyph3
{
	class ShaderReflectionFactoryDX11
	{
	public:
		~ShaderReflectionFactoryDX11();

		static ShaderReflectionDX11* GenerateReflection( ShaderDX11& shader );

	private:
		ShaderReflectionFactoryDX11();
	};

};
//--------------------------------------------------------------------------------
#endif // ShaderReflectionFactoryDX11_h
//--------------------------------------------------------------------------------

