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
// LineIndices 
//
// This class represents the two indices of a line.
//--------------------------------------------------------------------------------
#ifndef LineIndices_h
#define LineIndices_h
//--------------------------------------------------------------------------------
namespace Glyph3
{
	struct LineIndices
	{
		LineIndices();
		LineIndices( unsigned int I1, unsigned int I2 );
		~LineIndices();

		LineIndices& operator= ( const LineIndices& Line );

		void swap12( );

		unsigned int i1;
		unsigned int i2;
	};
};
//--------------------------------------------------------------------------------
#endif // LineIndices_h
//--------------------------------------------------------------------------------