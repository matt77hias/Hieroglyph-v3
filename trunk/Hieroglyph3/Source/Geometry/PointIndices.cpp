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
#include "PCH.h"
#include "PointIndices.h"
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
PointIndices::PointIndices()
{
	m_uiIndices[0] = 0;
}
//--------------------------------------------------------------------------------
PointIndices::PointIndices( unsigned int P1 )
{
	m_uiIndices[0] = P1;
}
//--------------------------------------------------------------------------------
PointIndices::~PointIndices()
{
}
//--------------------------------------------------------------------------------
PointIndices& PointIndices::operator= (const PointIndices& Point)
{
	m_uiIndices[0] = Point.m_uiIndices[0];

	return( *this );
}
//--------------------------------------------------------------------------------
unsigned int PointIndices::P1() const
{
	return( m_uiIndices[0] );
}
//--------------------------------------------------------------------------------
unsigned int& PointIndices::P1()
{
	return( m_uiIndices[0] );
}
//--------------------------------------------------------------------------------
