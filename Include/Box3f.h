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
// Box3f
//
//--------------------------------------------------------------------------------
#ifndef Box3f_h
#define Box3f_h
//--------------------------------------------------------------------------------
#include "Vector3f.h"
#include <array>
//--------------------------------------------------------------------------------
namespace Glyph3
{
	struct Box3f
	{
		Box3f( );
		Box3f( const Vector3f& Center, const Vector3f& Forward, const Vector3f& Up, 
			const Vector3f& Right, float fextents, float uextents, float rextents );
		~Box3f( );

		Vector3f				center;
		std::array<Vector3f,3>	axes;
		std::array<float,3>		extents;
	};
};
//--------------------------------------------------------------------------------
#endif // Box3f_h
//--------------------------------------------------------------------------------