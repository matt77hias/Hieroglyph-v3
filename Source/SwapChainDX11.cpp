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
#include "SwapChainDX11.h"
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
SwapChainDX11::SwapChainDX11( Microsoft::WRL::ComPtr<IDXGISwapChain> pSwapChain, ResourcePtr resource )
{
	m_pSwapChain = pSwapChain;
	m_Resource = resource;
}
//--------------------------------------------------------------------------------
SwapChainDX11::~SwapChainDX11()
{
}
//--------------------------------------------------------------------------------
IDXGISwapChain* SwapChainDX11::GetSwapChain()
{
	return( m_pSwapChain.Get() );
}
//--------------------------------------------------------------------------------
