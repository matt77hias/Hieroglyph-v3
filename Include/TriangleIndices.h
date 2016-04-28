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
// TriangleIndices 
//
// This class represents the three indices of a triangle.
//--------------------------------------------------------------------------------
#ifndef TriangleIndices_h
#define TriangleIndices_h
//--------------------------------------------------------------------------------
namespace Glyph3
{
	class TriangleIndices
	{
	public:
		TriangleIndices();
		TriangleIndices( unsigned int P1, unsigned int P2, unsigned int P3 );
		~TriangleIndices();

		void swap12( );
		void swap23( );
		void swap31( );

		unsigned int i1;
		unsigned int i2;
		unsigned int i3;
	};
};
//--------------------------------------------------------------------------------
#endif // TriangleIndices_h
//--------------------------------------------------------------------------------