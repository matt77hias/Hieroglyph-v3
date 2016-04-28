//--------------------------------------------------------------------------------
// This file is a portion of the Hieroglyph 3 Rendering Engine.  It is distributed
// under the MIT License, available in the root of this distribution and 
// at the following URL:
//
// http://www.opensource.org/licenses/mit-license.php
//
// Copyright (c) Jason Zink 
//--------------------------------------------------------------------------------

#include "PCH.h"
#include "RenderTargetViewConfigDX11.h"
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
RenderTargetViewConfigDX11::RenderTargetViewConfigDX11()
{
    SetDefaults();
}
//--------------------------------------------------------------------------------
RenderTargetViewConfigDX11::~RenderTargetViewConfigDX11()
{
}
//--------------------------------------------------------------------------------
void RenderTargetViewConfigDX11::SetDefaults()
{
    // Set the state to the default configuration.  These are the D3D11 default
    // values as well.

    m_State.Format = DXGI_FORMAT_UNKNOWN;
    m_State.ViewDimension = D3D11_RTV_DIMENSION_UNKNOWN;
}
//--------------------------------------------------------------------------------
void RenderTargetViewConfigDX11::SetFormat( DXGI_FORMAT state )
{
    m_State.Format = state;
}
//--------------------------------------------------------------------------------
void RenderTargetViewConfigDX11::SetViewDimensions( D3D11_RTV_DIMENSION state )
{
    m_State.ViewDimension = state;
}
//--------------------------------------------------------------------------------
void RenderTargetViewConfigDX11::SetBuffer( D3D11_BUFFER_RTV state )
{
    m_State.ViewDimension = D3D11_RTV_DIMENSION_BUFFER;
    m_State.Buffer = state;
}
//--------------------------------------------------------------------------------
void RenderTargetViewConfigDX11::SetTexture1D( D3D11_TEX1D_RTV state )
{
    m_State.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE1D;
    m_State.Texture1D = state;
}
//--------------------------------------------------------------------------------
void RenderTargetViewConfigDX11::SetTexture1DArray( D3D11_TEX1D_ARRAY_RTV state )
{
    m_State.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE1DARRAY;
    m_State.Texture1DArray = state;
}
//--------------------------------------------------------------------------------
void RenderTargetViewConfigDX11::SetTexture2D( D3D11_TEX2D_RTV state )
{
    m_State.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
    m_State.Texture2D = state;
}
//--------------------------------------------------------------------------------
void RenderTargetViewConfigDX11::SetTexture2DArray( D3D11_TEX2D_ARRAY_RTV state )
{
    m_State.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
    m_State.Texture2DArray = state;
}
//--------------------------------------------------------------------------------
void RenderTargetViewConfigDX11::SetTexture2DMS( D3D11_TEX2DMS_RTV state )
{
    m_State.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DMS;
    m_State.Texture2DMS = state;
}
//--------------------------------------------------------------------------------
void RenderTargetViewConfigDX11::SetTexture2DMSArray( D3D11_TEX2DMS_ARRAY_RTV state )
{
    m_State.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DMSARRAY;
    m_State.Texture2DMSArray = state;
}
//--------------------------------------------------------------------------------
void RenderTargetViewConfigDX11::SetTexture3D( D3D11_TEX3D_RTV state )
{
    m_State.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE3D;
    m_State.Texture3D = state;
}
//--------------------------------------------------------------------------------
D3D11_RENDER_TARGET_VIEW_DESC& RenderTargetViewConfigDX11::GetRTVDesc()
{
    return( m_State );
}
//--------------------------------------------------------------------------------
