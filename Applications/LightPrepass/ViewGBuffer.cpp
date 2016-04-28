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
#include "DepthStencilStateConfigDX11.h"
#include "GeometryGeneratorDX11.h"
#include "RasterizerStateConfigDX11.h"
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
ViewGBuffer::ViewGBuffer( RendererDX11& Renderer )
{
	ViewMatrix.MakeIdentity();
	ProjMatrix.MakeIdentity();

    // Make a depth-stencil state used for generating the MSAA mask.
    DepthStencilStateConfigDX11 dsConfig;
    dsConfig.DepthEnable = FALSE;
    dsConfig.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
    dsConfig.DepthFunc = D3D11_COMPARISON_ALWAYS;
    dsConfig.StencilEnable = true;
    dsConfig.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
    dsConfig.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;
    dsConfig.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
    dsConfig.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    dsConfig.FrontFace.StencilPassOp = D3D11_STENCIL_OP_INCR;
    dsConfig.FrontFace.StencilFunc = D3D11_COMPARISON_EQUAL;
    dsConfig.BackFace = dsConfig.FrontFace;

    m_iMaskDSState = Renderer.CreateDepthStencilState( &dsConfig );

    if ( m_iMaskDSState == -1 )
        Log::Get().Write( L"Failed to create mask depth stencil state" );

    // Create a rasterizer state with multisampling enabled
    RasterizerStateConfigDX11 rsConfig;
    rsConfig.MultisampleEnable = TRUE;
    rsConfig.CullMode = D3D11_CULL_BACK;
    m_iMaskRSState = Renderer.CreateRasterizerState( &rsConfig );

    m_MaskEffect.m_iDepthStencilState = m_iMaskDSState;
    m_MaskEffect.m_uStencilRef = 1;
    m_MaskEffect.m_iRasterizerState = m_iMaskRSState;

    // Generate geometry for a full screen quad
	m_QuadGeometry = GeometryPtr( new GeometryDX11() );
    GeometryGeneratorDX11::GenerateFullScreenQuad( m_QuadGeometry );
    m_QuadGeometry->LoadToBuffers();

    // Load shaders for generating the mask
    m_MaskEffect.SetVertexShader( Renderer.LoadShader( VERTEX_SHADER,
        std::wstring( L"MaskLP.hlsl" ),
        std::wstring( L"VSMain" ),
        std::wstring( L"vs_5_0" ) ) );
    _ASSERT( m_MaskEffect.GetVertexShader() != -1 );

    m_MaskEffect.SetPixelShader( Renderer.LoadShader( PIXEL_SHADER,
        std::wstring( L"MaskLP.hlsl" ),
        std::wstring( L"PSMain" ),
        std::wstring( L"ps_5_0" ) ) );
    _ASSERT( m_MaskEffect.GetPixelShader() != -1 );

	m_pGBufferTexture = Renderer.m_pParamMgr->GetShaderResourceParameterRef( std::wstring( L"GBufferTexture" ) );

	m_QuadGeometry->GenerateInputLayout( m_MaskEffect.GetVertexShader() );
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
		pPipelineManager->OutputMergerStage.DesiredState.RenderTargetViews.SetState( 0, m_GBufferTarget->m_iResourceRTV );
		pPipelineManager->OutputMergerStage.DesiredState.DepthTargetViews.SetState( m_DepthTarget->m_iResourceDSV );
		pPipelineManager->ApplyRenderTargets();

		// Configure the desired viewports in this pipeline
		ConfigureViewports( pPipelineManager );

		// Clear the G-Buffer targets
		Vector4f color(0.0f, 0.0f, 0.0f, 0.0f);
		pPipelineManager->ClearBuffers( color, 1.0f, 0 );

		// Set this view's render parameters
		SetRenderParams( pParamManager );

		// Run through the graph and render each of the entities
		m_pScene->GetRoot()->Render( pPipelineManager, pParamManager, VT_GBUFFER );

        // Now that we've filled the G-Buffer, we'll generate a stencil mask
        // that masks out all pixels where the individual sub-samples aren't
        // identical due to a triangle edge lying on that pixel. At this point the
        // stencil buffer already has a value of 1 for all pixels where geometry was
        // rendered, and this pass will increment that value for all edge pixels
        // leaving a value of 2.

        pPipelineManager->ClearRenderTargets();
		pPipelineManager->OutputMergerStage.DesiredState.DepthTargetViews.SetState( m_DepthTarget->m_iResourceDSV );
        pPipelineManager->ApplyRenderTargets();

        pParamManager->SetShaderResourceParameter( m_pGBufferTexture, m_GBufferTarget );


		// Use the effect to load all of the pipeline stages here.
		pPipelineManager->ClearPipelineResources();
		m_MaskEffect.ConfigurePipeline( pPipelineManager, pParamManager );
		pPipelineManager->ApplyPipelineResources();

        m_QuadGeometry->Execute( pPipelineManager, pParamManager );
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
void ViewGBuffer::SetTargets( ResourcePtr GBufferTarget, ResourcePtr DepthTarget, int Viewport )
{
    m_GBufferTarget = GBufferTarget;
    SetViewPort( Viewport );
    m_DepthTarget = DepthTarget;
}
//--------------------------------------------------------------------------------
std::wstring ViewGBuffer::GetName()
{
	return( L"ViewGBuffer" );
}
//--------------------------------------------------------------------------------