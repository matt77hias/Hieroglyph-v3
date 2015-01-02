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
// Ray3f
//
//--------------------------------------------------------------------------------
#ifndef Ray3f_h
#define Ray3f_h
//--------------------------------------------------------------------------------
#include "Shape3D.h"
#include "Vector3f.h"
//--------------------------------------------------------------------------------
namespace Glyph3
{
	class Ray3f : public Shape3D
	{
	public:
		Ray3f( );
		Ray3f( const Vector3f& position, const Vector3f& direction );
		virtual ~Ray3f( );

		virtual eSHAPE GetShapeType( ) const;

	public:
		Vector3f Origin;
		Vector3f Direction;
	};
};
//--------------------------------------------------------------------------------
#endif // Ray3f_h
