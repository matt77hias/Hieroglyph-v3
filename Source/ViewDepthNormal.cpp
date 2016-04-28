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
#include "ViewDepthNormal.h"
#include "Entity3D.h"
#include "Scene.h"
#include "Texture2dConfigDX11.h"
#include "Log.h"
#include "IParameterManager.h"
#include "PipelineManagerDX11.h"
#include "Texture2dDX11.h"
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
ViewDepthNormal::ViewDepthNormal( RendererDX11& Renderer, ResourcePtr RenderTarget, ResourcePtr DepthTarget )
	: ViewPerspective( Renderer, RenderTarget, DepthTarget )
{
	// This view is more or less the same as a ViewPerspective, with only a 
	// different scene rendering type (as indicated below with VT_LINEAR_DEPTH_NORMAL).  
	// In addition, the render target that gets passed to this view will serve as
	// the depth/normal buffer as opposed to a standard render target.

	m_pDepthNormalBuffer = Renderer.m_pParamMgr->GetShaderResourceParameterRef( std::wstring( L"DepthNormalBuffer" ) );
}
//--------------------------------------------------------------------------------
ViewDepthNormal::~ViewDepthNormal()
{
}
//--------------------------------------------------------------------------------
void ViewDepthNormal::QueuePreTasks( RendererDX11* pRenderer )
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
		m_pScene->PreRender( pRenderer, VT_LINEAR_DEPTH_NORMAL );
	}
}
//--------------------------------------------------------------------------------
void ViewDepthNormal::ExecuteTask( PipelineManagerDX11* pPipelineManager, IParameterManager* pParamManager )
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

		// Run through the graph and render each of the entities
		m_pScene->GetRoot()->Render( pPipelineManager, pParamManager, VT_LINEAR_DEPTH_NORMAL );
	}
}
//--------------------------------------------------------------------------------
void ViewDepthNormal::SetUsageParams( IParameterManager* pParamManager )
{
	// This view will bind the depth/normal buffer to the "DepthNormalBuffer" shader
	// resource view parameter, so that other views can make use of it.

	pParamManager->SetShaderResourceParameter( m_pDepthNormalBuffer, m_RenderTarget );
}
//--------------------------------------------------------------------------------
std::wstring ViewDepthNormal::GetName()
{
	return( L"ViewDepthNormal" );
}
//--------------------------------------------------------------------------------
