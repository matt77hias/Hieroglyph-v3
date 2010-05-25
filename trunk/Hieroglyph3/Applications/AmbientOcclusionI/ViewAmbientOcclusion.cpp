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

	// Create the visualization actor and send in the occlusion buffer

	pVisActor = ActorGenerator::GenerateVisualizationTexture2D( Renderer, 
			DepthNormalBuffer, 0 );
}
//--------------------------------------------------------------------------------
ViewAmbientOcclusion::~ViewAmbientOcclusion()
{
	SAFE_DELETE( pOcclusionEffect );
	SAFE_DELETE( pBilateralXEffect );
	SAFE_DELETE( pBilateralYEffect );

	SAFE_DELETE( pVisActor );
}
//--------------------------------------------------------------------------------
void ViewAmbientOcclusion::Update( float fTime )
{
}
//--------------------------------------------------------------------------------
void ViewAmbientOcclusion::Draw( RendererDX11& Renderer )
{
	// Set the view matrix if this render view is associated with an entity.
	if ( m_pEntity != NULL )
		ViewMatrix = m_pEntity->GetView();

	// Render the depth/normal buffer, keeping the depth target for a later pass.
	if ( m_pRoot )
	{
		// Run through the graph and pre-render the entities
		m_pRoot->PreRender( Renderer, VT_LINEAR_DEPTH_NORMALS );

		// Set the parameters for rendering this view
		Renderer.BindRenderTargets( DepthNormalBuffer, m_DepthTarget );
		Renderer.ClearBuffers( m_vColor, 1.0f );

		// Set the perspective view's render parameters, since the depth normal
		// buffer is a perspective rendering.
		ViewPerspective::SetRenderParams( Renderer );

		// Run through the graph and render each of the entities
		m_pRoot->Render( Renderer, VT_LINEAR_DEPTH_NORMALS );
	}
	
	// TODO: I added this bind statement here to force the DepthNormalBuffer to not
	//       be bound to the pipeline anymore.  This should be done automatically, or
	//       with an explicit method to clear render targets or something similar.
	//       One automatic way would be to check the resource being set for the shader
	//       resource parameter, to see if it is a render target view as well.  If so,
	//       then clear the render targets when the SRV is set.

	Renderer.BindRenderTargets( m_RenderTarget, m_DepthTarget );

	// Process the occlusion buffer next.  Start by setting the needed resource
	// parameters for the depth/normal buffer and the occlusion buffer.
	ViewAmbientOcclusion::SetRenderParams( Renderer );

	// Execute the compute shader to calculate the raw occlusion buffer.
	Renderer.Dispatch( *pOcclusionEffect, ResolutionX / 32, ResolutionY / 32, 1 );

	// Perform the blurring operations next.
	Renderer.Dispatch( *pBilateralXEffect, 1, ResolutionY, 1 );
	Renderer.Dispatch( *pBilateralYEffect, ResolutionX, 1, 1 );
	//Renderer.Dispatch( *pBilateralXEffect, 1, ResolutionY, 1 );
	//Renderer.Dispatch( *pBilateralYEffect, ResolutionX, 1, 1 );

	// Perform the final rendering pass now.  This will use the ViewAmbientOcclusion
	// output parameters (i.e. a shader resource view with occlusion buffer in it), and 
	// render with the perspective view's draw method.
	ViewAmbientOcclusion::SetUsageParams( Renderer );
	ViewPerspective::SetRenderParams( Renderer );

	ViewPerspective::Draw( Renderer );

	// Add the visualization rendering into the scene
	pVisActor->GetNode()->Render( Renderer, VT_PERSPECTIVE );
	
}
//--------------------------------------------------------------------------------
void ViewAmbientOcclusion::SetRenderParams( RendererDX11& Renderer )
{
	// Set the parameters for this view to be able to perform its processing
	// sequence.  In this case, we set the depth/normal buffer as a shader 
	// resource and the occlusion buffer as an unordered access view.

	Renderer.SetShaderResourceParameter( L"DepthNormalBuffer", DepthNormalBuffer );
	Renderer.SetUnorderedAccessParameter( L"AmbientOcclusionTarget", OcclusionBuffer );
}
//--------------------------------------------------------------------------------
void ViewAmbientOcclusion::SetUsageParams( RendererDX11& Renderer )
{
	// Set the parameters for allowing an application to use the current resources
	// for rendering.

	Renderer.SetShaderResourceParameter( L"AmbientOcclusionBuffer", OcclusionBuffer );
}
//--------------------------------------------------------------------------------
