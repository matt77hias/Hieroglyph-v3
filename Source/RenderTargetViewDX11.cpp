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
#include "RenderTargetViewDX11.h"
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
RenderTargetViewDX11::RenderTargetViewDX11( RenderTargetViewComPtr pView )
{
	m_pRenderTargetView = pView;
}
//--------------------------------------------------------------------------------
RenderTargetViewDX11::~RenderTargetViewDX11()
{
}
//--------------------------------------------------------------------------------
ID3D11RenderTargetView* RenderTargetViewDX11::GetRTV()
{
	return( m_pRenderTargetView.Get() );
}
//--------------------------------------------------------------------------------
