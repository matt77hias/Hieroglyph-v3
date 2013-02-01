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
#include "SamplerStateDX11.h"
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
SamplerStateDX11::SamplerStateDX11( ID3D11SamplerState* pState )
{
	m_pState = pState;
}
//--------------------------------------------------------------------------------
SamplerStateDX11::~SamplerStateDX11()
{
	SAFE_RELEASE( m_pState );
}
//--------------------------------------------------------------------------------
