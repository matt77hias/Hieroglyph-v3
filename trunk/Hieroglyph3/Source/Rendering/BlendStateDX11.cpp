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
#include "BlendStateDX11.h"
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
BlendStateDX11::BlendStateDX11( ID3D11BlendState* pState )
{
	m_pState = pState;
}
//--------------------------------------------------------------------------------
BlendStateDX11::~BlendStateDX11()
{
	SAFE_RELEASE( m_pState );
}
//--------------------------------------------------------------------------------
