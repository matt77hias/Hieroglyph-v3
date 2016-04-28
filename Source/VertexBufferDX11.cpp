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
#include "VertexBufferDX11.h"
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
VertexBufferDX11::VertexBufferDX11( Microsoft::WRL::ComPtr<ID3D11Buffer> pBuffer )
{
	m_pBuffer = pBuffer;
}
//--------------------------------------------------------------------------------
VertexBufferDX11::~VertexBufferDX11()
{
	// Buffer is released in the BufferDX11 destructor
}
//--------------------------------------------------------------------------------
ResourceType VertexBufferDX11::GetType()
{
	return( RT_VERTEXBUFFER );
}
//--------------------------------------------------------------------------------
void VertexBufferDX11::SetVertexSize( int size )
{
	m_iVertexSize = size;
}
//--------------------------------------------------------------------------------
void VertexBufferDX11::SetVertexCount( int count )
{
	m_iVertexCount = count;
}
//--------------------------------------------------------------------------------