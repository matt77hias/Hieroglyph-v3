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
#include "LineIndices.h"
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
LineIndices::LineIndices()
{
	i1 = 0;
	i2 = 0;
}
//--------------------------------------------------------------------------------
LineIndices::LineIndices( unsigned int I1, unsigned int I2 )
{
	i1 = I1;
	i2 = I2;
}
//--------------------------------------------------------------------------------
LineIndices::~LineIndices()
{
}
//--------------------------------------------------------------------------------
LineIndices& LineIndices::operator= ( const LineIndices& Line )
{
	i1 = Line.i1;
	i2 = Line.i2;

	return( *this );
}
//--------------------------------------------------------------------------------
void LineIndices::swap12( )
{
	unsigned int swap = i1;
	i1 = i2;
	i2 = swap;
}
//--------------------------------------------------------------------------------
