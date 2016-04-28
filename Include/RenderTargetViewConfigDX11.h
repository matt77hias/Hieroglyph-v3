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
// RenderTargetViewConfigDX11
//
//--------------------------------------------------------------------------------
#ifndef RenderTargetViewConfigDX11_h
#define RenderTargetViewConfigDX11_h
//--------------------------------------------------------------------------------
#include "RendererDX11.h"
//--------------------------------------------------------------------------------
namespace Glyph3
{
    class RenderTargetViewConfigDX11
    {
    public:
        RenderTargetViewConfigDX11();
        virtual ~RenderTargetViewConfigDX11();

        void SetDefaults();

        void SetFormat( DXGI_FORMAT state );
        void SetViewDimensions( D3D11_RTV_DIMENSION state );        

        void SetBuffer( D3D11_BUFFER_RTV state );        
        void SetTexture1D( D3D11_TEX1D_RTV state );
        void SetTexture1DArray( D3D11_TEX1D_ARRAY_RTV state );
        void SetTexture2D( D3D11_TEX2D_RTV state );
        void SetTexture2DArray( D3D11_TEX2D_ARRAY_RTV state );
        void SetTexture2DMS( D3D11_TEX2DMS_RTV state );
        void SetTexture2DMSArray( D3D11_TEX2DMS_ARRAY_RTV state );
        void SetTexture3D( D3D11_TEX3D_RTV state );        

        D3D11_RENDER_TARGET_VIEW_DESC& GetRTVDesc();

    protected:
        D3D11_RENDER_TARGET_VIEW_DESC 		m_State;

        friend RendererDX11;
    };
};
//--------------------------------------------------------------------------------
#endif // RenderTargetViewConfigDX11_h
//--------------------------------------------------------------------------------

