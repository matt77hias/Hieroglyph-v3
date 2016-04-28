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
// VisualizerVertexDX11
//
// This class represents a simple solid color vertex type for use in one of the
// pipeline executor templates.  This vertex structure requires an instanced
// rendering method to render properly.
//--------------------------------------------------------------------------------
#ifndef VisualizerVertexDX11_h
#define VisualizerVertexDX11_h
//--------------------------------------------------------------------------------
#include "PCH.h"
#include "Vector3f.h"
#include "Vector4f.h"
#include "Matrix4f.h"
//--------------------------------------------------------------------------------
namespace Glyph3
{
	class VisualizerVertexDX11
	{

	public:
		VisualizerVertexDX11();
		~VisualizerVertexDX11();

		
		struct VertexData
		{
			Vector3f position;
		};

		struct InstanceData
		{
			Matrix4f transform;
			Vector4f color;
		};

		static unsigned int GetElementCount();
		static D3D11_INPUT_ELEMENT_DESC Elements[6];
	};
};
//--------------------------------------------------------------------------------
#endif // VisualizerVertexDX11_h
//--------------------------------------------------------------------------------