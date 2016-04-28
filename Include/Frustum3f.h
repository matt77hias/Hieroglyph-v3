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
// Frustum3f
//
// This class represents a view frustum in 3D space.  It takes as input a
// view/projection matrix and builds the six planes of the frustum from it.  It
// can then be used for interection tests with points or spheres.
//--------------------------------------------------------------------------------
#ifndef Frustum3f_h
#define Frustum3f_h
//--------------------------------------------------------------------------------
#include "Plane3f.h"
#include "Vector3f.h"
#include "Sphere3f.h"
#include "Matrix4f.h"
#include <array>
//--------------------------------------------------------------------------------
namespace Glyph3
{
	struct Frustum3f
	{
		Frustum3f();
		Frustum3f( const Matrix4f& ViewProjection );
		~Frustum3f();

		void Update( const Matrix4f& ViewProjection, bool bNormalize );
		bool Test( const Vector3f& TestPoint ) const;
		bool Test( const Sphere3f& TestSphere ) const;

		// for use with the CEntity interface
		bool Intersects( const Sphere3f& test ) const;
		bool Envelops( const Sphere3f& test ) const;

		std::array<Plane3f,6> planes;
	};
};
//--------------------------------------------------------------------------------
#endif // Frustum3f_h
//--------------------------------------------------------------------------------