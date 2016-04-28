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
#include "StructuredBufferDX11.h"
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
StructuredBufferDX11::StructuredBufferDX11( Microsoft::WRL::ComPtr<ID3D11Buffer> pBuffer )
{
	m_pBuffer = pBuffer;
}
//--------------------------------------------------------------------------------
StructuredBufferDX11::~StructuredBufferDX11()
{
	// Buffer is released in the BufferDX11 destructor
}
//--------------------------------------------------------------------------------
ResourceType StructuredBufferDX11::GetType()
{
	return( RT_STRUCTUREDBUFFER );
}
//--------------------------------------------------------------------------------
