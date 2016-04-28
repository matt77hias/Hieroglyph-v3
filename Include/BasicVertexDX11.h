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
// BasicVertexDX11
//
// This class represents a simple, standard vertex type for use in one of the
// various pipeline executor templates.  With a position, normal, color, and 
// texture coordinates, this layout is fairly generic and should work for most
// basic effects.
//
// Included in the class is an inner class representing the vertex structure, 
// along with the input assembler layout to be used with it.
//--------------------------------------------------------------------------------
#ifndef BasicVertexDX11_h
#define BasicVertexDX11_h
//--------------------------------------------------------------------------------
#include "PCH.h"
#include "Vector2f.h"
#include "Vector3f.h"
#include "Vector4f.h"
//--------------------------------------------------------------------------------
namespace Glyph3
{
	class BasicVertexDX11
	{

	public:
		BasicVertexDX11();
		~BasicVertexDX11();

		struct Vertex {
			Vector3f position;
			Vector3f normal;
			Vector4f color;
			Vector2f texcoords;
		};

		static unsigned int GetElementCount();
		static D3D11_INPUT_ELEMENT_DESC Elements[4];
	};
};
//--------------------------------------------------------------------------------
#endif // BasicVertexDX11_h
//--------------------------------------------------------------------------------