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
#include "CompositeShape.h"
#include "Sphere3f.h"
#include "Box3f.h"
#include "Frustum3f.h"
#include "Plane3f.h"
#include "Ray3f.h"
#include "Triangle3f.h"
#include "IntrRay3fSphere3f.h"
#include "IntrRay3fBox3f.h"
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
CompositeShape::CompositeShape( )
{
}
//--------------------------------------------------------------------------------
CompositeShape::~CompositeShape()
{
}
//--------------------------------------------------------------------------------
void CompositeShape::AddSphere( const Sphere3f& sphere )
{
	m_spheres.push_back( sphere );
}
//--------------------------------------------------------------------------------
bool CompositeShape::RayIntersection( const Ray3f& ray, float* fDist )
{
	float fMin = 10000000000.0f;
	bool bHit = false;

	for ( const auto& sphere : m_spheres )
	{ 
		IntrRay3fSphere3f Intr( ray, sphere );
		if ( Intr.Test() )
		{
			bHit = true;

			Intr.Find();
			for ( int j = 0; j < Intr.m_iQuantity; j++ )
			{
				if ( Intr.m_afRayT[j] < *fDist )
					*fDist = Intr.m_afRayT[j];
			}
		}
	}

	//			break;
	//		}

	//	case BOX:
	//		{
	//			Box3f* pBox = (Box3f*)pShape;
	//			IntrRay3fBox3f Intr( ray, *pBox );
	//			if ( Intr.Test() )
	//			{
	//				bHit = true;

	//				Intr.Find();
	//				for ( int j = 0; j < Intr.m_iQuantity; j++ )
	//				{
	//					if ( Intr.m_afRayT[j] < *fDist )
	//						*fDist = Intr.m_afRayT[j];
	//				}
	//			}
	//			
	//			break;
	//		}


	return( bHit );

}
//--------------------------------------------------------------------------------
int CompositeShape::GetNumberOfShapes() const
{
	return( m_spheres.size() );
}
//--------------------------------------------------------------------------------