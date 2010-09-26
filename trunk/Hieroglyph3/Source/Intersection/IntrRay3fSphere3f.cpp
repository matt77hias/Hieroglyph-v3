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
#include "PCH.h"
#include "IntrRay3fSphere3f.h"
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
IntrRay3fSphere3f::IntrRay3fSphere3f( Ray3f& ray, Sphere3f& sphere )
{
	m_Ray = ray;
	m_Sphere = sphere;
	m_iQuantity = 0;
}
//--------------------------------------------------------------------------------
IntrRay3fSphere3f::~IntrRay3fSphere3f()
{
}
//--------------------------------------------------------------------------------
bool IntrRay3fSphere3f::Test()
{
	// Calculate a vector from the sphere center to the ray origin
	Vector3f delta = m_Ray.Origin - m_Sphere.Center;

	// Compare the length of the new vector agains the radius of the sphere
	float fA0 = delta.Dot( delta ) - m_Sphere.Radius * m_Sphere.Radius;

	// If the radius is larger, then the ray origin is inside the sphere
	if ( fA0 <= 0.0f )
		return( true );

	// Test if the ray is pointing in the correct direction
	float fA1 = m_Ray.Direction.Dot( delta );

	// If the result is positive, then there is no way for the ray to intersect
	// the sphere because it is moving away from the sphere center
	if ( fA1 >= 0.0f )
		return( false );

	return( fA1 * fA1 >= fA0 );

	return( false );
}
//--------------------------------------------------------------------------------
bool IntrRay3fSphere3f::Find()
{
	// Calculate a vector from the sphere center to the ray origin
	Vector3f delta = m_Ray.Origin - m_Sphere.Center;

	// Compare the length of the new vector agains the radius of the sphere
	float fA0 = delta.Dot( delta ) - m_Sphere.Radius * m_Sphere.Radius;
	float fA1 = m_Ray.Direction.Dot( delta );

	float fDiscr;
	float fRoot;

	// If the radius is larger, then the ray origin is inside the sphere
	if ( fA0 <= 0.0f )
	{
		m_iQuantity = 1;
		fDiscr = fA1*fA1 - fA0;
		
		fRoot = sqrt( fDiscr );

		m_afRayT[0] = -fA1 + fRoot;
		m_aPoints[0] = m_Ray.Origin + m_Ray.Direction * m_afRayT[0];

		return( true );
	}

    if ( fA1 >= 0.0f )
    {
        m_iQuantity = 0;
        return false;
    }

    fDiscr = fA1*fA1 - fA0;

    if ( fDiscr < 0.0f )
    {
        m_iQuantity = 0;
    }
    else if ( fDiscr >= 0.00000001f ) //Math<Real>::ZERO_TOLERANCE)
    {
        fRoot = sqrt( fDiscr );
        m_afRayT[0] = -fA1 - fRoot;
        m_afRayT[1] = -fA1 + fRoot;
        m_aPoints[0] = m_Ray.Origin + m_Ray.Direction * m_afRayT[0];
        m_aPoints[1] = m_Ray.Origin + m_Ray.Direction * m_afRayT[1];
        m_iQuantity = 2;
    }
    else
    {
        m_afRayT[0] = -fA1;
        m_aPoints[0] = m_Ray.Origin + m_Ray.Direction * m_afRayT[0];
        m_iQuantity = 1;
    }

    return( m_iQuantity > 0 );
}
//--------------------------------------------------------------------------------