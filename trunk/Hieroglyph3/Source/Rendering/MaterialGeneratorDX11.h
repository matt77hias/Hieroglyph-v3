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
// MaterialGeneratorDX11
//
//--------------------------------------------------------------------------------
#ifndef MaterialGeneratorDX11_h
#define MaterialGeneratorDX11_h
//--------------------------------------------------------------------------------
#include "MaterialDX11.h"
//--------------------------------------------------------------------------------
namespace Glyph3
{
	class MaterialGeneratorDX11
	{
	public:
		static MaterialDX11* GenerateWireFrame( RendererDX11& Renderer );
		static MaterialDX11* GenerateSkinnedSolid( RendererDX11& Renderer );

		static MaterialDX11* GeneratePhong( RendererDX11& Renderer );
		static MaterialDX11* GenerateSolidColor( RendererDX11& Renderer );

	private:
		MaterialGeneratorDX11();
		
	};
};
#endif // MaterialGeneratorDX11_h


