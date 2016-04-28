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
// TexturedVertex
//
// This class represents a textured vertex, with only a position and texture
// coordinate.  This is intended to be used with a material that can provide
// a texture SRV, and most likely also a color value to apply to the texture.  
// That color would also provide an alpha value for transparency applied to the
// texture.
//--------------------------------------------------------------------------------
#ifndef TexturedVertex_h
#define TexturedVertex_h
//--------------------------------------------------------------------------------
#include "PCH.h"
#include "Vector2f.h"
#include "Vector3f.h"
//--------------------------------------------------------------------------------
namespace Glyph3
{
	class TexturedVertex
	{

	public:
		TexturedVertex();
		~TexturedVertex();

		struct Vertex {
			Vector3f position;
			Vector2f texcoords;
		};

		static unsigned int GetElementCount();
		static D3D11_INPUT_ELEMENT_DESC Elements[2];
	};
};
//--------------------------------------------------------------------------------
#endif // TexturedVertex_h
//--------------------------------------------------------------------------------