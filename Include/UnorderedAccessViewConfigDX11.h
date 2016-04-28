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
// UnorderedAccessViewConfigDX11
//
//--------------------------------------------------------------------------------
#ifndef UnorderedAccessViewConfigDX11_h
#define UnorderedAccessViewConfigDX11_h
//--------------------------------------------------------------------------------
#include "RendererDX11.h"
//--------------------------------------------------------------------------------
namespace Glyph3
{
    class UnorderedAccessViewConfigDX11
    {
    public:
        UnorderedAccessViewConfigDX11();
        virtual ~UnorderedAccessViewConfigDX11();

        void SetDefaults();

        void SetFormat( DXGI_FORMAT state );
        void SetViewDimensions( D3D11_UAV_DIMENSION state );        

        void SetBuffer( D3D11_BUFFER_UAV state );
        void SetTexture1D( D3D11_TEX1D_UAV state );
        void SetTexture1DArray( D3D11_TEX1D_ARRAY_UAV state );
        void SetTexture2D( D3D11_TEX2D_UAV state );
        void SetTexture2DArray( D3D11_TEX2D_ARRAY_UAV state );
        void SetTexture3D( D3D11_TEX3D_UAV state);

        D3D11_UNORDERED_ACCESS_VIEW_DESC& GetUAVDesc();

    protected:
        D3D11_UNORDERED_ACCESS_VIEW_DESC    m_State;

        friend RendererDX11;
    };
};
//--------------------------------------------------------------------------------
#endif // UnorderedAccessViewConfigDX11_h
//--------------------------------------------------------------------------------

