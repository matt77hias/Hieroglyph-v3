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
#include "BlendStateConfigDX11.h"
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
BlendStateConfigDX11::BlendStateConfigDX11()
{
	SetDefaults();
}
//--------------------------------------------------------------------------------
BlendStateConfigDX11::~BlendStateConfigDX11()
{
}
//--------------------------------------------------------------------------------
void BlendStateConfigDX11::SetDefaults()
{
	// Set the state to the default configuration.  These are the D3D11 default
	// values as well.

	AlphaToCoverageEnable = false;
	IndependentBlendEnable = false;

	for ( int i = 0; i < 8; i++ )
	{
		RenderTarget[i].BlendEnable = false;
		RenderTarget[i].SrcBlend = D3D11_BLEND_ONE;
		RenderTarget[i].DestBlend = D3D11_BLEND_ZERO;
		RenderTarget[i].BlendOp = D3D11_BLEND_OP_ADD; 
		RenderTarget[i].SrcBlendAlpha = D3D11_BLEND_ONE;
		RenderTarget[i].DestBlendAlpha = D3D11_BLEND_ZERO;
		RenderTarget[i].BlendOpAlpha = D3D11_BLEND_OP_ADD; 
		RenderTarget[i].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	}
}
//--------------------------------------------------------------------------------
