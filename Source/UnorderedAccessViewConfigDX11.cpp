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
#include "UnorderedAccessViewConfigDX11.h"
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
UnorderedAccessViewConfigDX11::UnorderedAccessViewConfigDX11()
{
    SetDefaults();
}
//--------------------------------------------------------------------------------
UnorderedAccessViewConfigDX11::~UnorderedAccessViewConfigDX11()
{
}
//--------------------------------------------------------------------------------
void UnorderedAccessViewConfigDX11::SetDefaults()
{
    // Set the state to the default configuration.  These are the D3D11 default
    // values as well.

    m_State.Format = DXGI_FORMAT_UNKNOWN;
    m_State.ViewDimension = D3D11_UAV_DIMENSION_UNKNOWN;
}
//--------------------------------------------------------------------------------
void UnorderedAccessViewConfigDX11::SetFormat( DXGI_FORMAT state )
{
    m_State.Format = state;
}
//--------------------------------------------------------------------------------
void UnorderedAccessViewConfigDX11::SetViewDimensions( D3D11_UAV_DIMENSION state )
{
    m_State.ViewDimension = state;
}
//--------------------------------------------------------------------------------
void UnorderedAccessViewConfigDX11::SetBuffer( D3D11_BUFFER_UAV state )
{
    m_State.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
    m_State.Buffer = state;
}
//--------------------------------------------------------------------------------
void UnorderedAccessViewConfigDX11::SetTexture1D( D3D11_TEX1D_UAV state )
{
    m_State.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE1D;
    m_State.Texture1D = state;
}
//--------------------------------------------------------------------------------
void UnorderedAccessViewConfigDX11::SetTexture1DArray( D3D11_TEX1D_ARRAY_UAV state )
{
    m_State.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE1DARRAY;
    m_State.Texture1DArray = state;
}
//--------------------------------------------------------------------------------
void UnorderedAccessViewConfigDX11::SetTexture2D( D3D11_TEX2D_UAV state )
{
    m_State.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
    m_State.Texture2D = state;
}
//--------------------------------------------------------------------------------
void UnorderedAccessViewConfigDX11::SetTexture2DArray( D3D11_TEX2D_ARRAY_UAV state )
{
    m_State.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2DARRAY;
    m_State.Texture2DArray = state;
}
//--------------------------------------------------------------------------------
void UnorderedAccessViewConfigDX11::SetTexture3D( D3D11_TEX3D_UAV state )
{
    m_State.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE3D;
    m_State.Texture3D = state;
}
//--------------------------------------------------------------------------------
D3D11_UNORDERED_ACCESS_VIEW_DESC& UnorderedAccessViewConfigDX11::GetUAVDesc()
{
    return( m_State );
}
//--------------------------------------------------------------------------------
