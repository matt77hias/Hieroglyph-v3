//--------------------------------------------------------------------------------
// This file is a portion of the Hieroglyph 3 Rendering Engine.  It is distributed
// under the MIT License, available in the root of this distribution and 
// at the following URL:
//
// http://www.opensource.org/licenses/mit-license.php
//
// Copyright (c) 2003-2010 Jason Zink 
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

		TriangleIndices& operator= ( const TriangleIndices& Triangle );

		// member access
		unsigned int P1( ) const;
		unsigned int& P1( );
		unsigned int P2( ) const;
		unsigned int& P2( );
		unsigned int P3( ) const;
		unsigned int& P3( );

		void swapP1P2( );
		void swapP2P3( );
		void swapP3P1( );

	protected:
		unsigned int m_uiIndices[3];
	};
};
//--------------------------------------------------------------------------------
#endif // TriangleIndices_h
