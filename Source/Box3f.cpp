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
#include "Box3f.h"
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
Box3f::Box3f() :
center(),
axes(),
extents()
{
}
//--------------------------------------------------------------------------------
Box3f::Box3f( const Vector3f& Center, const Vector3f& Forward, const Vector3f& Up, 
			const Vector3f& Right, float fextents, float uextents, float rextents )
			//center( Center),
			//axes{ forward, up, right },
			//extents{ fextents, uextents, rextents }
{
	// TODO: The initializer list constructor is not implemented in VS2013. I need
	//       to check if this is implemented in VS2015.  Till then, we use direct
	//       initialization.
	center = Center;

	axes[0] = Forward;
	axes[1] = Up;
	axes[2] = Right;

	extents[0] = fextents;
	extents[1] = uextents;
	extents[2] = rextents;
}
//--------------------------------------------------------------------------------
Box3f::~Box3f()
{
}
//--------------------------------------------------------------------------------