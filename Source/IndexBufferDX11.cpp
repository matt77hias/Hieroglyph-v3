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
#include "PCH.h"
#include "IndexBufferDX11.h"
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
IndexBufferDX11::IndexBufferDX11( Microsoft::WRL::ComPtr<ID3D11Buffer> pBuffer )
{
	m_pBuffer = pBuffer;
}
//--------------------------------------------------------------------------------
IndexBufferDX11::~IndexBufferDX11()
{
	// Buffer is released in the BufferDX11 destructor
}
//--------------------------------------------------------------------------------
ResourceType IndexBufferDX11::GetType()
{
	return( RT_INDEXBUFFER );
}
//--------------------------------------------------------------------------------
void IndexBufferDX11::SetIndexSize( int size )
{
	m_iIndexSize = size;
}
//--------------------------------------------------------------------------------
void IndexBufferDX11::SetIndexCount( int count )
{
	m_iIndexCount = count;
}
//--------------------------------------------------------------------------------