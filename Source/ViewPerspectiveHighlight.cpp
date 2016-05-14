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
#include "ViewPerspectiveHighlight.h"
#include "Entity3D.h"
#include "Scene.h"
#include "Texture2dConfigDX11.h"
#include "Log.h"
#include "IParameterManager.h"
#include "PipelineManagerDX11.h"
#include "Texture2dDX11.h"
#include "BoundsVisualizerActor.h"
#include "SceneGraph.h"
#include "BlendStateConfigDX11.h"
#include "DepthStencilStateConfigDX11.h"
#include <algorithm>
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
ViewPerspectiveHighlight::ViewPerspectiveHighlight( RendererDX11& Renderer, ResourcePtr RenderTarget, ResourcePtr DepthTarget )
{
	ViewMatrix.MakeIdentity();
	ProjMatrix.MakeIdentity();

	m_pEntity = 0;
	m_vColor.MakeZero();

	SetRenderTargets( RenderTarget, DepthTarget );

	// Configure a material for our fullscreen actor.
	MaterialPtr pMaterial = MaterialPtr( new MaterialDX11() );

	// Create and fill the effect that will be used for this view type
	RenderEffectDX11* pEffect = new RenderEffectDX11();

	pEffect->SetVertexShader( RendererDX11::Get()->LoadShader( VERTEX_SHADER,
		std::wstring( L"FullscreenSilhouette.hlsl" ),
		std::wstring( L"VSMAIN" ),
		std::wstring( L"vs_4_0" ) ) );

	pEffect->SetPixelShader( RendererDX11::Get()->LoadShader( PIXEL_SHADER,
		std::wstring( L"FullscreenSilhouette.hlsl" ),
		std::wstring( L"PSMAIN" ),
		std::wstring( L"ps_4_0" ) ) );

	DepthStencilStateConfigDX11 depth;
	depth.DepthEnable = false;
	pEffect->m_iDepthStencilState = RendererDX11::Get()->CreateDepthStencilState( &depth );

	// Enable the material to render the given view type, and set its effect.
	pMaterial->Params[VT_PERSPECTIVE].bRender = true;
	pMaterial->Params[VT_PERSPECTIVE].pEffect = pEffect;

	m_SilhouetteActor.GetBody()->Visual.SetMaterial( pMaterial );
	m_SilhouetteActor.GetBody()->Parameters.SetVectorParameter( L"ObjectColor", Vector4f( 1.0f, 1.0f, 0.0f, 1.0f ) );
}
//--------------------------------------------------------------------------------
ViewPerspectiveHighlight::~ViewPerspectiveHighlight()
{
}
//--------------------------------------------------------------------------------
void ViewPerspectiveHighlight::SetRenderTargets( ResourcePtr RenderTarget, ResourcePtr DepthTarget )
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

			// Next we create a depth buffer for use in the traditional rendering
			// pipeline.
			if ( DepthTarget != NULL ) {
				m_DepthTarget = DepthTarget;
			} else {
				Texture2dConfigDX11 DepthConfig;
				DepthConfig.SetDepthBuffer( desc.Width, desc.Height );
				m_DepthTarget = RendererDX11::Get()->CreateTexture2D( &DepthConfig, nullptr );			
			}

			// Create a secondary texture to use for the silhouette render target.
			Texture2dConfigDX11 SilhouetteConfig;
			SilhouetteConfig.SetColorBuffer( desc.Width, desc.Height );
			SilhouetteConfig.SetBindFlags( D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET );
			SilhouetteConfig.SetFormat( DXGI_FORMAT_R8G8B8A8_UNORM );

			m_SilhouetteTarget = RendererDX11::Get()->CreateTexture2D( &SilhouetteConfig, nullptr );
			m_SilhouetteActor.GetBody()->Parameters.SetShaderResourceParameter( L"SilhouetteTarget", m_SilhouetteTarget );
			m_SilhouetteActor.GetBody()->Parameters.SetVectorParameter( L"ScreenResolution", Vector4f( static_cast<float>( desc.Width ), static_cast<float>( desc.Height ), 1.0f / static_cast<float>( desc.Width ), 1.0f / static_cast<float>( desc.Height ) ) );

			// Create a view port to use on the scene.  This basically selects the 
			// entire floating point area of the render target.
			D3D11_VIEWPORT viewport;
			viewport.Width = static_cast< float >( desc.Width );
			viewport.Height = static_cast< float >( desc.Height );
			viewport.MinDepth = 0.0f;
			viewport.MaxDepth = 1.0f;
			viewport.TopLeftX = 0;
			viewport.TopLeftY = 0;

			SetViewPort( RendererDX11::Get()->CreateViewPort( viewport ) );
		}
	}
}
//--------------------------------------------------------------------------------
void ViewPerspectiveHighlight::Update( float fTime )
{
}
//--------------------------------------------------------------------------------
void ViewPerspectiveHighlight::QueuePreTasks( RendererDX11* pRenderer )
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
void ViewPerspectiveHighlight::ExecuteTask( PipelineManagerDX11* pPipelineManager, IParameterManager* pParamManager )
{
	if ( m_pScene )
	{
		// Set the parameters for rendering this view
		pPipelineManager->ClearRenderTargets();
		pPipelineManager->OutputMergerStage.DesiredState.RenderTargetViews.SetState( 0, m_SilhouetteTarget->m_iResourceRTV );
		pPipelineManager->OutputMergerStage.DesiredState.DepthTargetViews.SetState( m_DepthTarget->m_iResourceDSV );
		pPipelineManager->ApplyRenderTargets();

		pPipelineManager->ClearBuffers( Vector4f( 0.0f, 0.0f, 0.0f, 0.0f ), 1.0f );

		// Configure the desired viewports in this pipeline
		ConfigureViewports( pPipelineManager );

		// Set this view's render parameters
		SetRenderParams( pParamManager );

		// Set the light parameters (currently only supporting the first light...)
		//if ( m_pScene->GetLightCount() > 0 ) {
		//	m_pScene->GetLight( 0 )->Parameters.SetRenderParams( pParamManager );
		//}

		pPipelineManager->ClearPipelineResources();

		// Run through the graph and render each of the entities
		m_pScene->GetRoot()->Render( pPipelineManager, pParamManager, VT_SILHOUETTE );







		// Set the parameters for rendering this view
		pPipelineManager->ClearRenderTargets();
		pPipelineManager->OutputMergerStage.DesiredState.RenderTargetViews.SetState( 0, m_RenderTarget->m_iResourceRTV );
		pPipelineManager->OutputMergerStage.DesiredState.DepthTargetViews.SetState( m_DepthTarget->m_iResourceDSV );
		pPipelineManager->ApplyRenderTargets();

		pPipelineManager->ClearBuffers( m_vColor, 1.0f );

		// Configure the desired viewports in this pipeline
		ConfigureViewports( pPipelineManager );

		// Set this view's render parameters
		SetRenderParams( pParamManager );

		// Set the light parameters (currently only supporting the first light...)
		if ( m_pScene->GetLightCount() > 0 ) {
			m_pScene->GetLight( 0 )->Parameters.SetRenderParams( pParamManager );
		}

		pPipelineManager->ClearPipelineResources();

		// Run through the graph and render each of the entities.  This will sort the entities 
		// based on whether or not they are transparent.
		
		std::vector<Entity3D*> entity_list;
		GetAllEntities( m_pScene->GetRoot(), entity_list );

		auto const transparent_check = []( Entity3D* entity) { 
			return entity->Visual.iPass != Renderable::ALPHA;
		};

		// We use stable partition to sort, and return the first transparent entity.
		auto const iter_first_alpha = std::stable_partition(begin(entity_list), end(entity_list), transparent_check);

		// Now we can render all entities in the sorted order.
		for ( auto& entity : entity_list ) {
			entity->Render( pPipelineManager, pParamManager, VT_PERSPECTIVE );
		}


		// Set the silhouette buffer as a shader resource, then draw the full screen
		// geometry to generate the silhouettes.
		m_SilhouetteActor.GetBody()->Render( pPipelineManager, pParamManager, VT_PERSPECTIVE );



		// If the debug view is enabled, then we can render some additional scene
		// related information as an overlay on this view.  Note that this is
		// accomplished without the actor being attached to the scene!
		if ( m_bDebugViewEnabled )
		{
			std::vector<Entity3D*> list;
			GetAllEntities( m_pScene->GetRoot(), list );
			m_pDebugVisualizer->UpdateBoundsData( list );

			m_pDebugVisualizer->GetBody()->Render( pPipelineManager, pParamManager, VT_PERSPECTIVE );
		}
	}
}
//--------------------------------------------------------------------------------
void ViewPerspectiveHighlight::Resize( UINT width, UINT height )
{
	RendererDX11::Get()->ResizeTexture( m_DepthTarget, width, height );
	RendererDX11::Get()->ResizeTexture( m_SilhouetteTarget, width, height );
	RendererDX11::Get()->ResizeViewport( m_iViewports[0], width, height );

	m_SilhouetteActor.GetBody()->Parameters.SetVectorParameter( L"ScreenResolution", Vector4f( static_cast<float>( width ), static_cast<float>( height ), 1.0f / static_cast<float>( width ), 1.0f / static_cast<float>( height ) ) );
}
//--------------------------------------------------------------------------------
std::wstring ViewPerspectiveHighlight::GetName()
{
	return( L"ViewPerspectiveHighlight" );
}
//--------------------------------------------------------------------------------
