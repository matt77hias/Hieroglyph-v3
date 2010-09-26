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
#include "PCH.h"
#include "Box3f.h"
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
Box3f::Box3f()
{
}
//--------------------------------------------------------------------------------
Box3f::Box3f( Vector3f& center, Vector3f& forward, Vector3f& up, 
			Vector3f& right, float fextents, float uextents, float rextents )
{
	Center = center;

	Axis[0] = forward;
	Axis[1] = up;
	Axis[2] = right;

	Extent[0] = fextents;
	Extent[1] = uextents;
	Extent[2] = rextents;
}
//--------------------------------------------------------------------------------
Box3f::~Box3f()
{
}
//--------------------------------------------------------------------------------
eSHAPE Box3f::GetShapeType( )
{
	return( BOX );
}
//--------------------------------------------------------------------------------