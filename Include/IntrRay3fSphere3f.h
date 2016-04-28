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
// IntrRay3fSphere3f
//
// This class is based off of the WildMagic engine by Dave Eberly.  Check it out
// at http://www.geometrictools.com, or check out one of his books.
//--------------------------------------------------------------------------------
#ifndef IntrRay3fSphere3f_h
#define IntrRay3fSphere3f_h
//--------------------------------------------------------------------------------
#include "Intersector.h"
#include "Ray3f.h"
#include "Sphere3f.h"
//--------------------------------------------------------------------------------
namespace Glyph3
{
	class IntrRay3fSphere3f : public Intersector
	{
	public:
		IntrRay3fSphere3f( const Ray3f& ray, const Sphere3f& sphere );
		virtual ~IntrRay3fSphere3f( );
	
		virtual bool Test();
		virtual bool Find();

	public:
		Ray3f			m_Ray;
		Sphere3f		m_Sphere;

		Vector3f		m_aPoints[2];
		float			m_afRayT[2];
		int				m_iQuantity;
	};
};
//--------------------------------------------------------------------------------
#endif // IntrRay3fSphere3f_h
