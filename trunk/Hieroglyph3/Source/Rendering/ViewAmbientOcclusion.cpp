//--------------------------------------------------------------------------------
// This file is a portion of the Hieroglyph 3 Rendering Engine.  It is distributed
// under the MIT License, available in the root of this distribution and 
// at the following URL:
//
// http://www.opensource.org/licenses/mit-license.php
//
// Copyright (c) 2003-2010 Jason Zink 
//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
#include "ViewAmbientOcclusion.h"
#include "Entity3D.h"
#include "Node3D.h"
#include "Texture2dConfigDX11.h"
#include "Log.h"
#include <sstream>
#include "ActorGenerator.h"
#include "ParameterManagerDX11.h"
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
ViewAmbientOcclusion::ViewAmbientOcclusion( RendererDX11& Renderer, ResourcePtr RenderTarget, ResourcePtr DepthTarget )
: ViewPerspective( Renderer, RenderTarget, DepthTarget )
{
	D3D11_TEXTURE2D_DESC desc = RenderTarget->m_pTexture2dConfig->GetTextureDesc();

	ResolutionX = desc.Width;
	ResolutionY = desc.Height;

	// Create the resources to be used in this rendering algorithm.  The 
	// depth/normal buffer will have four components, and be used as a render
	// target and shader resource.

	Texture2dConfigDX11 config;
	config.SetColorBuffer( ResolutionX, ResolutionY );
	config.SetBindFlags( D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET );
	DepthNormalBuffer = Renderer.CreateTexture2D( &config, 0 );
	
	// The occlusion buffer is calculated in the compute shader, so requires
	// an unordered access view as well as a shader resource.

	config.SetFormat( DXGI_FORMAT_R32_FLOAT );
	config.SetBindFlags( D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS );
	OcclusionBuffer = Renderer.CreateTexture2D( &config, 0 );
	
	// The blurring buffer will be used to blur the input buffer, and uses the same
	// configuration as the occlusion buffer.

	BilateralBuffer = Renderer.CreateTexture2D( &config, 0 );

	//pPerspectiveView = new ViewPerspective( Renderer, RenderTarget, DepthTarget );
	pOcclusionView = new ViewOcclusion( Renderer, OcclusionBuffer, BilateralBuffer, DepthNormalBuffer );
	pDepthNormalView = new ViewDepthNormal( Renderer, DepthNormalBuffer, DepthTarget );


	// Create the effects that will be used to calculate the occlusion buffer and
	// the bilateral filters.

	//pOcclusionEffect = new RenderEffectDX11();
	//pOcclusionEffect->m_iComputeShader = 
	//	Renderer.LoadShader( COMPUTE_SHADER,
	//	std::wstring( L"../Data/Shaders/AmbientOcclusionCS_32.hlsl" ),
	//	std::wstring( L"CSMAIN" ),
	//	std::wstring( L"cs_5_0" ) );

	//pBilateralXEffect = new RenderEffectDX11();
	//pBilateralXEffect->m_iComputeShader = 
	//	Renderer.LoadShader( COMPUTE_SHADER,
	//	std::wstring( L"../Data/Shaders/SeparableBilateralCS.hlsl" ),
	//	std::wstring( L"CS_Horizontal" ),
	//	std::wstring( L"cs_5_0" ) );

	//pBilateralYEffect = new RenderEffectDX11();
	//pBilateralYEffect->m_iComputeShader = 
	//	Renderer.LoadShader( COMPUTE_SHADER,
	//	std::wstring( L"../Data/Shaders/SeparableBilateralCS.hlsl" ),
	//	std::wstring( L"CS_Vertical" ),
	//	std::wstring( L"cs_5_0" ) );

	// Create the visualization actor and send in the occlusion buffer

//	pVisActor = ActorGenerator::GenerateVisualizationTexture2D( Renderer, 
//			DepthNormalBuffer, 0 );
}
//--------------------------------------------------------------------------------
ViewAmbientOcclusion::~ViewAmbientOcclusion()
{
	//SAFE_DELETE( pPerspectiveView );
	SAFE_DELETE( pOcclusionView );
	SAFE_DELETE( pDepthNormalView );

	//SAFE_DELETE( pOcclusionEffect );
	//SAFE_DELETE( pBilateralXEffect );
	//SAFE_DELETE( pBilateralYEffect );

	//SAFE_DELETE( pVisActor );
}
//--------------------------------------------------------------------------------
void ViewAmbientOcclusion::Update( float fTime )
{
}
//--------------------------------------------------------------------------------
void ViewAmbientOcclusion::PreDraw( RendererDX11* pRenderer )
{
	ViewPerspective::PreDraw( pRenderer );

	//pPerspectiveView->PreDraw( pRenderer );
	pOcclusionView->PreDraw( pRenderer );
	pDepthNormalView->PreDraw( pRenderer );

//	if ( m_pRoot )
//	{
		// Queue this view into the renderer for processing.
//		pRenderer->QueueRenderView( this );

		// Run through the graph and pre-render the entities
//		m_pRoot->PreRender( pRenderer, VT_LINEAR_DEPTH_NORMAL );
//	}
}
//--------------------------------------------------------------------------------
void ViewAmbientOcclusion::Draw( PipelineManagerDX11* pPipelineManager, ParameterManagerDX11* pParamManager )
{
	ViewPerspective::Draw( pPipelineManager, pParamManager );

	// Set the view matrix if this render view is associated with an entity.
	//if ( m_pEntity != NULL )
	//	ViewMatrix = m_pEntity->GetView();

	// Render the depth/normal buffer, keeping the depth target for a later pass.
	//if ( m_pRoot )
	//{
	//	// Run through the graph and pre-render the entities
	//	//m_pRoot->PreRender( pPipelineManager, pParamManager, VT_LINEAR_DEPTH_NORMALS );

	//	// Set the parameters for rendering this view
	//	pPipelineManager->ClearRenderTargets();
	//	pPipelineManager->BindRenderTargets( 0, DepthNormalBuffer );
	//	pPipelineManager->BindDepthTarget( m_DepthTarget );
	//	pPipelineManager->ApplyRenderTargets();
	//	pPipelineManager->ClearBuffers( m_vColor, 1.0f );

	//	// Set the perspective view's render parameters, since the depth normal
	//	// buffer is a perspective rendering.
	//	ViewPerspective::SetRenderParams( pParamManager );

	//	// Run through the graph and render each of the entities
	//	m_pRoot->Render( pPipelineManager, pParamManager, VT_LINEAR_DEPTH_NORMAL );
	//}
	//
	//// TODO: I added this bind statement here to force the DepthNormalBuffer to not
	////       be bound to the pipeline anymore.  This should be done automatically, or
	////       with an explicit method to clear render targets or something similar.
	////       One automatic way would be to check the resource being set for the shader
	////       resource parameter, to see if it is a render target view as well.  If so,
	////       then clear the render targets when the SRV is set.

	//pPipelineManager->ClearRenderTargets();
	//pPipelineManager->ApplyPipelineResources();

	//// Process the occlusion buffer next.  Start by setting the needed resource
	//// parameters for the depth/normal buffer and the occlusion buffer.
	//ViewAmbientOcclusion::SetRenderParams( pParamManager );

	//// Execute the compute shader to calculate the raw occlusion buffer.
	//pPipelineManager->Dispatch( *pOcclusionEffect, ResolutionX / 32, ResolutionY / 32, 1, pParamManager );

	//// Perform the blurring operations next.
	//pPipelineManager->Dispatch( *pBilateralXEffect, 1, ResolutionY, 1, pParamManager );
	//pPipelineManager->Dispatch( *pBilateralYEffect, ResolutionX, 1, 1, pParamManager );
	////pPipelineManager( *pBilateralXEffect, 1, ResolutionY, 1 );
	////pPipelineManager( *pBilateralYEffect, ResolutionX, 1, 1 );

	//// Perform the final rendering pass now.  This will use the ViewAmbientOcclusion
	//// output parameters (i.e. a shader resource view with occlusion buffer in it), and 
	//// render with the perspective view's draw method.
	//ViewAmbientOcclusion::SetUsageParams( pParamManager );
	//ViewPerspective::SetRenderParams( pParamManager );

	//ViewPerspective::Draw( pPipelineManager, pParamManager );

	//// Add the visualization rendering into the scene
	//pVisActor->GetNode()->Render( pPipelineManager, pParamManager, VT_PERSPECTIVE );
	
}
//--------------------------------------------------------------------------------
void ViewAmbientOcclusion::SetRenderParams( ParameterManagerDX11* pParamManager )
{
	// Set the parameters for this view to be able to perform its processing
	// sequence.  In this case, we set the depth/normal buffer as a shader 
	// resource and the occlusion buffer as an unordered access view.

	ViewPerspective::SetRenderParams( pParamManager );

	pParamManager->SetShaderResourceParameter( L"DepthNormalBuffer", DepthNormalBuffer );
	//pParamManager->SetUnorderedAccessParameter( L"AmbientOcclusionTarget", OcclusionBuffer );
	pParamManager->SetShaderResourceParameter( L"AmbientOcclusionBuffer", OcclusionBuffer );
}
//--------------------------------------------------------------------------------
void ViewAmbientOcclusion::SetUsageParams( ParameterManagerDX11* pParamManager )
{
	// Set the parameters for allowing an application to use the current resources
	// for rendering.

	pParamManager->SetShaderResourceParameter( L"AmbientOcclusionBuffer", OcclusionBuffer );
}
//--------------------------------------------------------------------------------
void ViewAmbientOcclusion::SetViewMatrix( Matrix4f& matrix )
{
	IRenderView::SetViewMatrix( matrix );

	//pPerspectiveView->SetViewMatrix( matrix );
	pDepthNormalView->SetViewMatrix( matrix );
}
//--------------------------------------------------------------------------------
void ViewAmbientOcclusion::SetProjMatrix( Matrix4f& matrix )
{
	IRenderView::SetProjMatrix( matrix );

	//pPerspectiveView->SetProjMatrix( matrix );
	pDepthNormalView->SetProjMatrix( matrix );
}
//--------------------------------------------------------------------------------
void ViewAmbientOcclusion::SetRoot( Node3D* pRoot )
{
	m_pRoot = pRoot;

	//pPerspectiveView->SetRoot( pRoot );
	pDepthNormalView->SetRoot( pRoot );
}
//--------------------------------------------------------------------------------
void ViewAmbientOcclusion::SetEntity( Entity3D* pEntity )
{
	m_pEntity = pEntity;

	//pPerspectiveView->SetEntity( pEntity );
	pDepthNormalView->SetEntity( pEntity );
}
//--------------------------------------------------------------------------------
void ViewAmbientOcclusion::SetBackColor( Vector4f color )
{
	m_vColor = color;

	//pPerspectiveView->SetBackColor( color );
	//pDepthNormalView->SetBackColor( color );
}
//--------------------------------------------------------------------------------
