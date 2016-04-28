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
#include "TriangleIndices.h"
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
TriangleIndices::TriangleIndices()
{
	i1 = 0;
	i2 = 0;
	i3 = 0;
}
//--------------------------------------------------------------------------------
TriangleIndices::TriangleIndices(unsigned int P1, unsigned int P2, unsigned int P3)
{
	i1 = P1;
	i2 = P2;
	i3 = P3;
}
//--------------------------------------------------------------------------------
TriangleIndices::~TriangleIndices()
{
}
//--------------------------------------------------------------------------------
void TriangleIndices::swap12( )
{
	unsigned int swap = i1;
	i1 = i2;
	i2 = swap;
}
//--------------------------------------------------------------------------------
void TriangleIndices::swap23( )
{
	unsigned int swap = i2;
	i2 = i3;
	i3 = swap;
}
//--------------------------------------------------------------------------------
void TriangleIndices::swap31( )
{
	unsigned int swap = i3;
	i3 = i1;
	i1 = swap;
}
//--------------------------------------------------------------------------------