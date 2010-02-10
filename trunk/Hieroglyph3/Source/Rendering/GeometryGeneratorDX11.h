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
// GeometryGeneratorDX11
//
// The geometry generator produces geometry objects with a number of vertex 
// elements as well as primitive information (i.e. points, lines, and triangles).
//
// The geometry object itself is returned without a fully finalized 
// vertex declaration, and the vertex information needs to be loaded to buffers
// still after the object is returned.  This is done to ensure that the caller
// can add additional per-vertex information after it has already been returned.
//
// With this specification, the various functions can be used in concert to build
// up a complex vertex type with standard functions.
//--------------------------------------------------------------------------------
#ifndef CGeometryGeneratorDX9_h
#define CGeometryGeneratorDX9_h
//--------------------------------------------------------------------------------
#include "GeometryDX11.h"
#include <string>
//--------------------------------------------------------------------------------
namespace Glyph3
{
	class GeometryGeneratorDX11
	{
	public:
		static void GenerateFullScreenQuad( GeometryDX11* pGeometry );

	private:
		GeometryGeneratorDX11();
		
	};
};
#endif // GeometryGeneratorDX11_h


