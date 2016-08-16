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
// BezierCubic
//
//--------------------------------------------------------------------------------
#ifndef BezierCubic_h
#define BezierCubic_h
//--------------------------------------------------------------------------------
#include <array>
#include "Vector3f.h"
//--------------------------------------------------------------------------------
namespace Glyph3
{
	class BezierCubic
	{
	public:
		BezierCubic();
		~BezierCubic();

		Vector3f sample( float u ) const;
		Vector3f sample_normal( float u ) const;
		Vector3f sample_tangent( float u ) const;

		std::array<Vector3f,4> points;
	};
};
//--------------------------------------------------------------------------------
#endif // BezierCubic_h
//--------------------------------------------------------------------------------