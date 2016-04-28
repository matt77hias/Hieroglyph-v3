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
#include "ViewGBuffer.h"
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
ViewGBuffer::ViewGBuffer( RendererDX11& Renderer )
    : m_Renderer( Renderer )
{
	ViewMatrix.MakeIdentity();
	ProjMatrix.MakeIdentity();
}
//--------------------------------------------------------------------------------
ViewGBuffer::~ViewGBuffer()
{
}
//--------------------------------------------------------------------------------
void ViewGBuffer::Update( float fTime )
{
}
//--------------------------------------------------------------------------------
void ViewGBuffer::QueuePreTasks( RendererDX11* pRenderer )
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
		m_pScene->GetRoot()->PreRender( pRenderer, VT_GBUFFER );
	}
}
//--------------------------------------------------------------------------------
void ViewGBuffer::ExecuteTask( PipelineManagerDX11* pPipelineManager, IParameterManager* pParamManager )
{
	if ( m_pScene )
	{
		// Set the parameters for rendering this view
		pPipelineManager->ClearRenderTargets();
		for ( unsigned int i = 0; i < m_GBufferTargets.size(); ++i )
			pPipelineManager->OutputMergerStage.DesiredState.RenderTargetViews.SetState( i, m_GBufferTargets[i]->m_iResourceRTV );
		pPipelineManager->OutputMergerStage.DesiredState.DepthTargetViews.SetState( m_DepthTarget->m_iResourceDSV );
		pPipelineManager->ApplyRenderTargets();

		// Configure the desired viewports in this pipeline
		ConfigureViewports( pPipelineManager );

		// Clear the G-Buffer targets
		Vector4f color( 0.0f, 0.0f, 0.0f, 0.0f );
		pPipelineManager->ClearBuffers( color, 1.0f, 0 );

		// Set this view's render parameters
		SetRenderParams( pParamManager );

		// Run through the graph and render each of the entities
		m_pScene->GetRoot()->Render( pPipelineManager, pParamManager, VT_GBUFFER );
	}
}
//--------------------------------------------------------------------------------
void ViewGBuffer::Resize( UINT width, UINT height )
{
	// Nothing needed here, since the main render view handles the resizing of
	// the resources and the viewports.
}
//--------------------------------------------------------------------------------
void ViewGBuffer::SetRenderParams( IParameterManager* pParamManager )
{
	pParamManager->SetViewMatrixParameter( &ViewMatrix );
	pParamManager->SetProjMatrixParameter( &ProjMatrix );
}
//--------------------------------------------------------------------------------
void ViewGBuffer::SetUsageParams( IParameterManager* pParamManager )
{

}
//--------------------------------------------------------------------------------
void ViewGBuffer::SetTargets( std::vector<ResourcePtr>& GBufferTargets,
                                ResourcePtr DepthTarget, int Viewport )
{
    m_GBufferTargets = GBufferTargets;

    SetViewPort( Viewport );
    m_DepthTarget = DepthTarget;
}
//--------------------------------------------------------------------------------
std::wstring ViewGBuffer::GetName()
{
	return( L"ViewGBuffer" );
}
//--------------------------------------------------------------------------------