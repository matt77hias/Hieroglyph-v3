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
// Shape3D
//
//--------------------------------------------------------------------------------
#ifndef Shape3D_h
#define Shape3D_h
//--------------------------------------------------------------------------------
#include "Vector3f.h"
//--------------------------------------------------------------------------------
namespace Glyph3
{
	enum eSHAPE
	{
		SPHERE,
		BOX,
		TRIANGLE,
		RAY,
		PLANE,
		FRUSTUM,
		CONE
	};

	class Shape3D
	{
	public:
		Shape3D( );
		virtual ~Shape3D( );

		virtual eSHAPE GetShapeType( ) const = 0;
	};
};
//--------------------------------------------------------------------------------
#endif // Shape3D_h
