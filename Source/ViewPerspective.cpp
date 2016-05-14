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
#include "ViewPerspective.h"
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
ViewPerspective::ViewPerspective( RendererDX11& Renderer, ResourcePtr RenderTarget, ResourcePtr DepthTarget )
{
	ViewMatrix.MakeIdentity();
	ProjMatrix.MakeIdentity();

	m_pEntity = 0;
	m_vColor.MakeZero();

	SetRenderTargets( RenderTarget, DepthTarget );
}
//--------------------------------------------------------------------------------
ViewPerspective::~ViewPerspective()
{
}
//--------------------------------------------------------------------------------
void ViewPerspective::SetRenderTargets( ResourcePtr RenderTarget, ResourcePtr DepthTarget )
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
				m_DepthTarget = RendererDX11::Get()->CreateTexture2D( &DepthConfig, 0 );			
			}

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
void ViewPerspective::Update( float fTime )
{
}
//--------------------------------------------------------------------------------
void ViewPerspective::QueuePreTasks( RendererDX11* pRenderer )
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
void ViewPerspective::ExecuteTask( PipelineManagerDX11* pPipelineManager, IParameterManager* pParamManager )
{
	if ( m_pScene )
	{
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

		// Run through the graph and render each of the entities
		//m_pScene->GetRoot()->Render( pPipelineManager, pParamManager, VT_PERSPECTIVE );

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
void ViewPerspective::Resize( UINT width, UINT height )
{
	RendererDX11::Get()->ResizeTexture( m_DepthTarget, width, height );
	RendererDX11::Get()->ResizeViewport( m_iViewports[0], width, height );
}
//--------------------------------------------------------------------------------
std::wstring ViewPerspective::GetName()
{
	return( L"ViewPerspective" );
}
//--------------------------------------------------------------------------------