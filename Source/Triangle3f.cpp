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
#include "Triangle3f.h"
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
Triangle3f::Triangle3f()
{
}
//--------------------------------------------------------------------------------
Triangle3f::Triangle3f( const Vector3f& P1, const Vector3f& P2, const Vector3f& P3 )
{
	p1 = P1;
	p2 = P2;
	p3 = P3;
}
//--------------------------------------------------------------------------------
Triangle3f::~Triangle3f()
{
}
//--------------------------------------------------------------------------------
