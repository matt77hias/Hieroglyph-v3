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
// SpriteVertexDX11
//
// This class represents a simple, standard vertex type for use in one of the
// various pipeline executor templates.  With a position, normal, color, and 
// texture coordinates, this layout is fairly generic and should work for most
// basic effects.
//
// Included in the class is an inner class representing the vertex structure, 
// along with the input assembler layout to be used with it.
//--------------------------------------------------------------------------------
#ifndef SpriteVertexDX11_h
#define SpriteVertexDX11_h
//--------------------------------------------------------------------------------
#include "PCH.h"
#include "Vector2f.h"
#include "Vector4f.h"
#include "Matrix4f.h"
//--------------------------------------------------------------------------------
namespace Glyph3
{
	class SpriteVertexDX11
	{

	public:
		SpriteVertexDX11();
		~SpriteVertexDX11();

		struct SpriteDrawRect
		{
			float X;
			float Y;
			float Width;
			float Height;
		};

		struct InstanceData
		{
			Matrix4f Transform;
			Vector4f Color;
			SpriteDrawRect DrawRect;
		};

		struct VertexData
		{
			Vector2f Position;
			Vector2f TexCoord;
		};

		static unsigned int GetElementCount();
		static D3D11_INPUT_ELEMENT_DESC Elements[8];
	};
};
//--------------------------------------------------------------------------------
#endif // SpriteVertexDX11_h
//--------------------------------------------------------------------------------