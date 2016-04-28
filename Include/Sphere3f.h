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
// Sphere3f
//
// This class represents a sphere with data members of a center and a radius.
// It may be used for intersection tests as well as visibility tests.
//--------------------------------------------------------------------------------
#ifndef Sphere3f_h
#define Sphere3f_h
//--------------------------------------------------------------------------------
#include "Vector3f.h"
//--------------------------------------------------------------------------------
namespace Glyph3
{
	struct Sphere3f
	{
		Sphere3f( );
		Sphere3f( const Vector3f& Center, float Radius );
		~Sphere3f( );

		bool Intersects( const Sphere3f& test ) const;
		bool Envelops( const Sphere3f& test ) const;

		void SamplePosition( Vector3f& position, float theta, float phi ) const;
		void SampleNormal( Vector3f& normal, float theta, float phi ) const;
		void SamplePositionAndNormal( Vector3f& position, Vector3f& normal, float theta, float phi ) const;

		Vector3f	center;
		float		radius;
	};
};
//--------------------------------------------------------------------------------
#endif // Sphere3f_h
//--------------------------------------------------------------------------------