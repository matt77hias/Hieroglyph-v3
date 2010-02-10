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
#include "UnorderedAccessViewDX11.h"
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
UnorderedAccessViewDX11::UnorderedAccessViewDX11( ID3D11UnorderedAccessView* pView )
{
	m_pUnorderedAccessView = pView;
}
//--------------------------------------------------------------------------------
UnorderedAccessViewDX11::~UnorderedAccessViewDX11()
{
	SAFE_RELEASE( m_pUnorderedAccessView );
}
//--------------------------------------------------------------------------------
