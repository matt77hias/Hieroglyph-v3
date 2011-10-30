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
#include "PCH.h"
#include "ViewOcclusion.h"
#include "Entity3D.h"
#include "Node3D.h"
#include "Texture2dConfigDX11.h"
#include "Log.h"
#include "ActorGenerator.h"
#include "IParameterManager.h"
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
ViewOcclusion::ViewOcclusion( RendererDX11& Renderer, ResourcePtr OcclusionTarget, ResourcePtr BlurTarget, ResourcePtr DepthNormalTarget )
{
	D3D11_TEXTURE2D_DESC desc = OcclusionTarget->m_pTexture2dConfig->GetTextureDesc();

	ResolutionX = desc.Width;
	ResolutionY = desc.Height;

	DepthNormalBuffer = DepthNormalTarget;
	OcclusionBuffer = OcclusionTarget;
	BilateralBuffer = BlurTarget;

	// Create the effects that will be used to calculate the occlusion buffer and
	// the bilateral filters.

	pOcclusionEffect = new RenderEffectDX11();
	pOcclusionEffect->m_iComputeShader = 
		Renderer.LoadShader( COMPUTE_SHADER,
		std::wstring( L"AmbientOcclusionCS_32.hlsl" ),
		std::wstring( L"CSMAIN" ),
		std::wstring( L"cs_5_0" ) );

	pBilateralXEffect = new RenderEffectDX11();
	pBilateralXEffect->m_iComputeShader = 
		Renderer.LoadShader( COMPUTE_SHADER,
		std::wstring( L"SeparableBilateralCS.hlsl" ),
		std::wstring( L"CS_Horizontal" ),
		std::wstring( L"cs_5_0" ) );

	pBilateralYEffect = new RenderEffectDX11();
	pBilateralYEffect->m_iComputeShader = 
		Renderer.LoadShader( COMPUTE_SHADER,
		std::wstring( L"SeparableBilateralCS.hlsl" ),
		std::wstring( L"CS_Vertical" ),
		std::wstring( L"cs_5_0" ) );

	m_pDepthNormalBuffer = Renderer.m_pParamMgr->GetShaderResourceParameterRef( std::wstring( L"DepthNormalBuffer" ) );
	m_pAmbientOcclusionBuffer = Renderer.m_pParamMgr->GetShaderResourceParameterRef( std::wstring( L"AmbientOcclusionBuffer" ) );
	m_pAmbientOcclusionTarget = Renderer.m_pParamMgr->GetUnorderedAccessParameterRef( std::wstring( L"AmbientOcclusionTarget" ) );

}
//--------------------------------------------------------------------------------
ViewOcclusion::~ViewOcclusion()
{
	SAFE_DELETE( pOcclusionEffect );
	SAFE_DELETE( pBilateralXEffect );
	SAFE_DELETE( pBilateralYEffect );
}
//--------------------------------------------------------------------------------
void ViewOcclusion::Update( float fTime )
{
}
//--------------------------------------------------------------------------------
void ViewOcclusion::PreDraw( RendererDX11* pRenderer )
{
	// Queue this view into the renderer for processing.
	pRenderer->QueueRenderView( this );
}
//--------------------------------------------------------------------------------
void ViewOcclusion::Draw( PipelineManagerDX11* pPipelineManager, IParameterManager* pParamManager )
{
	// Process the occlusion buffer next.  Start by setting the needed resource
	// parameters for the depth/normal buffer and the occlusion buffer.

	pPipelineManager->ClearRenderTargets();
	pPipelineManager->ApplyRenderTargets();

	SetRenderParams( pParamManager );

	// Execute the compute shader to calculate the raw occlusion buffer.
	pPipelineManager->Dispatch( *pOcclusionEffect, ResolutionX / 32, ResolutionY / 32, 1, pParamManager );

	// Perform the blurring operations next.
	pPipelineManager->Dispatch( *pBilateralXEffect, 1, ResolutionY, 1, pParamManager );
	pPipelineManager->Dispatch( *pBilateralYEffect, ResolutionX, 1, 1, pParamManager );
}
//--------------------------------------------------------------------------------
void ViewOcclusion::SetRenderParams( IParameterManager* pParamManager )
{
	// Set the parameters for this view to be able to perform its processing
	// sequence.  In this case, we set the depth/normal buffer as a shader 
	// resource and the occlusion buffer as an unordered access view.

	pParamManager->SetShaderResourceParameter( (RenderParameterDX11*)m_pDepthNormalBuffer, DepthNormalBuffer );
	pParamManager->SetUnorderedAccessParameter( (RenderParameterDX11*)m_pAmbientOcclusionTarget, OcclusionBuffer );
}
//--------------------------------------------------------------------------------
void ViewOcclusion::SetUsageParams( IParameterManager* pParamManager )
{
	// Set the parameters for allowing an application to use the current resources
	// for rendering.

	pParamManager->SetShaderResourceParameter( (RenderParameterDX11*)m_pAmbientOcclusionBuffer, OcclusionBuffer );
}
//--------------------------------------------------------------------------------
