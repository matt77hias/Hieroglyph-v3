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
// PointIndices 
//
// This class represents the single index of a point.
//--------------------------------------------------------------------------------
#ifndef PointIndices_h
#define PointIndices_h
//--------------------------------------------------------------------------------
namespace Glyph3
{
	class PointIndices
	{
	public:
		PointIndices();
		PointIndices( unsigned int P1 );
		~PointIndices();

		PointIndices& operator= ( const PointIndices& Point );

		// member access
		unsigned int P1( ) const;
		unsigned int& P1( );

	protected:
		unsigned int m_uiIndices[1];
	};
};
//--------------------------------------------------------------------------------
#endif // PointIndices_h
