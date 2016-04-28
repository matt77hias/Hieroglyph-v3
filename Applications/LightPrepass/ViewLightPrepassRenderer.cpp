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
#include "ViewLightPrepassRenderer.h"
#include "Entity3D.h"
#include "Scene.h"
#include "Texture2dConfigDX11.h"
#include "Log.h"
#include "ActorGenerator.h"
#include "IParameterManager.h"
#include "DepthStencilViewConfigDX11.h"
#include "ShaderResourceViewConfigDX11.h"
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
Vector3f Lerp( const Vector3f& x, const Vector3f& y, const Vector3f& s )
{
    return x + s * ( y - x );
}
//--------------------------------------------------------------------------------
ViewLightPrepassRenderer::ViewLightPrepassRenderer( RendererDX11& Renderer, ResourcePtr RenderTarget )
{
	m_BackBuffer = RenderTarget;

	D3D11_TEXTURE2D_DESC desc = m_BackBuffer->m_pTexture2dConfig->GetTextureDesc();

	ResolutionX = desc.Width;
	ResolutionY = desc.Height;

    // Create render targets

    DXGI_SAMPLE_DESC sampleDesc;
    sampleDesc.Count =  4;
    sampleDesc.Quality = 0;

    // Create the render targets for our optimized G-Buffer
    Texture2dConfigDX11 RTConfig;
    RTConfig.SetColorBuffer( ResolutionX, ResolutionY );
    RTConfig.SetBindFlags( D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET );
    RTConfig.SetSampleDesc( sampleDesc );

    // For the G-Buffer we'll use 4-component floating point format for
    // spheremap-encoded normals, specular albedo, and the coverage mask
    RTConfig.SetFormat( DXGI_FORMAT_R16G16B16A16_FLOAT );
    m_GBufferTarget = Renderer.CreateTexture2D( &RTConfig, NULL );

    // For the light buffer we'll use a 4-component floating point format
    // for storing diffuse RGB + mono specular
    RTConfig.SetFormat( DXGI_FORMAT_R16G16B16A16_FLOAT );
    m_LightTarget = Renderer.CreateTexture2D( &RTConfig, NULL );

    // We need one last render target for the final image
    RTConfig.SetFormat( DXGI_FORMAT_R10G10B10A2_UNORM );
    m_FinalTarget = Renderer.CreateTexture2D( &RTConfig, NULL );

    // Next we create a depth buffer for depth/stencil testing. Typeless formats let us
    // write depth with one format, and later interpret that depth as a color value using
    // a shader resource view.
    Texture2dConfigDX11 DepthTexConfig;
    DepthTexConfig.SetDepthBuffer( ResolutionX, ResolutionY );
    DepthTexConfig.SetFormat( DXGI_FORMAT_R24G8_TYPELESS );
    DepthTexConfig.SetBindFlags( D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE );
    DepthTexConfig.SetSampleDesc( sampleDesc );

    DepthStencilViewConfigDX11 DepthDSVConfig;
    D3D11_TEX2D_DSV DSVTex2D;
    DSVTex2D.MipSlice = 0;
    DepthDSVConfig.SetTexture2D( DSVTex2D );
    DepthDSVConfig.SetFormat( DXGI_FORMAT_D24_UNORM_S8_UINT );
    DepthDSVConfig.SetViewDimensions( D3D11_DSV_DIMENSION_TEXTURE2DMS );

    ShaderResourceViewConfigDX11 DepthSRVConfig;
    D3D11_TEX2D_SRV SRVTex2D;
    SRVTex2D.MipLevels = 1;
    SRVTex2D.MostDetailedMip = 0;
    DepthSRVConfig.SetTexture2D( SRVTex2D );
    DepthSRVConfig.SetFormat( DXGI_FORMAT_R24_UNORM_X8_TYPELESS );
    DepthSRVConfig.SetViewDimensions( D3D11_SRV_DIMENSION_TEXTURE2DMS );

    m_DepthTarget = Renderer.CreateTexture2D( &DepthTexConfig, NULL, &DepthSRVConfig, NULL, NULL, &DepthDSVConfig );

    // Now we need to create a depth stencil view with read-only flags set, so
    // that we can have the same buffer set as both a shader resource view and as
    // a depth stencil view
    DepthDSVConfig.SetFlags( D3D11_DSV_READ_ONLY_DEPTH | D3D11_DSV_READ_ONLY_STENCIL );
    m_ReadOnlyDepthTarget = ResourcePtr( new ResourceProxyDX11( m_DepthTarget->m_iResource,
                                                                &DepthTexConfig, RendererDX11::Get(),
                                                                &DepthSRVConfig, NULL, NULL,
                                                                &DepthDSVConfig ) );

    // Create a view port to use on the scene.  This basically selects the
    // entire floating point area of the render target.
    D3D11_VIEWPORT viewport;
    viewport.Width = static_cast< float >( ResolutionX );
    viewport.Height = static_cast< float >( ResolutionY );
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;
    viewport.TopLeftX = 0;
    viewport.TopLeftY = 0;

    SetViewPort( Renderer.CreateViewPort( viewport ) );

    // Create a render target for MSAA resolve
    Texture2dConfigDX11 resolveConfig;
    resolveConfig.SetColorBuffer( ResolutionX, ResolutionY );
    resolveConfig.SetFormat( DXGI_FORMAT_R10G10B10A2_UNORM );
    resolveConfig.SetBindFlags( D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET );
    m_ResolveTarget = Renderer.CreateTexture2D( &resolveConfig, NULL );


	// Create the three sub-views to perform the extra rendering operations for
	// ambient occlusion.
	m_pGBufferView = new ViewGBuffer( Renderer );
	m_pLightsView = new ViewLights( Renderer );
	m_pFinalPassView = new ViewFinalPass( Renderer );


	// Set the the targets for the views
    m_pGBufferView->SetTargets( m_GBufferTarget,
                                m_DepthTarget,
                                m_iViewports[0] );
    m_pLightsView->SetTargets( m_GBufferTarget,
                                m_LightTarget,
                                m_ReadOnlyDepthTarget,
                                m_iViewports[0] );
    m_pFinalPassView->SetTargets( m_LightTarget,
                                    m_FinalTarget,
                                    m_ReadOnlyDepthTarget,
                                    m_iViewports[0] );


    m_SpriteRenderer.Initialize();
}
//--------------------------------------------------------------------------------
ViewLightPrepassRenderer::~ViewLightPrepassRenderer()
{
	SAFE_DELETE( m_pGBufferView );
	SAFE_DELETE( m_pLightsView );
	SAFE_DELETE( m_pFinalPassView );
}
//--------------------------------------------------------------------------------
void ViewLightPrepassRenderer::Update( float fTime )
{
}
//--------------------------------------------------------------------------------
void ViewLightPrepassRenderer::QueuePreTasks( RendererDX11* pRenderer )
{
	// Call the super class's predraw in order to queue it in the renderer.  The
	// views are processed in a LIFO order, so this will be executed last in both
	// single- or multi-threaded mode.

	if ( m_pEntity != NULL )
	{
		Matrix4f view = m_pEntity->Transform.GetView();
		SetViewMatrix( view );
	}

	// Queue this view into the renderer for processing.
	pRenderer->QueueTask( this );

	if ( m_pScene )
	{
		// Run through the graph and pre-render the entities
		m_pScene->GetRoot()->PreRender( pRenderer, VT_PERSPECTIVE );
	}

	// Next we call the predraw method of each of the supporting views.

	SetupLights();
	m_pFinalPassView->QueuePreTasks( pRenderer );
	m_pLightsView->QueuePreTasks( pRenderer );
	m_pGBufferView->QueuePreTasks( pRenderer );
}
//--------------------------------------------------------------------------------
void ViewLightPrepassRenderer::ExecuteTask( PipelineManagerDX11* pPipelineManager, IParameterManager* pParamManager )
{
    // Render to the backbuffer
    pPipelineManager->ClearRenderTargets();
	pPipelineManager->OutputMergerStage.DesiredState.RenderTargetViews.SetState( 0, m_BackBuffer->m_iResourceRTV );
    pPipelineManager->ApplyRenderTargets();

    pPipelineManager->ClearBuffers( Vector4f( 0.0f, 0.0f, 0.0f, 0.0f ) );

	// Configure the desired viewports in this pipeline
	ConfigureViewports( pPipelineManager );

    // Need to resolve the MSAA target before we can render it
    pPipelineManager->ResolveSubresource( m_ResolveTarget, 0, m_FinalTarget, 0, DXGI_FORMAT_R10G10B10A2_UNORM );

    m_SpriteRenderer.Render( pPipelineManager, pParamManager, m_ResolveTarget, Matrix4f::Identity() );
}
//--------------------------------------------------------------------------------
void ViewLightPrepassRenderer::Resize( UINT width, UINT height )
{
	ResolutionX = width;
	ResolutionY = height;

	// First resize the depth target...
	RendererDX11::Get()->ResizeTexture( m_DepthTarget, width, height );

	// ...then the read only depth target.  In this case, we need to resize the
	// resource views manually instead of letting the RendererDX11::ResizeTexture
	// method do it for us.
	RendererDX11::Get()->ResizeTextureSRV( m_DepthTarget->m_iResource, m_ReadOnlyDepthTarget->m_iResourceSRV, width, height );
	RendererDX11::Get()->ResizeTextureDSV( m_DepthTarget->m_iResource, m_ReadOnlyDepthTarget->m_iResourceDSV, width, height );

	// Resize each of the remaining textures accordingly.
	RendererDX11::Get()->ResizeTexture( m_GBufferTarget, width, height );
	RendererDX11::Get()->ResizeTexture( m_LightTarget, width, height );
	RendererDX11::Get()->ResizeTexture( m_FinalTarget, width, height );
	RendererDX11::Get()->ResizeTexture( m_ResolveTarget, width, height );

	// Resize the viewport.
	RendererDX11::Get()->ResizeViewport( m_iViewports[0], width, height );

	m_pGBufferView->Resize( width, height );
	m_pLightsView->Resize( width, height );
	m_pFinalPassView->Resize( width, height );

	// Set the the targets for the views
    m_pGBufferView->SetViewPort( m_iViewports[0] );
    m_pLightsView->SetViewPort( m_iViewports[0] );
    m_pFinalPassView->SetViewPort( m_iViewports[0] );
}
//--------------------------------------------------------------------------------
void ViewLightPrepassRenderer::SetRenderParams( IParameterManager* pParamManager )
{
	// Set the parameters for this view to be able to perform its processing
	// sequence.  In this case, all of the render targets are already known to 
	// this view, since they are stored in this render view and passed on to the
	// sub-views.
}
//--------------------------------------------------------------------------------
void ViewLightPrepassRenderer::SetUsageParams( IParameterManager* pParamManager )
{
	// Set the parameters for allowing an application to use the current resources
	// for rendering.  This render view doesn't get used by any other views, so
	// there is no usage parameters to set.
}
//--------------------------------------------------------------------------------
void ViewLightPrepassRenderer::SetViewMatrix( const Matrix4f& matrix )
{
	// Perform the view matrix setting for this view.
	SceneRenderTask::SetViewMatrix( matrix );

	// Propagate the view matrix.
	m_pGBufferView->SetViewMatrix( matrix );
	m_pLightsView->SetViewMatrix( matrix );
	m_pFinalPassView->SetViewMatrix( matrix );
}
//--------------------------------------------------------------------------------
void ViewLightPrepassRenderer::SetProjMatrix( const Matrix4f& matrix )
{
	// Perform the projection matrix setting for this view.
	SceneRenderTask::SetProjMatrix( matrix );

	// Propagate the projection matrix.
	m_pGBufferView->SetProjMatrix( matrix );
	m_pLightsView->SetProjMatrix( matrix );
	m_pFinalPassView->SetProjMatrix( matrix );
}
//--------------------------------------------------------------------------------
void ViewLightPrepassRenderer::SetScene( Scene* pScene )
{
	// Perform the root setting call for this view.
	m_pScene = pScene;

	// Propagate the root setting call.
	m_pGBufferView->SetScene( pScene );
	m_pLightsView->SetScene( pScene );
	m_pFinalPassView->SetScene( pScene );
}
//--------------------------------------------------------------------------------
void ViewLightPrepassRenderer::SetEntity( Entity3D* pEntity )
{
	// Perform the entity setting call for this view.
	m_pEntity = pEntity;

	// Propagate the entity call.
	m_pGBufferView->SetEntity( pEntity );
	m_pLightsView->SetEntity( pEntity );
	m_pFinalPassView->SetEntity( pEntity );
}
//--------------------------------------------------------------------------------
void ViewLightPrepassRenderer::SetClipPlanes( float NearClip, float FarClip )
{
    m_fNearClip = NearClip;
    m_fFarClip = FarClip;

	m_pLightsView->SetClipPlanes( NearClip, FarClip );
}
//--------------------------------------------------------------------------------
void ViewLightPrepassRenderer::SetupLights( )
{
    // Set the lights to render
    LightParams light;
    light.Type = Point;
    light.Range = 2.0f;

    const int cubeSize = 3 + LightMode::Value * 2;
    const int cubeMin = -(cubeSize / 2);
    const int cubeMax = cubeSize / 2;

    const Vector3f minExtents ( -4.0f, 1.0f, -4.0f );
    const Vector3f maxExtents ( 4.0f, 11.0f, 4.0f );
    const Vector3f minColor ( 1.0f, 0.0f, 0.0f );
    const Vector3f maxColor ( 0.0f, 1.0f, 1.0f );

    for ( int x = cubeMin; x <= cubeMax; x++ )
    {
        for ( int y = cubeMin; y <= cubeMax; y++ )
        {
            for ( int z = cubeMin; z <= cubeMax; z++ )
            {
                Vector3f lerp;
                lerp.x = static_cast<float>( x - cubeMin ) / ( cubeSize - 1 );
                lerp.y = static_cast<float>( y - cubeMin ) / ( cubeSize - 1 );
                lerp.z = static_cast<float>( z - cubeMin ) / ( cubeSize - 1 );

                light.Position = Lerp( minExtents, maxExtents, lerp );
                light.Color = Lerp( minColor, maxColor, lerp ) * 1.5f;
                m_pLightsView->AddLight( light );
            }
        }
    }
}
//--------------------------------------------------------------------------------
std::wstring ViewLightPrepassRenderer::GetName()
{
	return( L"ViewLightPrepassRenderer" );
}
//--------------------------------------------------------------------------------