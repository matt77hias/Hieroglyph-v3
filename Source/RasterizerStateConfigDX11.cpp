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
#include "RasterizerStateConfigDX11.h"
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
RasterizerStateConfigDX11::RasterizerStateConfigDX11()
{
	SetDefaults();
}
//--------------------------------------------------------------------------------
RasterizerStateConfigDX11::~RasterizerStateConfigDX11()
{
}
//--------------------------------------------------------------------------------
void RasterizerStateConfigDX11::SetDefaults()
{
	// Set the state to the default configuration.  These are the D3D11 default
	// values as well.

	FillMode = D3D11_FILL_SOLID;
	CullMode = D3D11_CULL_BACK;
	FrontCounterClockwise = false;
	DepthBias = 0;
	SlopeScaledDepthBias = 0.0f;
	DepthBiasClamp = 0.0f;
	DepthClipEnable = true;
	ScissorEnable = false;
	MultisampleEnable = false;
	AntialiasedLineEnable = false;
}
//--------------------------------------------------------------------------------
