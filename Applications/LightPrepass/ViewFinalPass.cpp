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
#include "ViewFinalPass.h"
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
ViewFinalPass::ViewFinalPass( RendererDX11& Renderer )
{
    ViewMatrix.MakeIdentity();
    ProjMatrix.MakeIdentity();

	m_pLightTexture = Renderer.m_pParamMgr->GetShaderResourceParameterRef( std::wstring( L"LightTexture" ) );
}
//--------------------------------------------------------------------------------
ViewFinalPass::~ViewFinalPass()
{
}
//--------------------------------------------------------------------------------
void ViewFinalPass::Update( float fTime )
{
}
//--------------------------------------------------------------------------------
void ViewFinalPass::QueuePreTasks( RendererDX11* pRenderer )
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
        m_pScene->GetRoot()->PreRender( pRenderer, VT_FINALPASS );
    }
}
//--------------------------------------------------------------------------------
void ViewFinalPass::ExecuteTask( PipelineManagerDX11* pPipelineManager, IParameterManager* pParamManager )
{
    if ( m_pScene )
    {
	    // Set the render target for the final pass, and clear it
		pPipelineManager->ClearRenderTargets();
		pPipelineManager->OutputMergerStage.DesiredState.RenderTargetViews.SetState( 0, m_RenderTarget->m_iResourceRTV );
		pPipelineManager->ApplyRenderTargets();
		pPipelineManager->ClearBuffers( Vector4f( 0.0f, 0.0f, 0.0f, 0.0f ) );

		// Also bind the depth buffer
		pPipelineManager->OutputMergerStage.DesiredState.DepthTargetViews.SetState( m_DepthTarget->m_iResourceDSV );
		pPipelineManager->ApplyRenderTargets();

		// Configure the desired viewports in this pipeline
		ConfigureViewports( pPipelineManager );

        // Set this view's render parameters
        SetRenderParams( pParamManager );

        // Run through the graph and render each of the entities
        m_pScene->GetRoot()->Render( pPipelineManager, pParamManager, VT_FINALPASS );
    }
}
//--------------------------------------------------------------------------------
void ViewFinalPass::Resize( UINT width, UINT height )
{
	// Nothing needed here, since the main render view handles the resizing of
	// the resources and the viewports.
}
//--------------------------------------------------------------------------------
void ViewFinalPass::SetRenderParams( IParameterManager* pParamManager )
{
    pParamManager->SetViewMatrixParameter( &ViewMatrix );
    pParamManager->SetProjMatrixParameter( &ProjMatrix );

    // Set the light texture
    pParamManager->SetShaderResourceParameter( m_pLightTexture, m_LightTarget);
}
//--------------------------------------------------------------------------------
void ViewFinalPass::SetUsageParams( IParameterManager* pParamManager )
{

}
//--------------------------------------------------------------------------------
void ViewFinalPass::SetTargets( ResourcePtr LightTarget, ResourcePtr RenderTarget,
                                ResourcePtr DepthTarget, int Viewport )
{
    m_LightTarget = LightTarget;
    m_RenderTarget = RenderTarget;
    SetViewPort( Viewport );
    m_DepthTarget = DepthTarget;
}
//--------------------------------------------------------------------------------
std::wstring ViewFinalPass::GetName()
{
	return( L"ViewFinalPass" );
}
//--------------------------------------------------------------------------------