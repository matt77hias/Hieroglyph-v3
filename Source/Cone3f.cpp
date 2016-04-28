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
#include "Cone3f.h"
#include "Matrix3f.h"
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
Cone3f::Cone3f( ) :
	p1( 0.0f, 0.0f, 0.0f ),
	p2( 0.0f, 1.0f, 0.0f ),
	r1( 0.5f ),
	r2( 0.5f )
{
}
//--------------------------------------------------------------------------------
Cone3f::Cone3f( const Vector3f& P1, float R1, const Vector3f& P2, float R2 ) :
	p1( P1 ),
	p2( P2 ),
	r1( R1 ),
	r2( R2 )
{
}
//--------------------------------------------------------------------------------
Cone3f::~Cone3f()
{
}
//--------------------------------------------------------------------------------
void Cone3f::SamplePosition( Vector3f& position, float theta, float height_factor ) const
{
	// Define the axis of the cylinder from P2 to P1, and find its length.

	Vector3f axis = p1 - p2;
	Vector3f vnorm = Vector3f::Normalize( axis );
	float height = axis.Magnitude();

	// Find the unit vector perpendicular to the axis.  Note that if P1 and P2 are the
	// same point, that unit will be the zero vector!

	Vector3f unit = Vector3f::Normalize( Vector3f::Perpendicular( axis ) );

	float delta_radius = r1 - r2;
	float radius = r2 + delta_radius * height_factor;

	// Calculate the position, which is a combination of an offset to the p2 endpoint,
	// shifted along the axis of the cylinder, and a rotated and scaled radius component.
	Matrix3f r;
	r.RotationEuler( vnorm, theta );
	position = p2 + ( vnorm * height * height_factor ) + ( r * unit * radius );
}
//--------------------------------------------------------------------------------
void Cone3f::SampleNormal( Vector3f& normal, float theta, float height_factor ) const
{
	// Define the axis of the cylinder, and find its length.

	Vector3f axis = p1 - p2;
	Vector3f vnorm = Vector3f::Normalize( axis );
	float height = axis.Magnitude();

	// Find the unit vector perpendicular to the axis.  Note that if P1 and P2 are the
	// same point, that unit will be the zero vector!

	Vector3f unit = Vector3f::Normalize( Vector3f::Perpendicular( axis ) );


	float delta_radius = r1 - r2;
	float slope = delta_radius / height;

	// Calculate the position, which is a combination of an offset to the p2 endpoint,
	// shifted along the axis of the cylinder, and a rotated and scaled radius component.
	Matrix3f r;
	r.RotationEuler( vnorm, theta );

	// Calculate the normal vector, which is dependent on the delta in radii at each
	// end of the cylinder.
	normal = r * unit - vnorm * slope;
	normal.Normalize();
}
//--------------------------------------------------------------------------------
void Cone3f::SamplePositionAndNormal( Vector3f& position, Vector3f& normal, float theta, float height_factor ) const
{
	// Define the axis of the cylinder, and find its length.

	Vector3f axis = p1 - p2;
	Vector3f vnorm = Vector3f::Normalize( axis );
	float height = axis.Magnitude();

	// Find the unit vector perpendicular to the axis.  Note that if P1 and P2 are the
	// same point, that unit will be the zero vector!

	Vector3f unit = Vector3f::Normalize( Vector3f::Perpendicular( axis ) );

	float delta_radius = r1 - r2;
	float slope = delta_radius / height;
	float radius = r2 + delta_radius * height_factor;

	// Calculate the position, which is a combination of an offset to the p2 endpoint,
	// shifted along the axis of the cylinder, and a rotated and scaled radius component.
	Matrix3f r;
	r.RotationEuler( vnorm, theta );
	position = p2 + ( vnorm * height * height_factor ) + ( r * unit * radius );

	// Calculate the normal vector, which is dependent on the delta in radii at each
	// end of the cylinder.
	normal = r * unit - vnorm * slope;
	normal.Normalize();
}
//--------------------------------------------------------------------------------