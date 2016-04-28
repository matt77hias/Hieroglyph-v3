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
#include "Vector3f.h"
//--------------------------------------------------------------------------------
namespace Glyph3
{
	struct Triangle3f
	{
		Triangle3f( );
		Triangle3f( const Vector3f& P1, const Vector3f& P2, const Vector3f& P3 );
		~Triangle3f( );

		Vector3f p1;
		Vector3f p2;
		Vector3f p3;
	};
};
//--------------------------------------------------------------------------------
#endif // Triangle3f_h
//--------------------------------------------------------------------------------