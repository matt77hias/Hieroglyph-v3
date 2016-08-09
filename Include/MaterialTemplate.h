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
// MaterialTemplate
//
//--------------------------------------------------------------------------------
#ifndef MaterialTemplate_h
#define MaterialTemplate_h
//--------------------------------------------------------------------------------
#include "PCH.h"
#include "MaterialDX11.h"
//--------------------------------------------------------------------------------
namespace Glyph3
{
	// TODO: Take a reference to either an entity or a renderable, and provide
	//       standardized methods for manipulating common properties that we would
	//       expect for this rendering standard.
	//
	//       It may also be interesting to insert options for a quality level here
	//       as well.

	class MaterialTemplate
	{
	public:
		MaterialTemplate();
		~MaterialTemplate();

		enum ColorMode
		{
			ObjectColor,
			VertexColor,
			TexturedColor
		};

		enum LightingMode
		{
			Flat,
			Point
		};

		enum TransparencyMode
		{
			Off,
			Alpha
		};

		void SetColorMode( ColorMode mode );
		void SetLightingMode( LightingMode mode );
		void SetTransparencyMode( TransparencyMode mode );

		MaterialPtr	material;
	};
};
//--------------------------------------------------------------------------------
#endif // MaterialTemplate_h
//--------------------------------------------------------------------------------

