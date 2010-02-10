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
// CompositeShape
//
//--------------------------------------------------------------------------------
#ifndef CompositeShape_h
#define CompositeShape_h
//--------------------------------------------------------------------------------
#include "Shape3D.h"
#include "Ray3f.h"
#include "TArray.h"
//--------------------------------------------------------------------------------
namespace Glyph3
{
	class CompositeShape
	{
	public:
		CompositeShape( );
		virtual ~CompositeShape( );

		CompositeShape* DeepCopy( );

		void AddShape( Shape3D* pShape );
		bool RayIntersection( Ray3f pRay, float* fDist );

		int GetNumberOfShapes();

	protected:
		TArray< Shape3D* > m_Shapes;
	};
};
//--------------------------------------------------------------------------------
#endif // CompositeShape_h
