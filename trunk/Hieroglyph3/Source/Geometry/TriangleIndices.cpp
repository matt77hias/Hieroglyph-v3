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
#include "TriangleIndices.h"
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
TriangleIndices::TriangleIndices()
{
	m_uiIndices[0] = 0;
	m_uiIndices[1] = 0;
	m_uiIndices[2] = 0;
}
//--------------------------------------------------------------------------------
TriangleIndices::TriangleIndices(unsigned int P1, unsigned int P2, unsigned int P3)
{
	m_uiIndices[0] = P1;
	m_uiIndices[1] = P2;
	m_uiIndices[2] = P3;
}
//--------------------------------------------------------------------------------
TriangleIndices::~TriangleIndices()
{
}
//--------------------------------------------------------------------------------
TriangleIndices& TriangleIndices::operator= (const TriangleIndices& Triangle)
{
	m_uiIndices[0] = Triangle.m_uiIndices[0];
	m_uiIndices[1] = Triangle.m_uiIndices[1];
	m_uiIndices[2] = Triangle.m_uiIndices[2];

	return *this;
}
//--------------------------------------------------------------------------------
unsigned int TriangleIndices::P1() const
{
	return(m_uiIndices[0]);
}
//--------------------------------------------------------------------------------
unsigned int& TriangleIndices::P1()
{
	return(m_uiIndices[0]);
}
//--------------------------------------------------------------------------------
unsigned int TriangleIndices::P2() const
{
	return(m_uiIndices[1]);
}
//--------------------------------------------------------------------------------
unsigned int& TriangleIndices::P2()
{
	return(m_uiIndices[1]);
}
//--------------------------------------------------------------------------------
unsigned int TriangleIndices::P3() const
{
	return(m_uiIndices[2]);
}
//--------------------------------------------------------------------------------
unsigned int& TriangleIndices::P3()
{
	return(m_uiIndices[2]);
}
//--------------------------------------------------------------------------------
void TriangleIndices::swapP1P2( )
{
	unsigned int swap = m_uiIndices[0];
	m_uiIndices[0] = m_uiIndices[1];
	m_uiIndices[1] = swap;
}
//--------------------------------------------------------------------------------
void TriangleIndices::swapP2P3( )
{
	unsigned int swap = m_uiIndices[1];
	m_uiIndices[1] = m_uiIndices[2];
	m_uiIndices[2] = swap;
}
//--------------------------------------------------------------------------------
void TriangleIndices::swapP3P1( )
{
	unsigned int swap = m_uiIndices[2];
	m_uiIndices[2] = m_uiIndices[0];
	m_uiIndices[0] = swap;
}
//--------------------------------------------------------------------------------