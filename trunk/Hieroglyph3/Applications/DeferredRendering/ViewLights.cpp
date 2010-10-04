//--------------------------------------------------------------------------------
// This file is a portion of the Hieroglyph 3 Rendering Engine.  It is distributed
// under the MIT License, available in the root of this distribution and 
// at the following URL:
//
// http://www.opensource.org/licenses/mit-license.php
//
// Copyright (c) 2003-2010 Jason Zink 
//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
#include "ViewLights.h"
#include "Entity3D.h"
#include "Node3D.h"
#include "Texture2dConfigDX11.h"
#include "Log.h"
#include "ParameterManagerDX11.h"
#include "PipelineManagerDX11.h"
#include "Texture2dDX11.h"
#include "DepthStencilStateConfigDX11.h"
#include "ViewGBuffer.h"
#include "GeometryGeneratorDX11.h"
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
ViewLights::ViewLights( RendererDX11& Renderer, ResourcePtr pRenderTarget, ResourcePtr DepthTarget, 
                        TArray<ResourcePtr>& GBufferTargets )
    : m_pRenderTarget( pRenderTarget ), m_DepthTarget( DepthTarget ), m_GBufferTargets( GBufferTargets )
{
    m_sParams.iViewType = VT_LIGHTS;

    ViewMatrix.MakeIdentity();
    ProjMatrix.MakeIdentity();	

    // Create the viewport based on the first render target
    ResourceDX11* pResource = Renderer.GetResource( m_pRenderTarget->m_iResource & 0x0000ffff );

    if ( pResource->GetType() == D3D11_RESOURCE_DIMENSION_TEXTURE2D )
    {
        Texture2dDX11* pTexture = (Texture2dDX11*)pResource;
        D3D11_TEXTURE2D_DESC desc = pTexture->GetActualDescription();

        // Create a view port to use on the scene.  This basically selects the 
        // entire floating point area of the render target.
        D3D11_VIEWPORT viewport;
        viewport.Width = static_cast< float >( desc.Width );
        viewport.Height = static_cast< float >( desc.Height );
        viewport.MinDepth = 0.0f;
        viewport.MaxDepth = 1.0f;
        viewport.TopLeftX = 0;
        viewport.TopLeftY = 0;

        m_iViewport = Renderer.CreateViewPort( viewport );
    }

    // Create the depth stencil view. We'll disable stencil writes and depth writes
    DepthStencilStateConfigDX11 dsConfig;
    dsConfig.DepthEnable = false;
    dsConfig.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    dsConfig.DepthFunc = D3D11_COMPARISON_LESS;
    dsConfig.StencilEnable = true;
    dsConfig.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
    dsConfig.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;
    dsConfig.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
    dsConfig.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    dsConfig.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    dsConfig.FrontFace.StencilFunc = D3D11_COMPARISON_EQUAL;
    dsConfig.BackFace = dsConfig.FrontFace;

    m_iDepthStencilState = Renderer.CreateDepthStencilState( &dsConfig );

    if ( m_iDepthStencilState == -1 )	
        Log::Get().Write( L"Failed to create light depth stencil state" );

    // Generate geometry for a full screen quad
    GeometryGeneratorDX11::GenerateFullScreenQuad( &m_QuadGeometry );
    m_QuadGeometry.LoadToBuffers();

    // Load our common vertex shader
    m_PointLightEffect.m_iVertexShader =
        Renderer.LoadShader( VERTEX_SHADER,
        std::wstring( L"../Data/Shaders/Lights.hlsl" ),
        std::wstring( L"VSQuad" ),
        std::wstring( L"vs_5_0" ) );        
    _ASSERT( m_PointLightEffect.m_iVertexShader != -1 );

    m_SpotLightEffect.m_iVertexShader = m_PointLightEffect.m_iVertexShader;
    m_DirectionalLightEffect.m_iVertexShader = m_PointLightEffect.m_iVertexShader;

    // Load our permutations of the pixel shader
    D3D10_SHADER_MACRO defines[4];
    defines[0].Name = "POINTLIGHT";
    defines[0].Definition = "1";
    defines[1].Name = "SPOTLIGHT";
    defines[1].Definition = "0";
    defines[2].Name = "DIRECTIONALLIGHT";
    defines[2].Definition = "0";
    defines[3].Name = NULL;
    defines[3].Definition = NULL;

    m_PointLightEffect.m_iPixelShader =
        Renderer.LoadShader( PIXEL_SHADER,
        std::wstring( L"../Data/Shaders/Lights.hlsl" ),
        std::wstring( L"PSMain" ),
        std::wstring( L"ps_5_0" ),
        defines );        
    _ASSERT( m_PointLightEffect.m_iPixelShader != -1 );

    defines[0].Definition = "0";
    defines[1].Definition = "1";
    m_SpotLightEffect.m_iPixelShader =
        Renderer.LoadShader( PIXEL_SHADER,
        std::wstring( L"../Data/Shaders/Lights.hlsl" ),
        std::wstring( L"PSMain" ),
        std::wstring( L"ps_5_0" ),
        defines );        
    _ASSERT( m_SpotLightEffect.m_iPixelShader != -1 );

    defines[1].Definition = "0";
    defines[2].Definition = "1";
    m_DirectionalLightEffect.m_iPixelShader =
        Renderer.LoadShader( PIXEL_SHADER,
        std::wstring( L"../Data/Shaders/Lights.hlsl" ),
        std::wstring( L"PSMain" ),
        std::wstring( L"ps_5_0" ),
        defines );        
    _ASSERT( m_DirectionalLightEffect.m_iPixelShader != -1 );

}
//--------------------------------------------------------------------------------
ViewLights::~ViewLights()
{
}
//--------------------------------------------------------------------------------
void ViewLights::Update( float fTime )
{
}
//--------------------------------------------------------------------------------
void ViewLights::PreDraw( RendererDX11* pRenderer )
{
    if ( m_pEntity != NULL )
    {
        Matrix4f view = m_pEntity->GetView();
        SetViewMatrix( view );        
    }

    // Queue this view into the renderer for processing.
    pRenderer->QueueRenderView( this );
}
//--------------------------------------------------------------------------------
void ViewLights::Draw( PipelineManagerDX11* pPipelineManager, ParameterManagerDX11* pParamManager )
{
    // Bind the render target
    pPipelineManager->ClearRenderTargets();
    pPipelineManager->BindRenderTargets( 0, m_pRenderTarget );    
    pPipelineManager->ApplyRenderTargets();

    // Clear the render target
    Vector4f color(0.0f, 0.0f, 0.0f, 0.0f);
    pPipelineManager->ClearBuffers( color, 1.0f, 0 );

    // Bind the depth buffer
    pPipelineManager->BindDepthTarget( m_DepthTarget );
    pPipelineManager->ApplyRenderTargets();

    pPipelineManager->SetViewPort( m_iViewport );

    // Set default states for these stages
    pPipelineManager->SetRasterizerState( 0 );
    pPipelineManager->SetBlendState( 0 );

    // Set our depth stencil state, using the same reference value used in the G-Buffer pass
    pPipelineManager->SetDepthStencilState( m_iDepthStencilState, ViewGBuffer::StencilRef );

    

    // Set this view's render parameters
    SetRenderParams( pParamManager );

    // Loop through the lights
    for ( int i = 0; i < m_Lights.count(); ++i )
    {
        const Light& light = m_Lights[i];

        // Pick the effect based on the shader
        RenderEffectDX11* pEffect = NULL;
        if ( light.Type == Point )
            pEffect = &m_PointLightEffect;
        else if ( light.Type == Spot )
            pEffect = &m_SpotLightEffect;
        else if ( light.Type == Directional )
            pEffect = &m_DirectionalLightEffect;

        // Set the light params
        pParamManager->SetVectorParameter( L"LightPos", &Vector4f( light.Position, 1.0f ) );
        pParamManager->SetVectorParameter( L"LightColor", &Vector4f( light.Color, 1.0f ) );
        pParamManager->SetVectorParameter( L"LightDirection", &Vector4f( light.Direction, 1.0f ) );
        pParamManager->SetVectorParameter( L"SpotlightAngles", &Vector4f( light.SpotInnerAngle, light.SpotOuterAngle, 0.0f, 0.0f ) );

        // Draw the geometry
        pPipelineManager->Draw( *pEffect, m_QuadGeometry, pParamManager );
    }

    pPipelineManager->ClearPipelineResources();
}
//--------------------------------------------------------------------------------
void ViewLights::SetRenderParams( ParameterManagerDX11* pParamManager )
{
    pParamManager->SetViewMatrixParameter( &ViewMatrix );
    pParamManager->SetProjMatrixParameter( &ProjMatrix );

    if ( m_pRoot != NULL ) 
    {
        Vector4f cameraPos( m_pRoot->Position(), 1.0f );
        pParamManager->SetVectorParameter( L"CameraPos", &cameraPos );
    }    

    // Set the G-Buffer textures
    static const std::wstring TextureNames[4] = 
    { 
        L"NormalTexture",  
        L"DiffuseAlbedoTexture",
        L"SpecularAlbedoTexture",
        L"PositionTexture",
    };

    for( UINT i = 0; i < 4; ++i )    
        pParamManager->SetShaderResourceParameter( TextureNames[i], m_GBufferTargets[i] );    
}
//--------------------------------------------------------------------------------
void ViewLights::SetUsageParams( ParameterManagerDX11* pParamManager )
{

}
//--------------------------------------------------------------------------------
void ViewLights::AddLight( const Light& light )
{
    m_Lights.add( light );
}
