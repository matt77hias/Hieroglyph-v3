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
// Box3f
//
//--------------------------------------------------------------------------------
#ifndef Box3f_h
#define Box3f_h
//--------------------------------------------------------------------------------
#include "Shape3D.h"
#include "Vector3f.h"
//--------------------------------------------------------------------------------
namespace Glyph3
{
	class Box3f : public Shape3D
	{
	public:
		Box3f( );
		Box3f( const Vector3f& center, const Vector3f& forward, const Vector3f& up, 
			const Vector3f& right, float fextents, float uextents, float rextents );
		virtual ~Box3f( );

		virtual eSHAPE GetShapeType( ) const;

	public:
		Vector3f Center;

		Vector3f Axis[3];
		float Extent[3];
	};
};
//--------------------------------------------------------------------------------
#endif // Box3f_h
