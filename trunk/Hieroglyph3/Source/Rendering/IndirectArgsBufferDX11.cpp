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
#include "IndirectArgsBufferDX11.h"
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
IndirectArgsBufferDX11::IndirectArgsBufferDX11( ID3D11Buffer* pBuffer )
{
	m_pBuffer = pBuffer;
}
//--------------------------------------------------------------------------------
IndirectArgsBufferDX11::~IndirectArgsBufferDX11()
{
	// m_pBuffer is released in the BufferDX11 destructor
}
//--------------------------------------------------------------------------------
