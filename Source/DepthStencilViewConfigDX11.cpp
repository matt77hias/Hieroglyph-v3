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
#include "DepthStencilViewConfigDX11.h"
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
DepthStencilViewConfigDX11::DepthStencilViewConfigDX11()
{
	SetDefaults();
}
//--------------------------------------------------------------------------------
DepthStencilViewConfigDX11::~DepthStencilViewConfigDX11()
{
}
//--------------------------------------------------------------------------------
void DepthStencilViewConfigDX11::SetDefaults()
{
	// Set the state to the default configuration.  These are the D3D11 default
	// values as well.

	m_State.Flags = 0;
	m_State.Format = DXGI_FORMAT_UNKNOWN;
	m_State.ViewDimension = D3D11_DSV_DIMENSION_UNKNOWN;
}
//--------------------------------------------------------------------------------
void DepthStencilViewConfigDX11::SetFormat( DXGI_FORMAT state )
{
	m_State.Format = state;
}
//--------------------------------------------------------------------------------
void DepthStencilViewConfigDX11::SetViewDimensions( D3D11_DSV_DIMENSION state )
{
	m_State.ViewDimension = state;
}
//--------------------------------------------------------------------------------
void DepthStencilViewConfigDX11::SetFlags( UINT state )
{
	m_State.Flags = state;
}
//--------------------------------------------------------------------------------
void DepthStencilViewConfigDX11::SetTexture1D( D3D11_TEX1D_DSV state )
{
	m_State.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE1D;
	m_State.Texture1D = state;
}
//--------------------------------------------------------------------------------
void DepthStencilViewConfigDX11::SetTexture1DArray( D3D11_TEX1D_ARRAY_DSV state )
{
	m_State.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE1DARRAY;
	m_State.Texture1DArray = state;
}
//--------------------------------------------------------------------------------
void DepthStencilViewConfigDX11::SetTexture2D( D3D11_TEX2D_DSV state )
{
	m_State.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	m_State.Texture2D = state;
}
//--------------------------------------------------------------------------------
void DepthStencilViewConfigDX11::SetTexture2DArray( D3D11_TEX2D_ARRAY_DSV state )
{
	m_State.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DARRAY;
	m_State.Texture2DArray = state;
}
//--------------------------------------------------------------------------------
void DepthStencilViewConfigDX11::SetTexture2DMS( D3D11_TEX2DMS_DSV state )
{
	m_State.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;
	m_State.Texture2DMS = state;
}
//--------------------------------------------------------------------------------
void DepthStencilViewConfigDX11::SetTexture2DMSArray( D3D11_TEX2DMS_ARRAY_DSV state )
{
	m_State.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMSARRAY;
	m_State.Texture2DMSArray = state;
}
//--------------------------------------------------------------------------------
D3D11_DEPTH_STENCIL_VIEW_DESC& DepthStencilViewConfigDX11::GetDSVDesc()
{
	return( m_State );
}
//--------------------------------------------------------------------------------