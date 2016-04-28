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
// SwapChainDX11
//
//--------------------------------------------------------------------------------
#ifndef SwapChainDX11_h
#define SwapChainDX11_h
//--------------------------------------------------------------------------------
#include "RendererDX11.h"
//--------------------------------------------------------------------------------
namespace Glyph3
{
	class SwapChainDX11
	{
	public:
		explicit SwapChainDX11( Microsoft::WRL::ComPtr<IDXGISwapChain> pSwapChain, ResourcePtr resource );
		virtual ~SwapChainDX11();

		IDXGISwapChain* GetSwapChain();

	protected:
		Microsoft::WRL::ComPtr<IDXGISwapChain>	m_pSwapChain;
		ResourcePtr								m_Resource;

		friend RendererDX11;
	};
};
//--------------------------------------------------------------------------------
#endif // SwapChainDX11_h
//--------------------------------------------------------------------------------

