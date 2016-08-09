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
		static MaterialPtr GenerateWireFrame( RendererDX11& Renderer );

		
		static MaterialPtr GenerateStaticTextured( RendererDX11& Renderer );
		static MaterialPtr GenerateSkinnedTextured( RendererDX11& Renderer );
		static MaterialPtr GenerateSkinnedSolid( RendererDX11& Renderer );

		static MaterialPtr GeneratePhong( RendererDX11& Renderer );
		static MaterialPtr GenerateSolidColor( RendererDX11& Renderer );

		static MaterialPtr GenerateFromFile( RendererDX11& Renderer, std::wstring& file, unsigned int shaders );

		static MaterialPtr GenerateKinectReconstructionMaterial( RendererDX11& Renderer );
		static MaterialPtr GenerateKinectDepthBufferMaterial( RendererDX11& Renderer );

		static MaterialPtr GenerateImmediateGeometrySolidMaterial( RendererDX11& Renderer );
		static MaterialPtr GenerateImmediateGeometryTexturedMaterial( RendererDX11& Renderer );
		static MaterialPtr GenerateImmediateGeometryTransparentMaterial( RendererDX11& Renderer );
		static MaterialPtr GenerateImmediateGeometryTransparentFlatMaterial( RendererDX11& Renderer );

		static MaterialPtr GenerateTextMaterial( RendererDX11& Renderer );

		static MaterialPtr GenerateVolumeGeometryMaterial( RendererDX11& Renderer );

	private:
		MaterialGeneratorDX11();
		
	};
};
#endif // MaterialGeneratorDX11_h


