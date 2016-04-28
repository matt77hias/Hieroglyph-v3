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
#include "PointIndices.h"
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
PointIndices::PointIndices()
{
	i1 = 0;
}
//--------------------------------------------------------------------------------
PointIndices::PointIndices( unsigned int I1 )
{
	i1 = I1;
}
//--------------------------------------------------------------------------------
PointIndices::~PointIndices()
{
}
//--------------------------------------------------------------------------------
