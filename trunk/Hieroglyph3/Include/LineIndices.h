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
	class LineIndices
	{
	public:
		LineIndices();
		LineIndices( unsigned int P1, unsigned int P2 );
		~LineIndices();

		LineIndices& operator= ( const LineIndices& Line );

		// member access
		unsigned int P1( ) const;
		unsigned int& P1( );
		unsigned int P2( ) const;
		unsigned int& P2( );

		void swapP1P2( );

	protected:
		unsigned int m_uiIndices[2];
	};
};
//--------------------------------------------------------------------------------
#endif // LineIndices_h
