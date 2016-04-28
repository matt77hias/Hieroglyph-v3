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
// AxisAlignedBox
//
//--------------------------------------------------------------------------------
#ifndef AxisAlignedBox_h
#define AxisAlignedBox_h
//--------------------------------------------------------------------------------
#include "Vector3f.h"
//--------------------------------------------------------------------------------
namespace Glyph3
{
	struct AxisAlignedBox
	{
	public:
		AxisAlignedBox( );
		AxisAlignedBox( const Vector3f& mins, const Vector3f& maxs );
		~AxisAlignedBox( );

		bool contains(const Vector3f& p);
		void expand_to_point(const Vector3f& p);

		Vector3f minimums;
		Vector3f maximums;
	};
};
//--------------------------------------------------------------------------------
#endif // AxisAlignedBox_h
//--------------------------------------------------------------------------------