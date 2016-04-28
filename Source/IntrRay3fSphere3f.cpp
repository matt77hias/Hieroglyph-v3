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
#include "IntrRay3fSphere3f.h"
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
IntrRay3fSphere3f::IntrRay3fSphere3f( const Ray3f& ray, const Sphere3f& sphere ) :
	m_Ray( ray ),
	m_Sphere( sphere ),
	m_iQuantity( 0 )
{
}
//--------------------------------------------------------------------------------
IntrRay3fSphere3f::~IntrRay3fSphere3f()
{
}
//--------------------------------------------------------------------------------
bool IntrRay3fSphere3f::Test()
{
	// Calculate a vector from the sphere center to the ray origin
	Vector3f delta = m_Ray.origin - m_Sphere.center;

	// Compare the length of the new vector agains the radius of the sphere
	float fA0 = delta.Dot( delta ) - m_Sphere.radius * m_Sphere.radius;

	// If the radius is larger, then the ray origin is inside the sphere
	if ( fA0 <= 0.0f )
		return( true );

	// Test if the ray is pointing in the correct direction
	float fA1 = m_Ray.direction.Dot( delta );

	// If the result is positive, then there is no way for the ray to intersect
	// the sphere because it is moving away from the sphere center
	if ( fA1 >= 0.0f )
		return( false );

	return( fA1 * fA1 >= fA0 );
}
//--------------------------------------------------------------------------------
bool IntrRay3fSphere3f::Find()
{
	// Calculate a vector from the sphere center to the ray origin
	Vector3f delta = m_Ray.origin - m_Sphere.center;

	// Compare the length of the new vector agains the radius of the sphere
	float fA0 = delta.Dot( delta ) - m_Sphere.radius * m_Sphere.radius;
	float fA1 = m_Ray.direction.Dot( delta );

	float fDiscr;
	float fRoot;

	// If the radius of the sphere is larger than the distance from the ray 
	// origin to the center of the sphere, then the ray origin is inside the
	// sphere and thus must be intersecting.

	if ( fA0 <= 0.0f )
	{
		m_iQuantity = 1;
		fDiscr = fA1*fA1 - fA0;
		
		fRoot = sqrt( fDiscr );

		m_afRayT[0] = -fA1 + fRoot;
		m_aPoints[0] = m_Ray.origin + m_Ray.direction * m_afRayT[0];

		return( true );
	}

	// If the ray is not inside the sphere, then test if it is pointing away
	// from the sphere (in which case it can't intersect).  This can be
	// determined with the a1 term (see p700-701 in 3D Game Engine Design, 
	// 2nd Ed. for a derivation) being greater than or equal to zero.

    if ( fA1 >= 0.0f )
    {
        m_iQuantity = 0;
        return false;
    }

	// Calculate the discriminant of the equation.  This is the term inside
	// the square root of our quadratic equation.

    fDiscr = fA1*fA1 - fA0;

	// If the discriminant is negative, only complex roots exist which means
	// there is no intersection.

    if ( fDiscr < 0.0f )
    {
        m_iQuantity = 0;
    }
	// If the discriminant is positive, then there is an intersection at two
	// points.  Find them and store the values.
    else if ( fDiscr >= 0.00000001f )
    {
        fRoot = sqrt( fDiscr );
        m_afRayT[0] = -fA1 - fRoot;
        m_afRayT[1] = -fA1 + fRoot;
        m_aPoints[0] = m_Ray.origin + m_Ray.direction * m_afRayT[0];
        m_aPoints[1] = m_Ray.origin + m_Ray.direction * m_afRayT[1];
        m_iQuantity = 2;
    }
	// Otherwise we have the ray origin on the surface of the sphere, and its
	// direction is tangent to the surface.  This gives just one intersection.
    else
    {
        m_afRayT[0] = -fA1;
        m_aPoints[0] = m_Ray.origin + m_Ray.direction * m_afRayT[0];
        m_iQuantity = 1;
    }

    return( m_iQuantity > 0 );
}
//--------------------------------------------------------------------------------