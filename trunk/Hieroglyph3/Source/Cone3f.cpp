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
	P1( 0.0f, 0.0f, 0.0f ),
	P2( 0.0f, 1.0f, 0.0f ),
	R1( 0.5f ),
	R2( 0.5f )
{
}
//--------------------------------------------------------------------------------
Cone3f::Cone3f( const Vector3f& p1, float r1, const Vector3f& p2, float r2 ) :
	P1( p1 ),
	P2( p2 ),
	R1( r1 ),
	R2( r2 )
{
}
//--------------------------------------------------------------------------------
Cone3f::~Cone3f()
{
}
//--------------------------------------------------------------------------------
eSHAPE Cone3f::GetShapeType( ) const
{
	return( CONE );
}
//--------------------------------------------------------------------------------
void Cone3f::SamplePosition( Vector3f& position, float theta, float height_factor ) const
{
	// Define the axis of the cylinder from P2 to P1, and find its length.

	Vector3f axis = P1 - P2;
	Vector3f vnorm = Vector3f::Normalize( axis );
	float height = axis.Magnitude();

	// Find the unit vector perpendicular to the axis.  Note that if P1 and P2 are the
	// same point, that unit will be the zero vector!

	Vector3f unit = Vector3f::Normalize( Vector3f::Perpendicular( axis ) );

	float delta_radius = R1 - R2;
	float radius = R2 + delta_radius * height_factor;

	// Calculate the position, which is a combination of an offset to the p2 endpoint,
	// shifted along the axis of the cylinder, and a rotated and scaled radius component.
	Matrix3f r;
	r.RotationEuler( vnorm, theta );
	position = P2 + ( vnorm * height * height_factor ) + ( r * unit * radius );
}
//--------------------------------------------------------------------------------
void Cone3f::SampleNormal( Vector3f& normal, float theta, float height_factor ) const
{
	// Define the axis of the cylinder, and find its length.

	Vector3f axis = P1 - P2;
	Vector3f vnorm = Vector3f::Normalize( axis );
	float height = axis.Magnitude();

	// Find the unit vector perpendicular to the axis.  Note that if P1 and P2 are the
	// same point, that unit will be the zero vector!

	Vector3f unit = Vector3f::Normalize( Vector3f::Perpendicular( axis ) );


	float delta_radius = R1 - R2;
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

	Vector3f axis = P1 - P2;
	Vector3f vnorm = Vector3f::Normalize( axis );
	float height = axis.Magnitude();

	// Find the unit vector perpendicular to the axis.  Note that if P1 and P2 are the
	// same point, that unit will be the zero vector!

	Vector3f unit = Vector3f::Normalize( Vector3f::Perpendicular( axis ) );

	float delta_radius = R1 - R2;
	float slope = delta_radius / height;
	float radius = R2 + delta_radius * height_factor;

	// Calculate the position, which is a combination of an offset to the p2 endpoint,
	// shifted along the axis of the cylinder, and a rotated and scaled radius component.
	Matrix3f r;
	r.RotationEuler( vnorm, theta );
	position = P2 + ( vnorm * height * height_factor ) + ( r * unit * radius );

	// Calculate the normal vector, which is dependent on the delta in radii at each
	// end of the cylinder.
	normal = r * unit - vnorm * slope;
	normal.Normalize();
}
//--------------------------------------------------------------------------------