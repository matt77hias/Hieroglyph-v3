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
// Triangle3f
//
//--------------------------------------------------------------------------------
#ifndef Triangle3f_h
#define Triangle3f_h
//--------------------------------------------------------------------------------
#include "Shape3D.h"
#include "Vector3f.h"
//--------------------------------------------------------------------------------
namespace Glyph3
{
	class Triangle3f : public Shape3D
	{
	public:
		Triangle3f( );
		Triangle3f( const Vector3f& v1, const Vector3f& v2, const Vector3f& v3 );
		virtual ~Triangle3f( );

		virtual eSHAPE GetShapeType( ) const;

	protected:
		Vector3f v[3];
	};
};
//--------------------------------------------------------------------------------
#endif // Triangle3f_h
