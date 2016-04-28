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
#include "ShaderResourceViewConfigDX11.h"
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
ShaderResourceViewConfigDX11::ShaderResourceViewConfigDX11()
{
    SetDefaults();
}
//--------------------------------------------------------------------------------
ShaderResourceViewConfigDX11::~ShaderResourceViewConfigDX11()
{
}
//--------------------------------------------------------------------------------
void ShaderResourceViewConfigDX11::SetDefaults()
{
    // Set the state to the default configuration.  These are the D3D11 default
    // values as well.
    
    m_State.Format = DXGI_FORMAT_UNKNOWN;
    m_State.ViewDimension = D3D11_SRV_DIMENSION_UNKNOWN;
}
//--------------------------------------------------------------------------------
void ShaderResourceViewConfigDX11::SetFormat( DXGI_FORMAT state )
{
    m_State.Format = state;
}
//--------------------------------------------------------------------------------
void ShaderResourceViewConfigDX11::SetViewDimensions( D3D11_SRV_DIMENSION state )
{
    m_State.ViewDimension = state;
}
//--------------------------------------------------------------------------------
void ShaderResourceViewConfigDX11::SetBuffer( D3D11_BUFFER_SRV state )
{
    m_State.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
    m_State.Buffer = state;
}
//--------------------------------------------------------------------------------
void ShaderResourceViewConfigDX11::SetBufferEx( D3D11_BUFFEREX_SRV state )
{
    m_State.ViewDimension = D3D11_SRV_DIMENSION_BUFFEREX;
    m_State.BufferEx = state;
}
//--------------------------------------------------------------------------------
void ShaderResourceViewConfigDX11::SetTexture1D( D3D11_TEX1D_SRV state )
{
    m_State.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE1D;
    m_State.Texture1D = state;
}
//--------------------------------------------------------------------------------
void ShaderResourceViewConfigDX11::SetTexture1DArray( D3D11_TEX1D_ARRAY_SRV state )
{
    m_State.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE1DARRAY;
    m_State.Texture1DArray = state;
}
//--------------------------------------------------------------------------------
void ShaderResourceViewConfigDX11::SetTexture2D( D3D11_TEX2D_SRV state )
{
    m_State.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    m_State.Texture2D = state;
}
//--------------------------------------------------------------------------------
void ShaderResourceViewConfigDX11::SetTexture2DArray( D3D11_TEX2D_ARRAY_SRV state )
{
    m_State.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
    m_State.Texture2DArray = state;
}
//--------------------------------------------------------------------------------
void ShaderResourceViewConfigDX11::SetTexture2DMS( D3D11_TEX2DMS_SRV state )
{
    m_State.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DMS;
    m_State.Texture2DMS = state;
}
//--------------------------------------------------------------------------------
void ShaderResourceViewConfigDX11::SetTexture2DMSArray( D3D11_TEX2DMS_ARRAY_SRV state )
{
    m_State.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DMSARRAY;
    m_State.Texture2DMSArray = state;
}
//--------------------------------------------------------------------------------
void ShaderResourceViewConfigDX11::SetTexture3D( D3D11_TEX3D_SRV state )
{
    m_State.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE3D;
    m_State.Texture3D = state;
}
//--------------------------------------------------------------------------------
void ShaderResourceViewConfigDX11::SetTextureCube( D3D11_TEXCUBE_SRV state )
{
    m_State.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
    m_State.TextureCube = state;
}
//--------------------------------------------------------------------------------
void ShaderResourceViewConfigDX11::SetTextureCubeArray( D3D11_TEXCUBE_ARRAY_SRV state )
{
    m_State.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBEARRAY;
    m_State.TextureCubeArray = state;
}
//--------------------------------------------------------------------------------
D3D11_SHADER_RESOURCE_VIEW_DESC& ShaderResourceViewConfigDX11::GetSRVDesc()
{
    return( m_State );
}
//--------------------------------------------------------------------------------
