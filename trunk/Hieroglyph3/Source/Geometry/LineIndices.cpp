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
#include "LineIndices.h"
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
LineIndices::LineIndices()
{
	m_uiIndices[0] = 0;
	m_uiIndices[1] = 0;
}
//--------------------------------------------------------------------------------
LineIndices::LineIndices( unsigned int P1, unsigned int P2 )
{
	m_uiIndices[0] = P1;
	m_uiIndices[1] = P2;
}
//--------------------------------------------------------------------------------
LineIndices::~LineIndices()
{
}
//--------------------------------------------------------------------------------
LineIndices& LineIndices::operator= ( const LineIndices& Line )
{
	m_uiIndices[0] = Line.m_uiIndices[0];
	m_uiIndices[1] = Line.m_uiIndices[1];

	return( *this );
}
//--------------------------------------------------------------------------------
unsigned int LineIndices::P1() const
{
	return( m_uiIndices[0] );
}
//--------------------------------------------------------------------------------
unsigned int& LineIndices::P1()
{
	return( m_uiIndices[0] );
}
//--------------------------------------------------------------------------------
unsigned int LineIndices::P2() const
{
	return( m_uiIndices[1] );
}
//--------------------------------------------------------------------------------
unsigned int& LineIndices::P2()
{
	return( m_uiIndices[1] );
}
//--------------------------------------------------------------------------------
void LineIndices::swapP1P2( )
{
	unsigned int swap = m_uiIndices[0];
	m_uiIndices[0] = m_uiIndices[1];
	m_uiIndices[1] = swap;
}
//--------------------------------------------------------------------------------
