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
#include "IntrRay3fBox3f.h"
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
IntrRay3fBox3f::IntrRay3fBox3f( const Ray3f& ray, Box3f& box ) :
	m_Ray( ray ),
	m_Box( box ),
	m_iQuantity( 0 )
{
}
//--------------------------------------------------------------------------------
IntrRay3fBox3f::~IntrRay3fBox3f()
{
}
//--------------------------------------------------------------------------------
bool IntrRay3fBox3f::Test()
{
    float afWdU[3], afAWdU[3], afDdU[3], afADdU[3], afAWxDdU[3], fRhs;

    Vector3f kDiff = m_Ray.origin - m_Box.center;

    afWdU[0] = m_Ray.direction.Dot( m_Box.axes[0] );
    afAWdU[0] = fabs(afWdU[0]);
    afDdU[0] = kDiff.Dot(m_Box.axes[0]);
    afADdU[0] = fabs(afDdU[0]);
    if (afADdU[0] > m_Box.extents[0] && afDdU[0]*afWdU[0] >= 0.0f)
    {
        return false;
    }

    afWdU[1] = m_Ray.direction.Dot(m_Box.axes[1]);
    afAWdU[1] = fabs(afWdU[1]);
    afDdU[1] = kDiff.Dot(m_Box.axes[1]);
    afADdU[1] = fabs(afDdU[1]);
    if (afADdU[1] > m_Box.extents[1] && afDdU[1]*afWdU[1] >= 0.0)
    {
        return false;
    }

    afWdU[2] = m_Ray.direction.Dot(m_Box.axes[2]);
    afAWdU[2] = fabs(afWdU[2]);
    afDdU[2] = kDiff.Dot(m_Box.axes[2]);
    afADdU[2] = fabs(afDdU[2]);
    if (afADdU[2] > m_Box.extents[2] && afDdU[2]*afWdU[2] >= 0.0)
    {
        return false;
    }

    Vector3f kWxD = m_Ray.direction.Cross( kDiff );

    afAWxDdU[0] = fabs(kWxD.Dot(m_Box.axes[0]));
    fRhs = m_Box.extents[1]*afAWdU[2] + m_Box.extents[2]*afAWdU[1];
    if (afAWxDdU[0] > fRhs)
    {
        return false;
    }

    afAWxDdU[1] = fabs(kWxD.Dot(m_Box.axes[1]));
    fRhs = m_Box.extents[0]*afAWdU[2] + m_Box.extents[2]*afAWdU[0];
    if (afAWxDdU[1] > fRhs)
    {
        return false;
    }

    afAWxDdU[2] = fabs(kWxD.Dot(m_Box.axes[2]));
    fRhs = m_Box.extents[0]*afAWdU[1] + m_Box.extents[1]*afAWdU[0];
    if (afAWxDdU[2] > fRhs)
    {
        return false;
    }

    return( true );
}
//--------------------------------------------------------------------------------
bool IntrRay3fBox3f::Find()
{
	float fT0 = 0.0f;
	float fT1 = 10000000000.0f;

    // convert linear component to box coordinates
    Vector3f kDiff = m_Ray.origin - m_Box.center;
    
	Vector3f kBOrigin(
        kDiff.Dot(m_Box.axes[0]),
        kDiff.Dot(m_Box.axes[1]),
        kDiff.Dot(m_Box.axes[2])
    );

    Vector3f kBDirection(
        m_Ray.direction.Dot(m_Box.axes[0]),
        m_Ray.direction.Dot(m_Box.axes[1]),
        m_Ray.direction.Dot(m_Box.axes[2])
    );

    float fSaveT0 = fT0, fSaveT1 = fT1;

	bool bNotAllClipped =
        Clip(+kBDirection.x,-kBOrigin.x-m_Box.extents[0],fT0,fT1) &&
        Clip(-kBDirection.x,+kBOrigin.x-m_Box.extents[0],fT0,fT1) &&
        Clip(+kBDirection.y,-kBOrigin.y-m_Box.extents[1],fT0,fT1) &&
        Clip(-kBDirection.y,+kBOrigin.y-m_Box.extents[1],fT0,fT1) &&
        Clip(+kBDirection.z,-kBOrigin.z-m_Box.extents[2],fT0,fT1) &&
        Clip(-kBDirection.z,+kBOrigin.z-m_Box.extents[2],fT0,fT1);

    if ( bNotAllClipped && ( true || fT0 != fSaveT0 || fT1 != fSaveT1 ) )
    {
        if ( fT1 > fT0 )
        {
            //riIntrType = IT_SEGMENT;
            m_iQuantity = 2;
			m_afRayT[0] = fT0;
			m_afRayT[1] = fT1;
            m_aPoints[0] = m_Ray.origin + m_Ray.direction * fT0;
            m_aPoints[1] = m_Ray.origin + m_Ray.direction * fT1;
        }
        else
        {
            //riIntrType = IT_POINT;
            m_iQuantity = 1;
			m_afRayT[0] = fT0;
            m_aPoints[0] = m_Ray.origin + m_Ray.direction * fT0;
        }
    }
    else
    {
        m_iQuantity = 0;
        //riIntrType = IT_EMPTY;
    }

    return( m_iQuantity > 0 );
}
//--------------------------------------------------------------------------------
bool IntrRay3fBox3f::Clip ( float fDenom, float fNumer, float& rfT0, float& rfT1 )
{
    // Return value is 'true' if line segment intersects the current test
    // plane.  Otherwise 'false' is returned in which case the line segment
    // is entirely clipped.

    if ( fDenom > 0.0f )
    {
        if ( fNumer > fDenom * rfT1 )
        {
            return false;
        }
        if ( fNumer > fDenom * rfT0 )
        {
            rfT0 = fNumer / fDenom;
        }
        return true;
    }
    else if ( fDenom < 0.0f )
    {
        if ( fNumer > fDenom * rfT0 )
        {
            return false;
        }
        if ( fNumer > fDenom * rfT1 )
        {
            rfT1 = fNumer / fDenom;
        }
        return true;
    }
    else
    {
        return fNumer <= 0.0f;
    }
}
//--------------------------------------------------------------------------------