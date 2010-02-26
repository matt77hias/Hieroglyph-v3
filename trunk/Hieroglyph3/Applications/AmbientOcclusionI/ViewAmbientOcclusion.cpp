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
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
ViewAmbientOcclusion::ViewAmbientOcclusion( RendererDX11& Renderer, int SizeX, int SizeY )
{
	m_sParams.iViewType = VT_SIMULATION;

	ResolutionX = SizeX;
	ResolutionY = SizeY;

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
		std::wstring( L"../Data/Shaders/AmbientOcclusionCS_16.hlsl" ),
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
ViewAmbientOcclusion::~ViewAmbientOcclusion()
{
	SAFE_DELETE( pOcclusionEffect );
	SAFE_DELETE( pBilateralXEffect );
	SAFE_DELETE( pBilateralYEffect );
}
//--------------------------------------------------------------------------------
void ViewAmbientOcclusion::Update( float fTime )
{
}
//--------------------------------------------------------------------------------
void ViewAmbientOcclusion::Draw( RendererDX11& Renderer )
{
	// Set this view's render parameters.
	SetRenderParams( Renderer );

	// Perform the dispatch call to update the simulation state.
//	Renderer.Dispatch( *pWaterEffect, ThreadGroupsX, ThreadGroupsY, 1 );

	// Switch the two resources so that the current state is maintained in slot 0.
	//ResourcePtr TempState = WaterState[0];
	//WaterState[0] = WaterState[1];
	//WaterState[1] = TempState;
}
//--------------------------------------------------------------------------------
void ViewAmbientOcclusion::SetRenderParams( RendererDX11& Renderer )
{
	// Set the parameters for this view to be able to perform its processing
	// sequence.  In this case, water state '0' is always considered the current
	// state.

//	Renderer.SetShaderResourceParameter( L"CurrentWaterState", WaterState[0] );
//	Renderer.SetUnorderedAccessParameter( L"NewWaterState", WaterState[1] );
}
//--------------------------------------------------------------------------------
void ViewAmbientOcclusion::SetUsageParams( RendererDX11& Renderer )
{
	// Set the parameters for allowing an application to use the current state
	// as a height map via a shader resource view.

//	Renderer.SetShaderResourceParameter( L"CurrentWaterState", WaterState[0] );
}
//--------------------------------------------------------------------------------
