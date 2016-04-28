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
#include "SwapChainConfigDX11.h"
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
SwapChainConfigDX11::SwapChainConfigDX11()
{
	SetDefaults();
}
//--------------------------------------------------------------------------------
SwapChainConfigDX11::~SwapChainConfigDX11()
{
}
//--------------------------------------------------------------------------------
void SwapChainConfigDX11::SetDefaults()
{
	// Set the state to the default configuration.  These are the D3D11 default
	// values as well.

	m_State.BufferDesc.Width = 1;
	m_State.BufferDesc.Height = 1;
    m_State.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	m_State.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
    m_State.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	m_State.BufferDesc.RefreshRate.Numerator = 60;
	m_State.BufferDesc.RefreshRate.Denominator = 1;

	m_State.SampleDesc.Count = 1;
	m_State.SampleDesc.Quality = 0;

	m_State.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	m_State.BufferCount = 2;
	m_State.OutputWindow = 0;
	m_State.Windowed = true;
	m_State.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	// If you are creating a full screen swap chain, you may want to include the 
	// DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH flag in the flags below, which will
	// override the default DXGI behavior.  See here for more details:
	// http://msdn.microsoft.com/en-us/library/windows/desktop/ee417025%28v=vs.85%29.aspx

	m_State.Flags = 0; 
}
//--------------------------------------------------------------------------------
void SwapChainConfigDX11::SetWidth( UINT width )
{
	m_State.BufferDesc.Width = width;
}
//--------------------------------------------------------------------------------
void SwapChainConfigDX11::SetHeight( UINT height )
{
	m_State.BufferDesc.Height = height;
}
//--------------------------------------------------------------------------------
void SwapChainConfigDX11::SetRefreshRateNumerator( UINT numerator )
{
	m_State.BufferDesc.RefreshRate.Numerator = numerator;
}
//--------------------------------------------------------------------------------
void SwapChainConfigDX11::SetRefreshRateDenominator( UINT denominator )
{
	m_State.BufferDesc.RefreshRate.Denominator = denominator;
}
//--------------------------------------------------------------------------------
void SwapChainConfigDX11::SetFormat( DXGI_FORMAT Format )
{
	m_State.BufferDesc.Format = Format;
}
//--------------------------------------------------------------------------------
void SwapChainConfigDX11::SetScanlineOrder( DXGI_MODE_SCANLINE_ORDER ScanlineOrdering )
{
	m_State.BufferDesc.ScanlineOrdering = ScanlineOrdering;
}
//--------------------------------------------------------------------------------
void SwapChainConfigDX11::SetScaling( DXGI_MODE_SCALING Scaling )
{
	m_State.BufferDesc.Scaling = Scaling;
}
//--------------------------------------------------------------------------------
void SwapChainConfigDX11::SetBufferDesc( DXGI_MODE_DESC BufferDesc )
{
	m_State.BufferDesc = BufferDesc;
}
//--------------------------------------------------------------------------------
void SwapChainConfigDX11::SetSampleDesc( DXGI_SAMPLE_DESC SampleDesc )
{
	m_State.SampleDesc = SampleDesc;
}
//--------------------------------------------------------------------------------
void SwapChainConfigDX11::SetBufferUsage( DXGI_USAGE BufferUsage )
{
	m_State.BufferUsage = BufferUsage;
}
//--------------------------------------------------------------------------------
void SwapChainConfigDX11::SetBufferCount( UINT BufferCount )
{
	m_State.BufferCount = BufferCount;
}
//--------------------------------------------------------------------------------
void SwapChainConfigDX11::SetOutputWindow( HWND OutputWindow )
{
	m_State.OutputWindow = OutputWindow;
}
//--------------------------------------------------------------------------------
void SwapChainConfigDX11::SetWindowed( bool Windowed )
{
	m_State.Windowed = Windowed;
}
//--------------------------------------------------------------------------------
void SwapChainConfigDX11::SetSwapEffect( DXGI_SWAP_EFFECT SwapEffect )
{
	m_State.SwapEffect = SwapEffect;
}
//--------------------------------------------------------------------------------
void SwapChainConfigDX11::SetFlags( UINT Flags )
{
	m_State.Flags = Flags;
}
//--------------------------------------------------------------------------------
DXGI_SWAP_CHAIN_DESC SwapChainConfigDX11::GetSwapChainDesc()
{
	return( m_State );
}
//--------------------------------------------------------------------------------