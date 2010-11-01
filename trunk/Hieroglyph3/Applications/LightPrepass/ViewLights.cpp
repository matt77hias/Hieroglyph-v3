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
#include "BlendStateConfigDX11.h"
#include "AppSettings.h"
#include "RasterizerStateConfigDX11.h"
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
static float Clamp( float x, float low, float high )
{
    if ( x < low )
        x = low;
    if ( x > high )
        x = high;
    return x;
}
//--------------------------------------------------------------------------------
ViewLights::ViewLights( RendererDX11& Renderer)    
{
    m_sParams.iViewType = VT_LIGHTS;

    ViewMatrix.MakeIdentity();
    ProjMatrix.MakeIdentity();	
 

    // Create a depth stencil state with no depth testing, and with stencil testing
    // enabled to make sure we only light pixels where we rendered to the G-Buffer
    DepthStencilStateConfigDX11 dsConfig;
    dsConfig.DepthEnable = TRUE;
    dsConfig.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
    dsConfig.DepthFunc = D3D11_COMPARISON_LESS;
    dsConfig.StencilEnable = TRUE;
    dsConfig.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
    dsConfig.StencilWriteMask = 0;
    dsConfig.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
    dsConfig.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    dsConfig.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    dsConfig.FrontFace.StencilFunc = D3D11_COMPARISON_EQUAL;
    dsConfig.BackFace = dsConfig.FrontFace;

    m_iDisabledDSState = Renderer.CreateDepthStencilState( &dsConfig );

    if ( m_iDisabledDSState == -1 )	
        Log::Get().Write( L"Failed to create light depth stencil state" );

    // Create a depth stencil state with less-than-equal depth testing
    dsConfig.DepthEnable = FALSE;
    dsConfig.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
    m_iLessThanDSState = Renderer.CreateDepthStencilState( &dsConfig );

    if ( m_iLessThanDSState == -1 )	
        Log::Get().Write( L"Failed to create light depth stencil state" );

    // Create a depth stencil state with greater-than-equal depth testing
    dsConfig.DepthFunc = D3D11_COMPARISON_GREATER_EQUAL;
    m_iGreaterThanDSState = Renderer.CreateDepthStencilState( &dsConfig );

    if ( m_iGreaterThanDSState == -1 )	
        Log::Get().Write( L"Failed to create light depth stencil state" );

    // Create a blend state for additive blending
    BlendStateConfigDX11 blendConfig;
    blendConfig.AlphaToCoverageEnable = false;
    blendConfig.IndependentBlendEnable = false;
    for ( int i = 0; i < 8; ++i )
    {
        blendConfig.RenderTarget[i].BlendEnable = TRUE;
        blendConfig.RenderTarget[i].BlendOp = D3D11_BLEND_OP_ADD;
        blendConfig.RenderTarget[i].SrcBlend = D3D11_BLEND_ONE;
        blendConfig.RenderTarget[i].DestBlend = D3D11_BLEND_ONE;
        blendConfig.RenderTarget[i].BlendOpAlpha = D3D11_BLEND_OP_ADD;
        blendConfig.RenderTarget[i].SrcBlendAlpha = D3D11_BLEND_ONE;
        blendConfig.RenderTarget[i].DestBlendAlpha = D3D11_BLEND_ONE;
        blendConfig.RenderTarget[i].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
    }

    m_iBlendState = Renderer.CreateBlendState( &blendConfig );

    if ( m_iBlendState == -1 )    
        Log::Get().Write( L"Failed to create light blend state" );    

    // Create a rasterizer state with back-face culling enabled
    RasterizerStateConfigDX11 rsConfig;
    rsConfig.MultisampleEnable = TRUE;    
    rsConfig.CullMode = D3D11_CULL_BACK;
    m_iBackFaceCullRSState = Renderer.CreateRasterizerState( &rsConfig );

    // Create a rasterizer state with front-face culling enabled
    rsConfig.CullMode = D3D11_CULL_FRONT;
    m_iFrontFaceCullRSState = Renderer.CreateRasterizerState( &rsConfig );

    if ( m_iBackFaceCullRSState == -1 || m_iFrontFaceCullRSState == -1 )
        Log::Get().Write( L"Failed to create light rasterizer state" );

    // Generate geometry for a full screen quad
    GeometryGeneratorDX11::GenerateFullScreenQuad( &m_QuadGeometry );
    m_QuadGeometry.LoadToBuffers();

    // Generate geometry for a sphere
    GeometryGeneratorDX11::GenerateSphere( &m_SphereGeometry, 8, 7, 1.0f );
    m_SphereGeometry.LoadToBuffers();

    // Generate geometry for a cone
    GeometryGeneratorDX11::GenerateCone( &m_ConeGeometry, 8, 2, 1.0f, 1.0f );
    m_ConeGeometry.LoadToBuffers();
                 
    // We need two versions of each effect, one with a pixel shader that runs
    // per-pixel and one with a pixel shader that runs per-sample
    for ( int i = 0; i < 2; ++i )
    {
        std::wstring psEntry = ( i == 0 ) ? L"PSMain" : L"PSMainPerSample";
        UINT stencilRef = ( i == 0 ) ? 1 : 2;

        // We'll create permutations of our shaders based on the light types
        D3D10_SHADER_MACRO defines[4];
        defines[0].Name = "POINTLIGHT";
        defines[0].Definition = "1";
        defines[1].Name = "SPOTLIGHT";
        defines[1].Definition = "0";
        defines[2].Name = "DIRECTIONALLIGHT";
        defines[2].Definition = "0";
        defines[3].Name = NULL;
        defines[3].Definition = NULL;                       

        // Point light shaders
        m_PointLightEffect[i].m_iVertexShader = 
            Renderer.LoadShader( VERTEX_SHADER,
            std::wstring( L"../Data/Shaders/LightsLP.hlsl" ),
            std::wstring( L"VSMain" ),
            std::wstring( L"vs_5_0" ),
            defines );
        _ASSERT( m_PointLightEffect[i].m_iVertexShader != -1 );

        m_PointLightEffect[i].m_iPixelShader =
            Renderer.LoadShader( PIXEL_SHADER,
            std::wstring( L"../Data/Shaders/LightsLP.hlsl" ),
            psEntry,
            std::wstring( L"ps_5_0" ),
            defines );        
        _ASSERT( m_PointLightEffect[i].m_iPixelShader != -1 );

        m_PointLightEffect[i].m_iBlendState = m_iBlendState;
        m_PointLightEffect[i].m_iDepthStencilState = m_iLessThanDSState;
        m_PointLightEffect[i].m_iRasterizerState = m_iBackFaceCullRSState;
        m_PointLightEffect[i].m_uStencilRef = stencilRef;

        // Spot light shaders
        defines[0].Definition = "0";
        defines[1].Definition = "1";

        m_SpotLightEffect[i].m_iVertexShader = 
            Renderer.LoadShader( VERTEX_SHADER,
            std::wstring( L"../Data/Shaders/LightsLP.hlsl" ),
            std::wstring( L"VSMain" ),
            std::wstring( L"vs_5_0" ),
            defines );
        _ASSERT( m_SpotLightEffect[i].m_iVertexShader != -1 );

        m_SpotLightEffect[i].m_iPixelShader =
            Renderer.LoadShader( PIXEL_SHADER,
            std::wstring( L"../Data/Shaders/LightsLP.hlsl" ),
            psEntry,
            std::wstring( L"ps_5_0" ),
            defines );        
        _ASSERT( m_SpotLightEffect[i].m_iPixelShader != -1 );

        m_SpotLightEffect[i].m_iBlendState = m_iBlendState;
        m_SpotLightEffect[i].m_iDepthStencilState = m_iLessThanDSState;
        m_SpotLightEffect[i].m_iRasterizerState = m_iBackFaceCullRSState;
        m_SpotLightEffect[i].m_uStencilRef = stencilRef;

        // Directional light shaders
        defines[1].Definition = "0";
        defines[2].Definition = "1";

        m_DirectionalLightEffect[i].m_iVertexShader = 
            Renderer.LoadShader( VERTEX_SHADER,
            std::wstring( L"../Data/Shaders/LightsLP.hlsl" ),
            std::wstring( L"VSMain" ),
            std::wstring( L"vs_5_0" ),
            defines );
        _ASSERT( m_DirectionalLightEffect[i].m_iVertexShader != -1 );

        m_DirectionalLightEffect[i].m_iPixelShader =
            Renderer.LoadShader( PIXEL_SHADER,
            std::wstring( L"../Data/Shaders/LightsLP.hlsl" ),
            psEntry,
            std::wstring( L"ps_5_0" ),
            defines );
        _ASSERT( m_DirectionalLightEffect[i].m_iPixelShader != -1 );

        m_DirectionalLightEffect[i].m_iBlendState = m_iBlendState;
        m_DirectionalLightEffect[i].m_iDepthStencilState = m_iDisabledDSState;
        m_DirectionalLightEffect[i].m_iRasterizerState = m_iBackFaceCullRSState;
        m_DirectionalLightEffect[i].m_uStencilRef = stencilRef;   
    }
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

    // Copy the depth buffer
    pPipelineManager->CopySubresourceRegion( m_DepthTargetCopy, 0, 0, 0, 0, m_DepthTarget, 0, NULL );

    // Set this view's render parameters
    SetRenderParams( pParamManager );

    // Loop through the lights
    for ( int i = 0; i < m_Lights.count(); ++i )
    {
        const Light& light = m_Lights[i];

        // Pick the effect based on the shader
        RenderEffectDX11* pEffect = NULL;
        if ( light.Type == Point )
            pEffect = m_PointLightEffect;
        else if ( light.Type == Spot )
            pEffect = m_SpotLightEffect;
        else if ( light.Type == Directional )
            pEffect = m_DirectionalLightEffect;

        // Set the light params
        Vector4f pos = Vector4f( light.Position, 1.0f );
        Vector4f direction = Vector4f( Vector3f::Normalize( light.Direction ), 0.0f );
   
        // Light position/direction needs to be in view space
        pos = ViewMatrix * pos;
        direction = ViewMatrix * direction;        

        pParamManager->SetVectorParameter( L"LightPos", &pos );
        pParamManager->SetVectorParameter( L"LightColor", &Vector4f( light.Color, 1.0f ) );
        pParamManager->SetVectorParameter( L"LightDirection", &direction );
        pParamManager->SetVectorParameter( L"LightRange", &Vector4f( light.Range, 1.0f, 1.0f, 1.0f ) );
        pParamManager->SetVectorParameter( L"SpotlightAngles", &Vector4f( cosf( light.SpotInnerAngle / 2.0f ),
                                                                          cosf( light.SpotOuterAngle / 2.0f ), 
                                                                          0.0f, 0.0f ) );

        // Set the rasterizer and depth-stencil state, and draw            
        bool intersectsNearPlane = true;
        bool intersectsFarPlane = true;
        if( light.Type != Directional )
        {
            // Try to determine whether the bounding sphere intersects with the clip planes
            Vector4f lightPosVS = ViewMatrix * Vector4f( light.Position, 1.0f );
            intersectsNearPlane = lightPosVS.z - light.Range <= m_fNearClip;
            intersectsFarPlane = lightPosVS.z + light.Range >= m_fFarClip;
        }

        if ( light.Type == Directional || ( intersectsNearPlane && intersectsFarPlane ) )
        {
            // Draw twice, first to shade non-edge pixels and then shade the edge-pixels            
            pPipelineManager->Draw( m_DirectionalLightEffect[0], m_QuadGeometry, pParamManager );
            pPipelineManager->Draw( m_DirectionalLightEffect[1], m_QuadGeometry, pParamManager );
        }
        else
        {
            // Set an appropriate world matrix for the volume
            m_WorldMatrix.MakeIdentity();

            if ( light.Type == Spot )
            {
                // Determine the scaling factors based on the attenuation
                const float scaleZ = light.Range * 1.1f;
                const float scaleXY = scaleZ * tanf( light.SpotOuterAngle / 2.0f );

                // Set the orientation based on the light direction
                Vector3f zAxis = Vector3f::Normalize( light.Direction );
                Vector3f yAxis = zAxis.Perpendicular();
                Vector3f xAxis = Vector3f::Cross( yAxis, zAxis );
                m_WorldMatrix.SetRow( 0, xAxis * scaleXY );
                m_WorldMatrix.SetRow( 1, yAxis * scaleXY );
                m_WorldMatrix.SetRow( 2, zAxis * scaleZ );
            }
            else if ( light.Type == Point )
            {
                // Determine the scaling factor based on the attenuation
                const float scaleXYZ = light.Range * 1.1f;
                m_WorldMatrix.Scale( scaleXYZ );
            }
            
            m_WorldMatrix.SetTranslation( light.Position );
            pParamManager->SetWorldMatrixParameter( &m_WorldMatrix );

            // Draw twice, first to shade non-edge pixels and then shade the edge-pixels
            for ( int i = 0; i < 2; ++i )
            {
                // Render back-faces, unless we intersect with the far clip plane
                if ( intersectsFarPlane )
                {
                    pEffect[i].m_iRasterizerState = m_iBackFaceCullRSState;
                    pEffect[i].m_iDepthStencilState = m_iLessThanDSState;
                }
                else
                {
                    pEffect[i].m_iRasterizerState = m_iFrontFaceCullRSState;
                    pEffect[i].m_iDepthStencilState = m_iGreaterThanDSState;
                }

                if ( light.Type == Spot )
                    pPipelineManager->Draw( pEffect[i], m_ConeGeometry, pParamManager );
                else if ( light.Type == Point )
                    pPipelineManager->Draw( pEffect[i], m_SphereGeometry, pParamManager );
            }
        }        
    }

    pPipelineManager->ClearPipelineResources();

    // Clear the lights
    int numLights = m_Lights.count();
    for ( int i = numLights - 1; i >= 0; --i )
        m_Lights.remove( i );
}
//--------------------------------------------------------------------------------
void ViewLights::SetRenderParams( ParameterManagerDX11* pParamManager )
{
    pParamManager->SetViewMatrixParameter( &ViewMatrix );
    pParamManager->SetProjMatrixParameter( &ProjMatrix );

    Matrix4f invProj = ProjMatrix.Inverse();
    pParamManager->SetMatrixParameter( L"InvProjMatrix", &invProj );
    pParamManager->SetMatrixParameter( L"ProjMatrix", &ProjMatrix );

    // Set the G-Buffer texture
    pParamManager->SetShaderResourceParameter( L"GBufferTexture", m_GBufferTarget );

    // Bind depth
    pParamManager->SetShaderResourceParameter( L"DepthTexture", m_DepthTargetCopy );
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
//--------------------------------------------------------------------------------
void ViewLights::SetTargets( ResourcePtr GBufferTarget, ResourcePtr pRenderTarget, 
                            ResourcePtr DepthTarget, ResourcePtr DepthTargetCopy, int Viewport )
{
    m_GBufferTarget = GBufferTarget;
    m_pRenderTarget = pRenderTarget;
    m_DepthTarget = DepthTarget;
    m_DepthTargetCopy = DepthTargetCopy;
    m_iViewport = Viewport;
}
//--------------------------------------------------------------------------------
void ViewLights::SetClipPlanes( float NearClip, float FarClip )
{
    m_fNearClip = NearClip;
    m_fFarClip = FarClip;
}
//--------------------------------------------------------------------------------
