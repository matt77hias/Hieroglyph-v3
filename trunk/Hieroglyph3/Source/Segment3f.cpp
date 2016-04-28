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
#include "Segment3f.h"
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
Segment3f::Segment3f()
{
}
//--------------------------------------------------------------------------------
Segment3f::Segment3f( const Vector3f& P1, const Vector3f& P2 )
{
	p1 = P1;
	p2 = P2;
}
//--------------------------------------------------------------------------------
Segment3f::~Segment3f()
{
}
//--------------------------------------------------------------------------------
void Segment3f::Reverse()
{
	Vector3f temp = p1;
	p1 = p2;
	p2 = temp;
}
//--------------------------------------------------------------------------------
