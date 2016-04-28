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
#include "AxisAlignedBox.h"
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
AxisAlignedBox::AxisAlignedBox() :
	minimums(0.0f,0.0f,0.0f),
	maximums(0.0f,0.0f,0.0f)
{
}
//--------------------------------------------------------------------------------
AxisAlignedBox::AxisAlignedBox( const Vector3f& mins, const Vector3f& maxs ) :
	minimums( mins ),
	maximums( maxs )
{
}
//--------------------------------------------------------------------------------
AxisAlignedBox::~AxisAlignedBox()
{
}
//--------------------------------------------------------------------------------
bool AxisAlignedBox::contains(const Vector3f& p)
{
	if (minimums.x < p.x && p.x < maximums.x &&
		minimums.y < p.y && p.y < maximums.y &&
		minimums.z < p.z && p.z < maximums.z) 
	{
		return true;
	}
	else
	{
		return false;
	}
}
//--------------------------------------------------------------------------------
void AxisAlignedBox::expand_to_point(const Vector3f& p)
{
	if (p.x > maximums.x) maximums.x = p.x;
	if (p.y > maximums.y) maximums.y = p.y;
	if (p.z > maximums.z) maximums.z = p.z;

	if (p.x < minimums.x) minimums.x = p.x;
	if (p.y < minimums.y) minimums.y = p.y;
	if (p.z < minimums.z) minimums.z = p.z;
}
//--------------------------------------------------------------------------------
