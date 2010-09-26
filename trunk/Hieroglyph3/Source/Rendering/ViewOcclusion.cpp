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
#include "ParameterManagerDX11.h"
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
		std::wstring( L"../Data/Shaders/AmbientOcclusionCS_32.hlsl" ),
		std::wstring( L"CSMAIN" ),
		std::wstring( L"cs_5_0" ) );

	pBilateralXEffect = new RenderEffectDX11();
	pBilateralXEffect->m_iComputeShader = 
		Renderer.LoadShader( COMPUTE_SHADER,
		std::wstring( L"../Data/Shaders/SeparableBilateralCS.hlsl" ),
		std::wstring( L"CS_Horizontal" ),
		std::wstring( L"cs_5_0" ) );

	pBilateralYEffect = new RenderEffectDX11();
	pBilateralYEffect->m_iComputeShader = 
		Renderer.LoadShader( COMPUTE_SHADER,
		std::wstring( L"../Data/Shaders/SeparableBilateralCS.hlsl" ),
		std::wstring( L"CS_Vertical" ),
		std::wstring( L"cs_5_0" ) );
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
void ViewOcclusion::Draw( PipelineManagerDX11* pPipelineManager, ParameterManagerDX11* pParamManager )
{
	
	// TODO: I added this bind statement here to force the DepthNormalBuffer to not
	//       be bound to the pipeline anymore.  This should be done automatically, or
	//       with an explicit method to clear render targets or something similar.
	//       One automatic way would be to check the resource being set for the shader
	//       resource parameter, to see if it is a render target view as well.  If so,
	//       then clear the render targets when the SRV is set.

	//pPipelineManager->ClearRenderTargets();
	//pPipelineManager->ApplyPipelineResources();

	// Process the occlusion buffer next.  Start by setting the needed resource
	// parameters for the depth/normal buffer and the occlusion buffer.

	pPipelineManager->ClearRenderTargets();
	pPipelineManager->ApplyRenderTargets();


	SetRenderParams( pParamManager );

	// Execute the compute shader to calculate the raw occlusion buffer.
	pPipelineManager->Dispatch( *pOcclusionEffect, ResolutionX / 32, ResolutionY / 32, 1, pParamManager );

	// Perform the blurring operations next.
	//pPipelineManager->Dispatch( *pBilateralXEffect, 1, ResolutionY, 1, pParamManager );
	//pPipelineManager->Dispatch( *pBilateralYEffect, ResolutionX, 1, 1, pParamManager );
	//pPipelineManager( *pBilateralXEffect, 1, ResolutionY, 1 );
	//pPipelineManager( *pBilateralYEffect, ResolutionX, 1, 1 );

	// Perform the final rendering pass now.  This will use the ViewOcclusion
	// output parameters (i.e. a shader resource view with occlusion buffer in it), and 
	// render with the perspective view's draw method.
	//ViewOcclusion::SetUsageParams( pParamManager );
	//ViewPerspective::SetRenderParams( pParamManager );

	//ViewPerspective::Draw( pPipelineManager, pParamManager );

	// Add the visualization rendering into the scene
	//pVisActor->GetNode()->Render( pPipelineManager, pParamManager, VT_PERSPECTIVE );
	
}
//--------------------------------------------------------------------------------
void ViewOcclusion::SetRenderParams( ParameterManagerDX11* pParamManager )
{
	// Set the parameters for this view to be able to perform its processing
	// sequence.  In this case, we set the depth/normal buffer as a shader 
	// resource and the occlusion buffer as an unordered access view.

	pParamManager->SetShaderResourceParameter( L"DepthNormalBuffer", DepthNormalBuffer );
	pParamManager->SetUnorderedAccessParameter( L"AmbientOcclusionTarget", OcclusionBuffer );
}
//--------------------------------------------------------------------------------
void ViewOcclusion::SetUsageParams( ParameterManagerDX11* pParamManager )
{
	// Set the parameters for allowing an application to use the current resources
	// for rendering.

	pParamManager->SetShaderResourceParameter( L"AmbientOcclusionBuffer", OcclusionBuffer );
}
//--------------------------------------------------------------------------------
