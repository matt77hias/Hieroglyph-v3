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
#include "ViewLights.h"
#include "Entity3D.h"
#include "Node3D.h"
#include "Texture2dConfigDX11.h"
#include "Log.h"
#include "IParameterManager.h"
#include "PipelineManagerDX11.h"
#include "Texture2dDX11.h"
#include "DepthStencilStateConfigDX11.h"
#include "ViewGBuffer.h"
#include "GeometryGeneratorDX11.h"
#include "BlendStateConfigDX11.h"
#include "AppSettings.h"
#include "RasterizerStateConfigDX11.h"
#include "BufferConfigDX11.h"
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
static void DrawLightType( const std::vector<LightParams>& lights, RenderEffectDX11 effects[2], 
                            ResourcePtr vb, int inputLayout, PipelineManagerDX11* pPipelineManager,
                            IParameterManager* pParamManager )
{
    if ( lights.size() > 0 )
    {
        // Copy in the light data for each vertex
        D3D11_MAPPED_SUBRESOURCE mapped;
        mapped = pPipelineManager->MapResource( vb->m_iResource, 0, D3D11_MAP_WRITE_DISCARD, 0 );
        memcpy( mapped.pData, &lights[0], sizeof( LightParams ) * lights.size() );
        pPipelineManager->UnMapResource( vb->m_iResource, 0 );

        // Render the lights for non-edge pixels. We'll render as a point list.
        pPipelineManager->DrawNonIndexed( effects[0], vb, inputLayout,
                                            D3D11_PRIMITIVE_TOPOLOGY_POINTLIST, sizeof( LightParams ),
                                            lights.size(), 0, pParamManager );

        // Render the lights for edge pixels. We'll render as a point list.
        pPipelineManager->DrawNonIndexed( effects[1], vb, inputLayout, 
                                            D3D11_PRIMITIVE_TOPOLOGY_POINTLIST, sizeof( LightParams ),
                                            lights.size(), 0, pParamManager );
    }
}
//--------------------------------------------------------------------------------
ViewLights::ViewLights( RendererDX11& Renderer)
{
    ViewMatrix.MakeIdentity();
    ProjMatrix.MakeIdentity();

    // Create a depth stencil state with no depth testing, and with stencil testing
    // enabled to make sure we only light pixels where we rendered to the G-Buffer
    DepthStencilStateConfigDX11 dsConfig;
    dsConfig.DepthEnable = FALSE;
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
    dsConfig.DepthEnable = TRUE;
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

    if ( m_iBackFaceCullRSState == -1 )
        Log::Get().Write( L"Failed to create light rasterizer state" );

    // Create a dynamic vertex buffer full of points, where each point is a single light source
    BufferConfigDX11 vbConfig;
    vbConfig.SetBindFlags( D3D11_BIND_VERTEX_BUFFER );
    vbConfig.SetByteWidth( MaxNumLights * sizeof(LightParams) );
    vbConfig.SetCPUAccessFlags( D3D11_CPU_ACCESS_WRITE );
    vbConfig.SetUsage( D3D11_USAGE_DYNAMIC );
    m_pVertexBuffer = Renderer.CreateVertexBuffer( &vbConfig, NULL );

    if( m_pVertexBuffer->m_iResource == -1 )
        Log::Get().Write( L"Failed to create light vertex buffer" );   

    // We need two versions of each effect, one with a pixel shader that runs
    // per-pixel and one with a pixel shader that runs per-sample
    for ( int i = 0; i < 2; ++i )
    {
        std::wstring psEntry = ( i == 0 ) ? L"PSMain" : L"PSMainPerSample";
        UINT stencilRef = ( i == 0 ) ? 1 : 2;

        // We'll create permutations of our shaders based on the light types
        D3D_SHADER_MACRO defines[4];
        defines[0].Name = "POINTLIGHT";
        defines[0].Definition = "1";
        defines[1].Name = "SPOTLIGHT";
        defines[1].Definition = "0";
        defines[2].Name = "DIRECTIONALLIGHT";
        defines[2].Definition = "0";
        defines[3].Name = NULL;
        defines[3].Definition = NULL;

        // Point light shaders
        m_PointLightEffect[i].SetVertexShader( Renderer.LoadShader( VERTEX_SHADER,
            std::wstring( L"LightsLP.hlsl" ),
            std::wstring( L"VSMain" ),
            std::wstring( L"vs_5_0" ),
            defines ) );
        _ASSERT( m_PointLightEffect[i].GetVertexShader() != -1 );

        m_PointLightEffect[i].SetGeometryShader( Renderer.LoadShader( GEOMETRY_SHADER,
            std::wstring( L"LightsLP.hlsl" ),
            std::wstring( L"GSMain" ),
            std::wstring( L"gs_5_0" ),
            defines ) );
        _ASSERT( m_PointLightEffect[i].GetGeometryShader() != -1 );

        m_PointLightEffect[i].SetPixelShader( Renderer.LoadShader( PIXEL_SHADER,
            std::wstring( L"LightsLP.hlsl" ),
            psEntry,
            std::wstring( L"ps_5_0" ),
            defines ) );
        _ASSERT( m_PointLightEffect[i].GetPixelShader() != -1 );

        m_PointLightEffect[i].m_iBlendState = m_iBlendState;
        m_PointLightEffect[i].m_iDepthStencilState = m_iGreaterThanDSState;
        m_PointLightEffect[i].m_iRasterizerState = m_iBackFaceCullRSState;
        m_PointLightEffect[i].m_uStencilRef = stencilRef;

        // Spot light shaders
        defines[0].Definition = "0";
        defines[1].Definition = "1";

        m_SpotLightEffect[i].SetVertexShader( Renderer.LoadShader( VERTEX_SHADER,
            std::wstring( L"LightsLP.hlsl" ),
            std::wstring( L"VSMain" ),
            std::wstring( L"vs_5_0" ),
            defines ) );
        _ASSERT( m_SpotLightEffect[i].GetVertexShader() != -1 );

        m_SpotLightEffect[i].SetGeometryShader( Renderer.LoadShader( GEOMETRY_SHADER,
            std::wstring( L"LightsLP.hlsl" ),
            std::wstring( L"GSMain" ),
            std::wstring( L"gs_5_0" ),
            defines ) );
        _ASSERT( m_SpotLightEffect[i].GetGeometryShader() != -1 );

        m_SpotLightEffect[i].SetPixelShader( Renderer.LoadShader( PIXEL_SHADER,
            std::wstring( L"LightsLP.hlsl" ),
            psEntry,
            std::wstring( L"ps_5_0" ),
            defines ) );
        _ASSERT( m_SpotLightEffect[i].GetPixelShader() != -1 );

        m_SpotLightEffect[i].m_iBlendState = m_iBlendState;
        m_SpotLightEffect[i].m_iDepthStencilState = m_iGreaterThanDSState;
        m_SpotLightEffect[i].m_iRasterizerState = m_iBackFaceCullRSState;
        m_SpotLightEffect[i].m_uStencilRef = stencilRef;

        // Directional light shaders
        defines[1].Definition = "0";
        defines[2].Definition = "1";

        m_DirectionalLightEffect[i].SetVertexShader( Renderer.LoadShader( VERTEX_SHADER,
            std::wstring( L"LightsLP.hlsl" ),
            std::wstring( L"VSMain" ),
            std::wstring( L"vs_5_0" ),
            defines ) );
        _ASSERT( m_DirectionalLightEffect[i].GetVertexShader() != -1 );

        m_DirectionalLightEffect[i].SetGeometryShader( Renderer.LoadShader( GEOMETRY_SHADER,
            std::wstring( L"LightsLP.hlsl" ),
            std::wstring( L"GSMain" ),
            std::wstring( L"gs_5_0" ),
            defines ) );
        _ASSERT( m_DirectionalLightEffect[i].GetGeometryShader() != -1 );

        m_DirectionalLightEffect[i].SetPixelShader( Renderer.LoadShader( PIXEL_SHADER,
            std::wstring( L"LightsLP.hlsl" ),
            psEntry,
            std::wstring( L"ps_5_0" ),
            defines ) );
        _ASSERT( m_DirectionalLightEffect[i].GetPixelShader() != -1 );

        m_DirectionalLightEffect[i].m_iBlendState = m_iBlendState;
        m_DirectionalLightEffect[i].m_iDepthStencilState = m_iDisabledDSState;
        m_DirectionalLightEffect[i].m_iRasterizerState = m_iBackFaceCullRSState;
        m_DirectionalLightEffect[i].m_uStencilRef = stencilRef;
    }

    // Create the input layouts
    // Create the input layout
    std::vector<D3D11_INPUT_ELEMENT_DESC> inputElements;
    D3D11_INPUT_ELEMENT_DESC element;

    // Position
    element.SemanticName = "POSITION";
    element.SemanticIndex = 0;
    element.Format = DXGI_FORMAT_R32G32B32_FLOAT;
    element.InputSlot = 0;
    element.AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
    element.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
    element.InstanceDataStepRate = 0;
	inputElements.push_back( element);

    // Color
    element.SemanticName = "COLOR";
    inputElements.push_back( element );

    // Direction
    element.SemanticName = "DIRECTION";
    inputElements.push_back( element );

    // Range
    element.SemanticName = "RANGE";
    element.Format = DXGI_FORMAT_R32_FLOAT;
    inputElements.push_back( element );

    // Spotlight angles
    element.SemanticName = "SPOTANGLES";
    element.Format = DXGI_FORMAT_R32G32_FLOAT;
    inputElements.push_back( element) ; 

    m_iPointLightIL = Renderer.CreateInputLayout( inputElements, m_PointLightEffect[0].GetVertexShader() );
    m_iSpotLightIL = Renderer.CreateInputLayout( inputElements, m_SpotLightEffect[0].GetVertexShader() );
    m_iDirectionalLightIL = Renderer.CreateInputLayout( inputElements, m_DirectionalLightEffect[0].GetVertexShader() );

    if( m_iPointLightIL == -1 || m_iSpotLightIL == -1 || m_iDirectionalLightIL == -1 )
        Log::Get().Write( L"Failed to create light input layout" );

	m_pInvProjMatrix = Renderer.m_pParamMgr->GetMatrixParameterRef( std::wstring( L"InvProjMatrix" ) );
	m_pProjMatrix = Renderer.m_pParamMgr->GetMatrixParameterRef( std::wstring( L"ProjMatrix" ) );

	m_pClipPlanes = Renderer.m_pParamMgr->GetVectorParameterRef( std::wstring( L"ClipPlanes" ) );

	m_pGBufferTexture = Renderer.m_pParamMgr->GetShaderResourceParameterRef( std::wstring( L"GBufferTexture" ) );
	m_pDepthTexture = Renderer.m_pParamMgr->GetShaderResourceParameterRef( std::wstring( L"DepthTexture" ) );

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
void ViewLights::QueuePreTasks( RendererDX11* pRenderer )
{
    if ( m_pEntity != NULL )
    {
        Matrix4f view = m_pEntity->Transform.GetView();
        SetViewMatrix( view );
    }

    // Queue this view into the renderer for processing.
    pRenderer->QueueTask( this );
}
//--------------------------------------------------------------------------------
void ViewLights::ExecuteTask( PipelineManagerDX11* pPipelineManager, IParameterManager* pParamManager )
{
    // Bind the render target
    pPipelineManager->ClearRenderTargets();
	pPipelineManager->OutputMergerStage.DesiredState.RenderTargetViews.SetState( 0, m_pRenderTarget->m_iResourceRTV );
    pPipelineManager->ApplyRenderTargets();

    // Clear the render target
    Vector4f color(0.0f, 0.0f, 0.0f, 0.0f);
    pPipelineManager->ClearBuffers( color, 1.0f, 0 );

    // Bind the depth buffer
	pPipelineManager->OutputMergerStage.DesiredState.DepthTargetViews.SetState( m_pDepthTarget->m_iResourceDSV );
    pPipelineManager->ApplyRenderTargets();

	pPipelineManager->RasterizerStage.DesiredState.ViewportCount.SetState( 1 );
	pPipelineManager->RasterizerStage.DesiredState.Viewports.SetState( 0, m_iViewport );

    // Set this view's render parameters
    SetRenderParams( pParamManager );
    
    // Render each light type
    DrawLightType( m_PointLights, m_PointLightEffect, m_pVertexBuffer, 
                        m_iPointLightIL, pPipelineManager, pParamManager );

    DrawLightType( m_SpotLights, m_SpotLightEffect, m_pVertexBuffer, 
                        m_iSpotLightIL, pPipelineManager, pParamManager );

    DrawLightType( m_DirectionalLights, m_DirectionalLightEffect, m_pVertexBuffer, 
                        m_iDirectionalLightIL, pPipelineManager, pParamManager );

    pPipelineManager->ClearPipelineResources();

    // Clear the lights
    m_PointLights.clear();
    m_SpotLights.clear();
    m_DirectionalLights.clear();
}
//--------------------------------------------------------------------------------
void ViewLights::Resize( UINT width, UINT height )
{
	// Nothing needed here, since the main render view handles the resizing of
	// the resources and the viewports.
}
//--------------------------------------------------------------------------------
void ViewLights::SetRenderParams( IParameterManager* pParamManager )
{
    pParamManager->SetViewMatrixParameter( &ViewMatrix );
    pParamManager->SetProjMatrixParameter( &ProjMatrix );

    Matrix4f invProj = ProjMatrix.Inverse();
    pParamManager->SetMatrixParameter( m_pInvProjMatrix, &invProj );
    pParamManager->SetMatrixParameter( m_pProjMatrix, &ProjMatrix );

    pParamManager->SetVectorParameter( m_pClipPlanes, &Vector4f( m_fNearClip, m_fFarClip, 1.0f, 1.0f ) );    

    // Set the G-Buffer texture
    pParamManager->SetShaderResourceParameter( m_pGBufferTexture, m_pGBufferTarget );

    // Bind depth
    pParamManager->SetShaderResourceParameter( m_pDepthTexture, m_pDepthTarget );
}
//--------------------------------------------------------------------------------
void ViewLights::SetUsageParams( IParameterManager* pParamManager )
{

}
//--------------------------------------------------------------------------------
void ViewLights::AddLight( const LightParams& light )
{
    if ( light.Type == Point )
        m_PointLights.push_back( light );
    else if ( light.Type == Spot )
        m_SpotLights.push_back( light );
    else if ( light.Type == Directional )
        m_DirectionalLights.push_back( light );    
}
//--------------------------------------------------------------------------------
void ViewLights::SetTargets( ResourcePtr GBufferTarget, ResourcePtr pRenderTarget,
                            ResourcePtr DepthTarget, int Viewport )
{
    m_pGBufferTarget = GBufferTarget;
    m_pRenderTarget = pRenderTarget;
    m_pDepthTarget = DepthTarget;
    m_iViewport = Viewport;
}
//--------------------------------------------------------------------------------
void ViewLights::SetClipPlanes( float NearClip, float FarClip )
{
    m_fNearClip = NearClip;
    m_fFarClip = FarClip;
}
//--------------------------------------------------------------------------------
std::wstring ViewLights::GetName()
{
	return( L"ViewLights" );
}
//--------------------------------------------------------------------------------
