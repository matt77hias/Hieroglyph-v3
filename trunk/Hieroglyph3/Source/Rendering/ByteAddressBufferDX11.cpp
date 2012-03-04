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
#include "ByteAddressBufferDX11.h"
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
ByteAddressBufferDX11::ByteAddressBufferDX11( ID3D11Buffer* pBuffer )
{
	m_pBuffer = pBuffer;
}
//--------------------------------------------------------------------------------
ByteAddressBufferDX11::~ByteAddressBufferDX11()
{
	// Buffer is released in the BufferDX11 destructor
}
//--------------------------------------------------------------------------------
ResourceType ByteAddressBufferDX11::GetType()
{
	return( RT_BYTEADDRESSBUFFER );
}
//--------------------------------------------------------------------------------
