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
// PointIndices 
//
// This class represents the single index of a point.
//--------------------------------------------------------------------------------
#ifndef PointIndices_h
#define PointIndices_h
//--------------------------------------------------------------------------------
namespace Glyph3
{
	struct PointIndices
	{
		PointIndices();
		PointIndices( unsigned int I1 );
		~PointIndices();

		unsigned int i1;
	};
};
//--------------------------------------------------------------------------------
#endif // PointIndices_h
//--------------------------------------------------------------------------------