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
#include "Ray3f.h"
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
Ray3f::Ray3f()
{
}
//--------------------------------------------------------------------------------
Ray3f::Ray3f( const Vector3f& Position, const Vector3f& Direction )
{
	origin = Position;
	direction = Direction;
}
//--------------------------------------------------------------------------------
Ray3f::~Ray3f()
{
}
//--------------------------------------------------------------------------------