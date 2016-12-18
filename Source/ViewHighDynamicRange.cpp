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
#include "ViewHighDynamicRange.h"
#include "Entity3D.h"
#include "Scene.h"
#include "Texture2dConfigDX11.h"
#include "Log.h"
#include "IParameterManager.h"
#include "PipelineManagerDX11.h"
#include "Texture2dDX11.h"
#include "BoundsVisualizerActor.h"
#include "SceneGraph.h"
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
ViewHighDynamicRange::ViewHighDynamicRange( RendererDX11& Renderer, ResourcePtr RenderTarget, float threshold, ResourcePtr DepthTarget ) :
	processor(),
	fullscreen( ResourcePtr(nullptr) ),
	m_threshold( threshold )
{
	SetRenderTargets( RenderTarget, DepthTarget );

	MaterialPtr pMaterial = MaterialPtr( new MaterialDX11() );
	RenderEffectDX11* pEffect = new RenderEffectDX11();

	pEffect->SetVertexShader( 
		RendererDX11::Get()->LoadShader( VERTEX_SHADER,
			std::wstring( L"FullscreenTwoTextures.hlsl" ),
			std::wstring( L"VSMAIN" ),
			std::wstring( L"vs_4_0" ) ) );

	pEffect->SetPixelShader( 
		RendererDX11::Get()->LoadShader( PIXEL_SHADER,
			std::wstring( L"FullscreenTwoTextures.hlsl" ),
			std::wstring( L"PSMAIN" ),
			std::wstring( L"ps_4_0" ) ) );

	pMaterial->Params[VT_PERSPECTIVE].bRender = true;
	pMaterial->Params[VT_PERSPECTIVE].pEffect = pEffect;

	fullscreen.GetBody()->Visual.SetMaterial( pMaterial );
}
//--------------------------------------------------------------------------------
ViewHighDynamicRange::~ViewHighDynamicRange()
{
}
//--------------------------------------------------------------------------------
void ViewHighDynamicRange::SetRenderTargets( ResourcePtr RenderTarget, ResourcePtr DepthTarget )
{
	// Update the render target to the desired resource. We can't delete the 
	// render target here because they are often shared, and not owned by this 
	// object. Instead, we simply overwrite it and let the owner of the resource
	// manage its lifetime.

	m_RenderTarget = RenderTarget;
	
	// If there is already a depth target, release it from the renderer.
	if ( nullptr != m_DepthTarget ) {
		RendererDX11::Get()->DeleteResource( m_DepthTarget );
		m_DepthTarget = nullptr;
	}

	if ( nullptr != m_RenderTarget )
	{
		ResourceDX11* pResource = RendererDX11::Get()->GetResourceByIndex( m_RenderTarget->m_iResource );

		if ( pResource->GetType() == RT_TEXTURE2D )
		{
			Texture2dDX11* pTexture = (Texture2dDX11*)pResource;
			D3D11_TEXTURE2D_DESC desc = pTexture->GetActualDescription();

			m_width = desc.Width;
			m_height = desc.Height;

			// Next we create a depth buffer for use in the traditional rendering
			// pipeline.
			if ( DepthTarget != NULL ) {
				m_DepthTarget = DepthTarget;
			} else {
				Texture2dConfigDX11 DepthConfig;
				DepthConfig.SetDepthBuffer( m_width, m_height );
				m_DepthTarget = RendererDX11::Get()->CreateTexture2D( &DepthConfig, 0 );			
			}

			// Create our additional render targets for high dynamic range.
			Texture2dConfigDX11 float_desc;
			float_desc.SetWidth( m_width );
			float_desc.SetHeight( m_height );
			float_desc.SetFormat( DXGI_FORMAT::DXGI_FORMAT_R16G16B16A16_FLOAT );
			float_desc.SetBindFlags( D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET );
			m_FloatTarget = RendererDX11::Get()->CreateTexture2D( &float_desc, nullptr );

			float_desc.SetBindFlags( D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS );
			m_BlurTargetX = RendererDX11::Get()->CreateTexture2D( &float_desc, nullptr );
			m_BlurTargetY = RendererDX11::Get()->CreateTexture2D( &float_desc, nullptr );

			// Create a view port to use on the scene.  This basically selects the 
			// entire floating point area of the render target.
			D3D11_VIEWPORT viewport;
			viewport.Width = static_cast< float >( m_width );
			viewport.Height = static_cast< float >( m_height );
			viewport.MinDepth = 0.0f;
			viewport.MaxDepth = 1.0f;
			viewport.TopLeftX = 0;
			viewport.TopLeftY = 0;

			SetViewPort( RendererDX11::Get()->CreateViewPort( viewport ) );
		}
	}
}
//--------------------------------------------------------------------------------
void ViewHighDynamicRange::Update( float fTime )
{
}
//--------------------------------------------------------------------------------
void ViewHighDynamicRange::QueuePreTasks( RendererDX11* pRenderer )
{
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
		m_pScene->PreRender( pRenderer, VT_PERSPECTIVE );
	}
}
//--------------------------------------------------------------------------------
void ViewHighDynamicRange::ExecuteTask( PipelineManagerDX11* pPipelineManager, IParameterManager* pParamManager )
{
	if ( m_pScene )
	{
		// Acquire the list of entities to be included in this rendering pass.

		std::vector<Entity3D*> entity_list;
		GetAllEntities( m_pScene->GetRoot(), entity_list );

		auto const transparent_check = []( Entity3D* entity) { 
			return entity->Visual.iPass != Renderable::ALPHA;
		};

		// We use stable partition to sort, and return the first transparent entity.
		auto const iter_first_alpha = std::stable_partition( begin( entity_list ), end( entity_list ), transparent_check );

		// Render the scene into the floating point buffer.  This captures all
		// of the light in the floating point format.
		// Set the parameters for rendering this view
		pPipelineManager->ClearRenderTargets();
		pPipelineManager->OutputMergerStage.DesiredState.RenderTargetViews.SetState( 0, m_FloatTarget->m_iResourceRTV );
		pPipelineManager->OutputMergerStage.DesiredState.DepthTargetViews.SetState( m_DepthTarget->m_iResourceDSV );
		pPipelineManager->ApplyRenderTargets();

		// Clear the color and depth targets if requested.
		if ( m_bEnableColorClear ) { pPipelineManager->ClearColorBuffers( m_BufferClearColor ); }
		if ( m_bEnableDepthClear ) { pPipelineManager->ClearDepthStencilBuffers( m_fDepthClearValue ); }

		// Configure the desired viewports in this pipeline
		ConfigureViewports( pPipelineManager );

		// Set this view's render parameters
		SetRenderParams( pParamManager );

		// Set the light parameters (currently only supporting the first light...)
		if ( m_pScene->GetLightCount() > 0 ) {
			m_pScene->GetLight( 0 )->Parameters.SetRenderParams( pParamManager );
		}

		pPipelineManager->ClearPipelineResources();


		// Now we can render all entities in the sorted order.
		for ( auto& entity : entity_list ) {
			entity->Render( pPipelineManager, pParamManager, VT_PERSPECTIVE );
		}

		pPipelineManager->ClearRenderTargets();
		pPipelineManager->ApplyRenderTargets();

		// Fill the first buffer with any data that exceeds a luminance threshold.
		processor.LuminanceThreshold( pPipelineManager, pParamManager, m_width, m_height, m_FloatTarget, m_BlurTargetX, m_threshold );


		// Blur the image in the X direction.
		processor.GaussianBlur( pPipelineManager, pParamManager, m_width, m_height, m_BlurTargetX, m_BlurTargetY );

		// Blur the image in the Y direction.

		// TODO: Need to update the blurring and tone mapping here!



		// Composite the blurred image together with the original image, and
		// apply the tone mapping operation.
		pParamManager->SetShaderResourceParameter( L"ColorTexture1", m_FloatTarget );
		pParamManager->SetShaderResourceParameter( L"ColorTexture2", m_BlurTargetY );

		pPipelineManager->ClearRenderTargets();
		pPipelineManager->OutputMergerStage.DesiredState.RenderTargetViews.SetState( 0, m_RenderTarget->m_iResourceRTV );
		pPipelineManager->OutputMergerStage.DesiredState.DepthTargetViews.SetState( m_DepthTarget->m_iResourceDSV );
		pPipelineManager->ApplyRenderTargets();
		
		// Clear the color and depth targets if requested.
		if ( m_bEnableColorClear ) { pPipelineManager->ClearColorBuffers( m_BufferClearColor ); }
		if ( m_bEnableDepthClear ) { pPipelineManager->ClearDepthStencilBuffers( m_fDepthClearValue ); }

		fullscreen.GetBody()->Render( pPipelineManager, pParamManager, VT_PERSPECTIVE );
	}
}
//--------------------------------------------------------------------------------
void ViewHighDynamicRange::Resize( UINT width, UINT height )
{
	m_width = width;
	m_height = height;

	RendererDX11::Get()->ResizeTexture( m_DepthTarget, width, height );
	RendererDX11::Get()->ResizeViewport( m_iViewports[0], width, height );

	RendererDX11::Get()->ResizeTexture( m_FloatTarget, width, height );
	RendererDX11::Get()->ResizeTexture( m_BlurTargetX, width, height );
	RendererDX11::Get()->ResizeTexture( m_BlurTargetY, width, height );
}
//--------------------------------------------------------------------------------
std::wstring ViewHighDynamicRange::GetName()
{
	return( L"ViewHighDynamicRange" );
}
//--------------------------------------------------------------------------------