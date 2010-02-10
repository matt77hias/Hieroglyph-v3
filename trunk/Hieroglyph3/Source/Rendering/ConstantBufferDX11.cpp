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
#include "ConstantBufferDX11.h"
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
ConstantBufferDX11::ConstantBufferDX11( ID3D11Buffer* pBuffer )
{
	m_pBuffer = pBuffer;
}
//--------------------------------------------------------------------------------
ConstantBufferDX11::~ConstantBufferDX11()
{
	// Buffer is released in the BufferDX11 destructor
}
//--------------------------------------------------------------------------------
void ConstantBufferDX11::SetConstantSize( int size )
{
	m_iConstantSize = size;
}
//--------------------------------------------------------------------------------
void ConstantBufferDX11::SetConstantCount( int count )
{
	m_iConstantCount = count;
}
//--------------------------------------------------------------------------------