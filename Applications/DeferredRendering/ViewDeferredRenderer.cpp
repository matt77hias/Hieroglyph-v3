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
#include "ViewDeferredRenderer.h"
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
ViewDeferredRenderer::ViewDeferredRenderer( RendererDX11& Renderer, ResourcePtr RenderTarget )
{
	m_BackBuffer = RenderTarget;

	D3D11_TEXTURE2D_DESC desc = m_BackBuffer->m_pTexture2dConfig->GetTextureDesc();

	ResolutionX = desc.Width;
	ResolutionY = desc.Height;

    // Create render targets for all AA modes
    for ( int aaMode = 0; aaMode < AAMode::NumSettings; ++aaMode )
    {
        int rtWidth = ResolutionX;
        int rtHeight = ResolutionY;
        if ( aaMode == AAMode::SSAA )
        {
            rtWidth *= 2;
            rtHeight *= 2;
        }

        DXGI_SAMPLE_DESC sampleDesc;
        sampleDesc.Count = aaMode == AAMode::MSAA ? 4 : 1;
        sampleDesc.Quality = 0;

	    // Create the render targets for our unoptimized G-Buffer, which just uses
        // 32-bit floats for everything
	    Texture2dConfigDX11 RTConfig;
	    RTConfig.SetColorBuffer( rtWidth, rtHeight );
	    RTConfig.SetFormat( DXGI_FORMAT_R32G32B32A32_FLOAT );
        RTConfig.SetBindFlags( D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET );
        RTConfig.SetSampleDesc( sampleDesc );
	    for(int i = 0; i < 4; ++i)
            m_GBuffer[GBufferOptMode::OptDisabled][aaMode].push_back( Renderer.CreateTexture2D( &RTConfig, NULL ) );

        // Create the render targets for our optimized G-Buffer

        // 2-component signed normalized format for spheremap-encoded normals
        RTConfig.SetFormat( DXGI_FORMAT_R16G16_SNORM );
		m_GBuffer[GBufferOptMode::OptEnabled][aaMode].push_back( Renderer.CreateTexture2D( &RTConfig, NULL ) );

        // 3-component 10-bit unsigned normalized format for diffuse albedo
        RTConfig.SetFormat( DXGI_FORMAT_R10G10B10A2_UNORM );
		m_GBuffer[GBufferOptMode::OptEnabled][aaMode].push_back( Renderer.CreateTexture2D( &RTConfig, NULL ) );

        // 4-component 8-bit unsigned normalized format for specular albedo and power
        RTConfig.SetFormat( DXGI_FORMAT_R8G8B8A8_UNORM );
		m_GBuffer[GBufferOptMode::OptEnabled][aaMode].push_back( Renderer.CreateTexture2D( &RTConfig, NULL ) );

        // We need one last render target for the final image
        RTConfig.SetFormat( DXGI_FORMAT_R10G10B10A2_UNORM );
        m_FinalTarget[aaMode] = Renderer.CreateTexture2D( &RTConfig, NULL );

	    // Next we create a depth buffer for depth/stencil testing. Typeless formats let us
        // write depth with one format, and later interpret that depth as a color value using
        // a shader resource view.
	    Texture2dConfigDX11 DepthTexConfig;
	    DepthTexConfig.SetDepthBuffer( rtWidth, rtHeight );
        DepthTexConfig.SetFormat( DXGI_FORMAT_R24G8_TYPELESS );
        DepthTexConfig.SetBindFlags( D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE );
        DepthTexConfig.SetSampleDesc( sampleDesc );

        DepthStencilViewConfigDX11 DepthDSVConfig;
        D3D11_TEX2D_DSV DSVTex2D;
        DSVTex2D.MipSlice = 0;
        DepthDSVConfig.SetTexture2D( DSVTex2D );
        DepthDSVConfig.SetFormat( DXGI_FORMAT_D24_UNORM_S8_UINT );

        ShaderResourceViewConfigDX11 DepthSRVConfig;
        D3D11_TEX2D_SRV SRVTex2D;
        SRVTex2D.MipLevels = 1;
        SRVTex2D.MostDetailedMip = 0;
        DepthSRVConfig.SetTexture2D( SRVTex2D );
        DepthSRVConfig.SetFormat( DXGI_FORMAT_R24_UNORM_X8_TYPELESS );

        if ( aaMode == AAMode::MSAA )
        {
            DepthSRVConfig.SetViewDimensions( D3D11_SRV_DIMENSION_TEXTURE2DMS );
            DepthDSVConfig.SetViewDimensions( D3D11_DSV_DIMENSION_TEXTURE2DMS );
        }
        else
        {
            DepthSRVConfig.SetViewDimensions( D3D11_SRV_DIMENSION_TEXTURE2D );
            DepthDSVConfig.SetViewDimensions( D3D11_DSV_DIMENSION_TEXTURE2D );
        }

        m_DepthTarget[aaMode] = Renderer.CreateTexture2D( &DepthTexConfig, NULL, &DepthSRVConfig, NULL, NULL, &DepthDSVConfig );

        // Now we need to create a depth stencil view with read-only flags set, so
        // that we can have the same buffer set as both a shader resource view and as
        // a depth stencil view
        DepthDSVConfig.SetFlags( D3D11_DSV_READ_ONLY_DEPTH | D3D11_DSV_READ_ONLY_STENCIL );
        m_ReadOnlyDepthTarget[aaMode] = ResourcePtr( new ResourceProxyDX11( m_DepthTarget[aaMode]->m_iResource,
                                                                            &DepthTexConfig, RendererDX11::Get(),
                                                                            &DepthSRVConfig, NULL, NULL,
                                                                            &DepthDSVConfig ) );

        // Create a view port to use on the scene.  This basically selects the
        // entire floating point area of the render target.
        D3D11_VIEWPORT viewport;
        viewport.Width = static_cast< float >( rtWidth );
        viewport.Height = static_cast< float >( rtHeight );
        viewport.MinDepth = 0.0f;
        viewport.MaxDepth = 1.0f;
        viewport.TopLeftX = 0;
        viewport.TopLeftY = 0;

        m_iViewport[aaMode] = Renderer.CreateViewPort( viewport );
    }

    // Create a render target for MSAA resolve
    Texture2dConfigDX11 RTConfig;
    RTConfig.SetColorBuffer( ResolutionX, ResolutionY);
    RTConfig.SetFormat( DXGI_FORMAT_R10G10B10A2_UNORM );
    RTConfig.SetBindFlags( D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET );
    m_ResolveTarget = Renderer.CreateTexture2D( &RTConfig, NULL );

	// Create the two sub-views to perform the extra rendering operations for
	// ambient occlusion.

	m_pGBufferView = new ViewGBuffer( Renderer );
	m_pLightsView = new ViewLights( Renderer );

    m_SpriteRenderer.Initialize();
	m_pFont = SpriteFontLoaderDX11::LoadFont( std::wstring( L"Arial" ), 14, 0, true );
}
//--------------------------------------------------------------------------------
ViewDeferredRenderer::~ViewDeferredRenderer()
{
	SAFE_DELETE( m_pGBufferView );
	SAFE_DELETE( m_pLightsView );
}
//--------------------------------------------------------------------------------
void ViewDeferredRenderer::Update( float fTime )
{
}
//--------------------------------------------------------------------------------
void ViewDeferredRenderer::QueuePreTasks( RendererDX11* pRenderer )
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

	SetupViews();
	m_pLightsView->QueuePreTasks( pRenderer );
	m_pGBufferView->QueuePreTasks( pRenderer );
}
//--------------------------------------------------------------------------------
void ViewDeferredRenderer::ExecuteTask( PipelineManagerDX11* pPipelineManager, IParameterManager* pParamManager )
{
	// Here we select what will be visible on the final rendered image based on 
	// the current options that have been selected.  This includes performing a
	// resolve if necesary for MSAA render targets, or producing a composite 
	// image from several of our existing render targets.


	// Bind the back buffer and clear it
    pPipelineManager->ClearRenderTargets();
	pPipelineManager->OutputMergerStage.DesiredState.RenderTargetViews.SetState( 0, m_BackBuffer->m_iResourceRTV );
    pPipelineManager->ApplyRenderTargets();
    pPipelineManager->ClearBuffers( Vector4f( 0.0f, 0.0f, 0.0f, 0.0f ) );

	// Configure the desired viewports in this pipeline
	ConfigureViewports( pPipelineManager );

    std::vector<ResourcePtr>& gBuffer = m_GBuffer[GBufferOptMode::Value][AAMode::Value];
    float scaleFactor = AAMode::Value == AAMode::SSAA ? 0.5f : 1.0f;

    if ( DisplayMode::Value == DisplayMode::Final )
    {
        ResourcePtr target = m_FinalTarget[AAMode::Value];
        if ( AAMode::Value == AAMode::MSAA )
        {
            // Need to resolve the MSAA target before we can render it
            pPipelineManager->ResolveSubresource( m_ResolveTarget, 0, target, 0, DXGI_FORMAT_R10G10B10A2_UNORM );
            target = m_ResolveTarget;
        }

        m_SpriteRenderer.Render( pPipelineManager, pParamManager, target, Matrix4f::ScaleMatrix( scaleFactor ) );
    }
    else
    {
        if ( AAMode::Value == AAMode::MSAA )
        {
            std::wstring text = L"Unable to view G-Buffers while MSAA is enabled";
            Matrix4f mat = Matrix4f::Identity();
            mat.SetTranslation( Vector3f( 500.0f, 350.0f, 0.0f ) );
            m_SpriteRenderer.RenderText( pPipelineManager, pParamManager, m_pFont, text.c_str(), mat );
        }
        else
        {
            if ( DisplayMode::Value == DisplayMode::GBuffer )
            {
                Matrix4f mat = Matrix4f::Identity();
                mat = Matrix4f::ScaleMatrix( 0.5f * scaleFactor ) *  Matrix4f::TranslationMatrix(0, 0, 0);
                m_SpriteRenderer.Render( pPipelineManager, pParamManager, gBuffer[0], mat );

                mat = Matrix4f::ScaleMatrix( 0.5f * scaleFactor ) *  Matrix4f::TranslationMatrix(640, 0, 0);
                m_SpriteRenderer.Render( pPipelineManager, pParamManager, gBuffer[1], mat );

                mat = Matrix4f::ScaleMatrix( 0.5f * scaleFactor ) *  Matrix4f::TranslationMatrix(0, 360, 0);
                m_SpriteRenderer.Render( pPipelineManager, pParamManager, gBuffer[2], mat );

                mat = Matrix4f::ScaleMatrix( 0.5f * scaleFactor ) *  Matrix4f::TranslationMatrix(640, 360, 0);

                if ( GBufferOptMode::Enabled() )
                    m_SpriteRenderer.Render( pPipelineManager, pParamManager, m_DepthTarget[AAMode::Value], mat );
                else
                    m_SpriteRenderer.Render( pPipelineManager, pParamManager, gBuffer[3], mat );
            }
            else
            {
                ResourcePtr target;
                if ( GBufferOptMode::Enabled() && DisplayMode::Value == DisplayMode::Position )
                    target = m_DepthTarget[AAMode::Value];
                else
                    target = gBuffer[ DisplayMode::Value - DisplayMode::Normals ];

                m_SpriteRenderer.Render( pPipelineManager, pParamManager, target, Matrix4f::ScaleMatrix( scaleFactor ) );
            }
        }
    }
}
//--------------------------------------------------------------------------------
void ViewDeferredRenderer::Resize( UINT width, UINT height )
{
	RendererDX11* pRenderer = RendererDX11::Get();

	// Remember the new dimensions of the render view.
	ResolutionX = width;
	ResolutionY = height;

    // Resize the resources for all AA modes.
    for ( int aaMode = 0; aaMode < AAMode::NumSettings; ++aaMode )
    {
        int rtWidth = ResolutionX;
        int rtHeight = ResolutionY;

		// Super-sampling requires a larger set of render targets.
        if ( aaMode == AAMode::SSAA ) {
            rtWidth *= 2;
            rtHeight *= 2;
        }

		// First resize the depth target...
		pRenderer->ResizeTexture( m_DepthTarget[aaMode], rtWidth, rtHeight );

		// ...then the read only depth target.  In this case, we need to resize the
		// resource views manually instead of letting the RendererDX11::ResizeTexture
		// method do it for us.
		pRenderer->ResizeTextureSRV( m_DepthTarget[aaMode]->m_iResource, m_ReadOnlyDepthTarget[aaMode]->m_iResourceSRV, rtWidth, rtHeight );
		pRenderer->ResizeTextureDSV( m_DepthTarget[aaMode]->m_iResource, m_ReadOnlyDepthTarget[aaMode]->m_iResourceDSV, rtWidth, rtHeight );

		// Resize each of the G-Buffers for optimization disabled mode.
		for( unsigned int i = 0; i < m_GBuffer[GBufferOptMode::OptDisabled][aaMode].size(); ++i ) {
            pRenderer->ResizeTexture( m_GBuffer[GBufferOptMode::OptDisabled][aaMode][i], rtWidth, rtHeight );
		}
        
		// Resize each of the G-Buffers for optimization enabled mode.
		for ( unsigned int i = 0; i < m_GBuffer[GBufferOptMode::OptEnabled][aaMode].size(); ++i ) {
	        pRenderer->ResizeTexture( m_GBuffer[GBufferOptMode::OptEnabled][aaMode][i], rtWidth, rtHeight );
		}

		// Resize the final render target.
		pRenderer->ResizeTexture( m_FinalTarget[aaMode], rtWidth, rtHeight );

		// Resize the viewport.
		pRenderer->ResizeViewport( m_iViewport[aaMode], rtWidth, rtHeight );
    }

	// Resize the resolve render target.
    pRenderer->ResizeTexture( m_ResolveTarget, ResolutionX, ResolutionY );

	// Notify each of the sub-render views that they must resize.  In this case,
	// they don't do anything, but should be called anyway to ensure correct
	// behavior in the future.
	m_pGBufferView->Resize( width, height );
	m_pLightsView->Resize( width, height );
}
//--------------------------------------------------------------------------------
void ViewDeferredRenderer::SetRenderParams( IParameterManager* pParamManager )
{
	// Set the parameters for this view to be able to perform its processing
	// sequence.  In this case, all of the render targets are already known to 
	// this view, since they are stored in this render view and passed on to the
	// sub-views.
}
//--------------------------------------------------------------------------------
void ViewDeferredRenderer::SetUsageParams( IParameterManager* pParamManager )
{
	// Set the parameters for allowing an application to use the current resources
	// for rendering.  This render view doesn't get used by any other views, so
	// there is no usage parameters to set.
}
//--------------------------------------------------------------------------------
void ViewDeferredRenderer::SetViewMatrix( const Matrix4f& matrix )
{
	// Perform the view matrix setting for this view.
	SceneRenderTask::SetViewMatrix( matrix );

	// Propagate the view matrix.
	m_pGBufferView->SetViewMatrix( matrix );
	m_pLightsView->SetViewMatrix( matrix );
}
//--------------------------------------------------------------------------------
void ViewDeferredRenderer::SetProjMatrix( const Matrix4f& matrix )
{
	// Perform the projection matrix setting for this view.
	SceneRenderTask::SetProjMatrix( matrix );

	// Propagate the projection matrix.
	m_pGBufferView->SetProjMatrix( matrix );
	m_pLightsView->SetProjMatrix( matrix );
}
//--------------------------------------------------------------------------------
void ViewDeferredRenderer::SetScene( Scene* pScene )
{
	// Perform the root setting call for this view.
	m_pScene = pScene;

	// Propagate the root setting call.
	m_pGBufferView->SetScene( pScene );
	m_pLightsView->SetScene( pScene );
}
//--------------------------------------------------------------------------------
void ViewDeferredRenderer::SetEntity( Entity3D* pEntity )
{
	// Perform the entity setting call for this view.
	m_pEntity = pEntity;

	// Propagate the entity call.
	m_pGBufferView->SetEntity( pEntity );
	m_pLightsView->SetEntity( pEntity );
}
//--------------------------------------------------------------------------------
void ViewDeferredRenderer::SetClipPlanes( float NearClip, float FarClip )
{
    m_fNearClip = NearClip;
    m_fFarClip = FarClip;

	m_pLightsView->SetClipPlanes( NearClip, FarClip );
}
//--------------------------------------------------------------------------------
void ViewDeferredRenderer::SetupViews( )
{
    // Set the lights to render. We'll create a 3D grid of point lights
    // with different colors.
    LightParams light;
    light.Type = Point;

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
                m_pLightsView->AddLight(light);
            }
        }
    }

    int vpWidth = ResolutionX;
    int vpHeight = ResolutionY;
    if ( AAMode::Value == AAMode::SSAA )
    {
        vpWidth *= 2;
        vpHeight *= 2;
    }


    // Configure all of the view resources according to the options
	// that have been selected.
	SetViewPort( m_iViewport[AAMode::Value] );

    m_pGBufferView->SetTargets( m_GBuffer[GBufferOptMode::Value][AAMode::Value],
                                m_DepthTarget[AAMode::Value],
                                m_iViewport[AAMode::Value] );
    m_pLightsView->SetTargets( m_GBuffer[GBufferOptMode::Value][AAMode::Value],
                               m_FinalTarget[AAMode::Value],
                               m_ReadOnlyDepthTarget[AAMode::Value],
                               m_iViewport[AAMode::Value], vpWidth, vpHeight );
}
//--------------------------------------------------------------------------------
std::wstring ViewDeferredRenderer::GetName()
{
	return( L"ViewDeferredRenderer" );
}
//--------------------------------------------------------------------------------
