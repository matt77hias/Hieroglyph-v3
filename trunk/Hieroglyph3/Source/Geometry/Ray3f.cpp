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
#include "Ray3f.h"
#include <math.h>
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
Ray3f::Ray3f()
{
}
//--------------------------------------------------------------------------------
Ray3f::Ray3f( Vector3f& position, Vector3f& direction )
{
	Origin = position;
	Direction = direction;
}
//--------------------------------------------------------------------------------
Ray3f::~Ray3f()
{
}
//--------------------------------------------------------------------------------
eSHAPE Ray3f::GetShapeType()
{
	return( RAY );
}
//--------------------------------------------------------------------------------