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
	for ( auto pShape : m_Shapes )
		delete pShape;
}
//--------------------------------------------------------------------------------
void CompositeShape::AddShape( Shape3D* pShape )
{
	if ( pShape )
		m_Shapes.push_back( pShape );
}
//--------------------------------------------------------------------------------
CompositeShape* CompositeShape::DeepCopy( )
{
	CompositeShape* pCopy = new CompositeShape();

	for ( auto pShape : m_Shapes )
	{
		// Get the shape type
		eSHAPE type = pShape->GetShapeType();

		// Create and copy the shape, then add it to the copied composite
		switch( type )
		{
		case SPHERE:
			{
				Sphere3f* pSphere = new Sphere3f();
				*pSphere = *(Sphere3f*)pShape;
				pCopy->AddShape( pSphere );
				
				break;
			}

		case BOX:
			{
				Box3f* pBox = new Box3f();
				*pBox = *(Box3f*)pShape;
				pCopy->AddShape( pBox );
				
				break;
			}

		case FRUSTUM:
			{
				Frustum3f* pFrustum = new Frustum3f();
				*pFrustum = *(Frustum3f*)pShape;
				pCopy->AddShape( pFrustum );
				
				break;
			}

		case PLANE:
			{
				Plane3f* pPlane = new Plane3f();
				*pPlane = *(Plane3f*)pShape;
				pCopy->AddShape( pPlane );
				
				break;
			}

		case RAY:
			{
				Ray3f* pRay = new Ray3f();
				*pRay = *(Ray3f*)pShape;
				pCopy->AddShape( pRay );
				
				break;
			}

		case TRIANGLE:
			{
				Triangle3f* pTriangle = new Triangle3f();
				*pTriangle = *(Triangle3f*)pShape;
				pCopy->AddShape( pTriangle );
				
				break;
			}
		}
	}

	return( pCopy );
}
//--------------------------------------------------------------------------------
bool CompositeShape::RayIntersection( const Ray3f& ray, float* fDist )
{
	float fMin = 10000000000.0f;
	bool bHit = false;

	for ( auto pShape : m_Shapes )
	{
		eSHAPE type = pShape->GetShapeType();

		switch( type )
		{
		case SPHERE:
			{
				Sphere3f* pSphere = (Sphere3f*)pShape;
				IntrRay3fSphere3f Intr( ray, *pSphere );
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

				break;
			}

		case BOX:
			{
				Box3f* pBox = (Box3f*)pShape;
				IntrRay3fBox3f Intr( ray, *pBox );
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
				
				break;
			}

		case FRUSTUM:
			{
				
				break;
			}

		case PLANE:
			{
				
				break;
			}

		case RAY:
			{
				
				break;
			}

		case TRIANGLE:
			{
				
				break;
			}
		}
	}

	return( bHit );

}
//--------------------------------------------------------------------------------
int CompositeShape::GetNumberOfShapes() const
{
	return( m_Shapes.size() );
}
//--------------------------------------------------------------------------------