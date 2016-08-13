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
#include "GlyphRift/ViewRiftHighlight.h"
#include "Entity3D.h"
#include "Scene.h"
#include "Texture2dConfigDX11.h"
#include "Log.h"
#include "IParameterManager.h"
#include "PipelineManagerDX11.h"
#include "Texture2dDX11.h"
#include "BoundsVisualizerActor.h"
#include "SceneGraph.h"
#include "DepthStencilStateConfigDX11.h"
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
ViewRiftHighlight::ViewRiftHighlight( RiftHMDPtr hmd, ResourcePtr RenderTarget, int SwapChain  )
{
	m_pHmd = hmd;

	// Get the desired texture sizes for the eye render targets.  Note that these 
	// are typically larger than the resolution of the HMD's display panel itself.

	unsigned int width = m_pHmd->DesiredEyeTextureWidth();
	unsigned int height = m_pHmd->DesiredEyeTextureHeight();

	// Create a single depth buffer, which will be used in rendering the scene
	// into each eye.

	Texture2dConfigDX11 depthConfig;
	depthConfig.SetDepthBuffer( width, height );
	m_DepthTarget = RendererDX11::Get()->CreateTexture2D( &depthConfig, 0 );

	// Read back the actual size of the texture created for use in the other
	// setup functions below.

	D3D11_TEXTURE2D_DESC desc = RendererDX11::Get()->GetTexture2DByIndex( m_DepthTarget->m_iResource )->GetActualDescription();

	// Create a secondary texture to use for the silhouette render target.
	Texture2dConfigDX11 SilhouetteConfig;
	SilhouetteConfig.SetColorBuffer( desc.Width, desc.Height );
	SilhouetteConfig.SetBindFlags( D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET );
	SilhouetteConfig.SetFormat( DXGI_FORMAT_R8G8B8A8_UNORM );

	m_SilhouetteTarget = RendererDX11::Get()->CreateTexture2D( &SilhouetteConfig, nullptr );
	m_SilhouetteActor.GetBody()->Parameters.SetShaderResourceParameter( L"SilhouetteTarget", m_SilhouetteTarget );
	m_SilhouetteActor.GetBody()->Parameters.SetVectorParameter( L"ScreenResolution", Vector4f( static_cast<float>( desc.Width ), static_cast<float>( desc.Height ), 1.0f / static_cast<float>( desc.Width ), 1.0f / static_cast<float>( desc.Height ) ) );


	// Create a view port to use on the scene.  Since they are the exact same
	// for both eyes, we can just use one viewport.

	D3D11_VIEWPORT viewport;
	viewport.Width = static_cast< float >( desc.Width );
	viewport.Height = static_cast< float >( desc.Height );
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;

	m_iViewports[0] = RendererDX11::Get()->CreateViewPort( viewport );

	// Configure the rendering for the rift's eye textures.  This is not dependent
	// on the swap chain size, but we provide the option to limit the resolution
	// for the eye textures here.  Default values are 2048 x 2048, essentially
	// allowing whatever the rift requests.

	m_pHmd->ConfigureRendering( );


	// Set up the projection matrices according to the FOV for each eye.  These
	// can be calculated once and then reused as long as the FOV is not changed
	// during runtime.

	m_proj[0] = m_pHmd->GetPerspectiveFov( 0, 0.01f, 10000.0f );
	m_proj[1] = m_pHmd->GetPerspectiveFov( 1, 0.01f, 10000.0f );


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
ViewRiftHighlight::~ViewRiftHighlight()
{
}
//--------------------------------------------------------------------------------
void ViewRiftHighlight::Update( float fTime )
{
}
//--------------------------------------------------------------------------------
void ViewRiftHighlight::QueuePreTasks( RendererDX11* pRenderer )
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
void ViewRiftHighlight::ExecuteTask( PipelineManagerDX11* pPipelineManager, IParameterManager* pParamManager )
{
	// Here we assume that the ovrHmd_BeginFrame function has already been called
	// by the application at the beginning of the rendering phase.  The 
	// application will also call the ovrHmd_EndFrame function after all rendering
	// is completed.

	if ( m_pScene )
	{
		// Configure the desired viewports in this pipeline.  This state is the 
		// same for both eyes, so we pull it out of the loop.
		ConfigureViewports( pPipelineManager );

		for ( unsigned int eye = 0; eye < 2; ++eye )
		{
			// Set up the view matrices for this eye.  This starts by getting the 
			// view matrix from the node that this view's entity is attached to.  
			// That represents the camera's node (instead of its body).  We then apply
			// the eye's spatial state (including position and orientation) to that
			// parent location.
			//
			// NOTE: This view's entity location and orientation is updated according
			//       to the Rift's *head* location.  That provides an approximate location
			//       for using to visualize the location of the rift in world space.
			//       This is why we need to grab the parent's view - the eye matrices are
			//       in relation to the whole rift, as opposed to relative to the head.

			Matrix4f centerView = m_pEntity->GetParent()->Transform.GetView();	
			m_view[eye] = centerView * m_pHmd->GetEyeSpatialState( eye );

			// Set this view's render parameters
			SetViewMatrix( m_view[eye] );
			SetProjMatrix( m_proj[eye] );
			SetRenderParams( pParamManager );

			// This is inserted to avoid a pipeline hazard debug warning from the
			// silhouette render target.
			pPipelineManager->ClearPipelineResources();
			pPipelineManager->ApplyPipelineResources();

			// Set the parameters for rendering this view
			pPipelineManager->ClearRenderTargets();
			pPipelineManager->OutputMergerStage.DesiredState.RenderTargetViews.SetState( 0, m_SilhouetteTarget->m_iResourceRTV );
			pPipelineManager->OutputMergerStage.DesiredState.DepthTargetViews.SetState( m_DepthTarget->m_iResourceDSV );
			pPipelineManager->ApplyRenderTargets();
			pPipelineManager->ClearBuffers( Vector4f( 0.0f, 0.0f, 0.0f, 0.0f ), 1.0f );

			// Run through the graph and render each of the entities
			m_pScene->GetRoot()->Render( pPipelineManager, pParamManager, VT_SILHOUETTE );



			// Set the parameters for rendering this view
			pPipelineManager->ClearRenderTargets();
			pPipelineManager->OutputMergerStage.DesiredState.RenderTargetViews.SetState( 0, m_pHmd->GetEyeTexture(eye)->m_iResourceRTV );
			pPipelineManager->OutputMergerStage.DesiredState.DepthTargetViews.SetState( m_DepthTarget->m_iResourceDSV );
			pPipelineManager->ApplyRenderTargets();
			pPipelineManager->ClearBuffers( m_vColor, 1.0f );


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

			pPipelineManager->ClearPipelineResources();

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
}
//--------------------------------------------------------------------------------
void ViewRiftHighlight::Resize( UINT width, UINT height )
{
	// These sizes are determined by the Rift Headset, and should not change
	// with window size changes.

	//RendererDX11::Get()->ResizeTexture( m_DepthTarget, width, height );
	//RendererDX11::Get()->ResizeTexture( m_SilhouetteTarget, width, height );
	//RendererDX11::Get()->ResizeViewport( m_iViewports[0], width, height );

	//m_SilhouetteActor.GetBody()->Parameters.SetVectorParameter( L"ScreenResolution", Vector4f( static_cast<float>( width ), static_cast<float>( height ), 1.0f / static_cast<float>( width ), 1.0f / static_cast<float>( height ) ) );
}
//--------------------------------------------------------------------------------
std::wstring ViewRiftHighlight::GetName()
{
	return( L"ViewRiftHighlight" );
}
//--------------------------------------------------------------------------------