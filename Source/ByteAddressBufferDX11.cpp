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
#include "ByteAddressBufferDX11.h"
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
ByteAddressBufferDX11::ByteAddressBufferDX11( Microsoft::WRL::ComPtr<ID3D11Buffer> pBuffer )
{
	m_pBuffer = pBuffer;
}
//--------------------------------------------------------------------------------
ByteAddressBufferDX11::~ByteAddressBufferDX11()
{
}
//--------------------------------------------------------------------------------
ResourceType ByteAddressBufferDX11::GetType()
{
	return( RT_BYTEADDRESSBUFFER );
}
//--------------------------------------------------------------------------------
