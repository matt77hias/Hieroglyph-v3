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
#include "RasterizerStateDX11.h"
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
RasterizerStateDX11::RasterizerStateDX11( ID3D11RasterizerState* pState )
{
	m_pState = pState;
}
//--------------------------------------------------------------------------------
RasterizerStateDX11::~RasterizerStateDX11()
{
	SAFE_RELEASE( m_pState );
}
//--------------------------------------------------------------------------------
