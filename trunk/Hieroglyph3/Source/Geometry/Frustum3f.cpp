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
#include "Frustum3f.h"
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
Frustum3f::Frustum3f()
{
	for (int i = 0; i < 6; i++)
	{
		m_Planes[i].a() = 0.0f;
		m_Planes[i].b() = 0.0f;
		m_Planes[i].c() = 0.0f;
		m_Planes[i].d() = 0.0f;
	}
}
//--------------------------------------------------------------------------------
Frustum3f::Frustum3f(Matrix4f& ViewProjection)
{
	Update(ViewProjection, true);
}
//--------------------------------------------------------------------------------
Frustum3f::~Frustum3f()
{
}
//--------------------------------------------------------------------------------
void Frustum3f::Update(Matrix4f& ViewProj, bool bNormalize)
{
	// Left Plane
	m_Planes[0].a() = ViewProj(0,3) + ViewProj(0,0);
	m_Planes[0].b() = ViewProj(1,3) + ViewProj(1,0);
	m_Planes[0].c() = ViewProj(2,3) + ViewProj(2,0);
	m_Planes[0].d() = ViewProj(3,3) + ViewProj(3,0);

	// Right Plane
	m_Planes[1].a() = ViewProj(0,3) - ViewProj(0,0);
	m_Planes[1].b() = ViewProj(1,3) - ViewProj(1,0);
	m_Planes[1].c() = ViewProj(2,3) - ViewProj(2,0);
	m_Planes[1].d() = ViewProj(3,3) - ViewProj(3,0);

    // Top Plane
	m_Planes[2].a() = ViewProj(0,3) - ViewProj(0,1);
	m_Planes[2].b() = ViewProj(1,3) - ViewProj(1,1);
	m_Planes[2].c() = ViewProj(2,3) - ViewProj(2,1);
	m_Planes[2].d() = ViewProj(3,3) - ViewProj(3,1);

	// Bottom Plane
	m_Planes[3].a() = ViewProj(0,3) + ViewProj(0,1);
	m_Planes[3].b() = ViewProj(1,3) + ViewProj(1,1);
	m_Planes[3].c() = ViewProj(2,3) + ViewProj(2,1);
	m_Planes[3].d() = ViewProj(3,3) + ViewProj(3,1);

	// Near Plane
	m_Planes[4].a() = ViewProj(0,2);
	m_Planes[4].b() = ViewProj(1,2);
	m_Planes[4].c() = ViewProj(2,2);
	m_Planes[4].d() = ViewProj(3,2);

	// Far Plane
	m_Planes[5].a() = ViewProj(0,3) - ViewProj(0,2);
	m_Planes[5].b() = ViewProj(1,3) - ViewProj(1,2);
	m_Planes[5].c() = ViewProj(2,3) - ViewProj(2,2);
	m_Planes[5].d() = ViewProj(3,3) - ViewProj(3,2);


	//// Left Plane
	//m_Planes[0].a() = ViewProj(1,4) + ViewProj(1,1);
	//m_Planes[0].b() = ViewProj(2,4) + ViewProj(2,1);
	//m_Planes[0].c() = ViewProj(3,4) + ViewProj(3,1);
	//m_Planes[0].d() = ViewProj(4,4) + ViewProj(4,1);

	//// Right Plane
	//m_Planes[1].a() = ViewProj(1,4) - ViewProj(1,1);
	//m_Planes[1].b() = ViewProj(2,4) - ViewProj(2,1);
	//m_Planes[1].c() = ViewProj(3,4) - ViewProj(3,1);
	//m_Planes[1].d() = ViewProj(4,4) - ViewProj(4,1);

 //   // Top Plane
	//m_Planes[2].a() = ViewProj(1,4) - ViewProj(1,2);
	//m_Planes[2].b() = ViewProj(2,4) - ViewProj(2,2);
	//m_Planes[2].c() = ViewProj(3,4) - ViewProj(3,2);
	//m_Planes[2].d() = ViewProj(4,4) - ViewProj(4,2);

	//// Bottom Plane
	//m_Planes[3].a() = ViewProj(1,4) + ViewProj(1,2);
	//m_Planes[3].b() = ViewProj(2,4) + ViewProj(2,2);
	//m_Planes[3].c() = ViewProj(3,4) + ViewProj(3,2);
	//m_Planes[3].d() = ViewProj(4,4) + ViewProj(4,2);

	//// Near Plane
	//m_Planes[4].a() = ViewProj(1,3);
	//m_Planes[4].b() = ViewProj(2,3);
	//m_Planes[4].c() = ViewProj(3,3);
	//m_Planes[4].d() = ViewProj(4,3);

	//// Far Plane
	//m_Planes[5].a() = ViewProj(1,4) - ViewProj(1,3);
	//m_Planes[5].b() = ViewProj(2,4) - ViewProj(2,3);
	//m_Planes[5].c() = ViewProj(3,4) - ViewProj(3,3);
	//m_Planes[5].d() = ViewProj(4,4) - ViewProj(4,3);


	// Normalize all planes
	if (bNormalize)
	{
		for (int i = 0; i < 6; i++)
		{
			m_Planes[i].Normalize();
		}
	}
}
//--------------------------------------------------------------------------------
bool Frustum3f::Test(Vector3f& TestPoint)
{
	// Test the point against each plane
	if (m_Planes[0].DistanceToPoint(TestPoint) < 0)
		return(false);

	if (m_Planes[1].DistanceToPoint(TestPoint) < 0)
		return(false);

	if (m_Planes[2].DistanceToPoint(TestPoint) < 0)
		return(false);

	if (m_Planes[3].DistanceToPoint(TestPoint) < 0)
		return(false);

	if (m_Planes[4].DistanceToPoint(TestPoint) < 0)
		return(false);

	if (m_Planes[5].DistanceToPoint(TestPoint) < 0)
		return(false);

	// If all tests passed, point is within the frustum
	return(true);
}
//--------------------------------------------------------------------------------
bool Frustum3f::Test(Sphere3f& TestSphere)
{
	// Test the center against each plane and compare the radius
	
	float fTemp = 0.0f;
	
	for (int i = 0; i < 6; i++)
	{
		fTemp = m_Planes[i].DistanceToPoint( TestSphere.Center );
		if ( fTemp < -TestSphere.Radius )
			return( false );

		if ( float(fabs(fTemp)) < TestSphere.Radius )
			return( true );
	}

	// If all tests passed, sphere is at least intersecting the frustum
	return( true );
}
//--------------------------------------------------------------------------------
bool Frustum3f::Intersects( Sphere3f& bounds )
{
	// distance to point plus the radius must be greater than zero to be intersecting!!!

	for ( int i = 0; i < 6; i++ )
	{
		if ( m_Planes[i].DistanceToPoint( bounds.Center ) + bounds.Radius < 0)
			return( false );
	}

	// must not be enveloped if it is intersecting

	//if ( Envelops( bounds ) )
	//	return( false );

	// if all tests passed, then sphere is enveloped

	return( true );
}
//--------------------------------------------------------------------------------
bool Frustum3f::Envelops( Sphere3f& bounds )
{
	// distance to point minus the radius must be greater than zero to be enveloped!!!

	for ( int i = 0; i < 6; i++ )
	{
		if (m_Planes[i].DistanceToPoint( bounds.Center ) - bounds.Radius < 0)
			return( false );
	}

	// If all tests passed, sphere is enveloped

	return( true );
}
//--------------------------------------------------------------------------------
eSHAPE Frustum3f::GetShapeType( )
{
	return( FRUSTUM );
}
//--------------------------------------------------------------------------------