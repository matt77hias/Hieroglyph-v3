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
// SwapChainConfigDX11
//
//--------------------------------------------------------------------------------
#ifndef SwapChainConfigDX11_h
#define SwapChainConfigDX11_h
//--------------------------------------------------------------------------------
#include "RendererDX11.h"
//--------------------------------------------------------------------------------
namespace Glyph3
{
	class SwapChainConfigDX11
	{
	public:
		SwapChainConfigDX11();
		virtual ~SwapChainConfigDX11();

		void SetDefaults();

		void SetWidth( UINT width );
		void SetHeight( UINT height );
		void SetRefreshRateNumerator( UINT numerator );
		void SetRefreshRateDenominator( UINT denominator );
		void SetFormat( DXGI_FORMAT Format );
		void SetScanlineOrder( DXGI_MODE_SCANLINE_ORDER ScanlineOrdering );
		void SetScaling( DXGI_MODE_SCALING Scaling );


		void SetBufferDesc( DXGI_MODE_DESC BufferDesc );
		void SetSampleDesc( DXGI_SAMPLE_DESC SampleDesc );
		void SetBufferUsage( DXGI_USAGE BufferUsage );
		void SetBufferCount( UINT BufferCount );
		void SetOutputWindow( HWND OutputWindow );
		void SetWindowed( bool Windowed );
		void SetSwapEffect( DXGI_SWAP_EFFECT SwapEffect );
		void SetFlags( UINT Flags );

		DXGI_SWAP_CHAIN_DESC GetSwapChainDesc();

	protected:
		DXGI_SWAP_CHAIN_DESC 		m_State;

		friend RendererDX11;
	};
};
//--------------------------------------------------------------------------------
#endif // SwapChainConfigDX11_h
//--------------------------------------------------------------------------------

