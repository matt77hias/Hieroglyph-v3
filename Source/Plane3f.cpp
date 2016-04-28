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
#include "PCH.h"
#include "Plane3f.h"
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
Plane3f::Plane3f()
{
	a = 0.0f;
	b = 0.0f;
	c = 0.0f;
	d = 0.0f;
}
//--------------------------------------------------------------------------------
Plane3f::Plane3f(float A, float B, float C, float D)
{
	a = A;
	b = B;
	c = C;
	d = D;
}
//--------------------------------------------------------------------------------
Plane3f::~Plane3f()
{
}
//--------------------------------------------------------------------------------
void Plane3f::Normalize()
{
	float fMagnitude = sqrt(a*a + b*b + c*c);

	a /= fMagnitude;
	b /= fMagnitude;
	c /= fMagnitude;
	d /= fMagnitude;
}
//--------------------------------------------------------------------------------
float Plane3f::DistanceToPoint( const Vector3f& pt ) const
{
	return (a * pt.x +
			b * pt.y +
			c * pt.z +
			d);
}
//--------------------------------------------------------------------------------
