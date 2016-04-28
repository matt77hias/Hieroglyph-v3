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
// Cone3f
//
// This class represents a cone with two points, and the cone radius at each of 
// those two points.  This configuration can also be used to represent a cylinder
// when the two radii are equal, or even a partial cone with its top cut off.
//--------------------------------------------------------------------------------
#ifndef Cone3f_h
#define Cone3f_h
//--------------------------------------------------------------------------------
#include "Vector3f.h"
//--------------------------------------------------------------------------------
namespace Glyph3
{
	struct Cone3f
	{
		Cone3f( );
		Cone3f( const Vector3f& p1, float r1, const Vector3f& p2, float r2 );
		~Cone3f( );

		void SamplePosition( Vector3f& position, float theta, float height_factor ) const;
		void SampleNormal( Vector3f& normal, float theta, float height_factor ) const;
		void SamplePositionAndNormal( Vector3f& position, Vector3f& normal, float theta, float height_factor ) const;

		Vector3f	p1;
		Vector3f	p2;
		float		r1;
		float		r2;
	};
};
//--------------------------------------------------------------------------------
#endif // Cone3f_h
//--------------------------------------------------------------------------------