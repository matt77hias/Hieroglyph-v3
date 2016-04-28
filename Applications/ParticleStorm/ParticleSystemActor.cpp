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
#include "ParticleSystemActor.h"
#include "MaterialGeneratorDX11.h"
#include "ParticleSystemExecutorDX11.h"
#include "ShaderResourceParameterWriterDX11.h"
#include "BlendStateConfigDX11.h"
#include "DepthStencilStateConfigDX11.h"
#include "SamplerStateConfigDX11.h"
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
ParticleSystemActor::ParticleSystemActor()
{
	RendererDX11* pRenderer = RendererDX11::Get();

	m_pExecutor = ParticleSystemExecutorPtr( new ParticleSystemExecutorDX11() );
	
	// Create the material for use by the entity.
	m_pMaterial = MaterialPtr( new MaterialDX11() );

	// Create and fill the effect that will be used for this view type
	RenderEffectDX11* pEffect = new RenderEffectDX11();

	pEffect->SetVertexShader( pRenderer->LoadShader( VERTEX_SHADER,
		std::wstring( L"ParticleSystemRender.hlsl" ),
		std::wstring( L"VSMAIN" ),
		std::wstring( L"vs_5_0" ) ) );

	pEffect->SetGeometryShader( pRenderer->LoadShader( GEOMETRY_SHADER,
		std::wstring( L"ParticleSystemRender.hlsl" ),
		std::wstring( L"GSMAIN" ),
		std::wstring( L"gs_5_0" ) ) );

	pEffect->SetPixelShader( pRenderer->LoadShader( PIXEL_SHADER,
		std::wstring( L"ParticleSystemRender.hlsl" ),
		std::wstring( L"PSMAIN" ),
		std::wstring( L"ps_5_0" ) ) );

	// We will use additive blending, so configure each state appropriately.
	// TODO: Should add a preconfigured method to the BlendStateConfigDX11 for
	//       doing this...
	BlendStateConfigDX11 BS;
	BS.RenderTarget[0].BlendEnable = true;
	BS.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
	BS.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
	BS.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD; 
	BS.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	BS.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
	BS.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD; 
	BS.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	
	pEffect->m_iBlendState =
		pRenderer->CreateBlendState( &BS );

	DepthStencilStateConfigDX11 DS;
	DS.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	pEffect->m_iDepthStencilState =
		pRenderer->CreateDepthStencilState( &DS );

	// This block can be uncommented to allow wireframe viewing of the particles...
	//RasterizerStateConfigDX11 RS;
	//RS.FillMode = D3D11_FILL_WIREFRAME;
	//pEffect->m_iRasterizerState = 
	//	pRenderer11->CreateRasterizerState( &RS );

	// Create the render view that will run the simulation.  The size should be
	// selected accordingly for the maximum number of particles.

	m_pSimulation = new ViewSimulation( *pRenderer, 512*512 );

	m_pExecutor->SetArgumentsBuffer( m_pSimulation->GetParticleCountIndirectArgsBuffer() );

	// Enable the material to render the given view type, and set its effect.
	m_pMaterial->Params[VT_PERSPECTIVE].bRender = true;
	m_pMaterial->Params[VT_PERSPECTIVE].pEffect = pEffect;
	m_pMaterial->Params[VT_PERSPECTIVE].Tasks.push_back( m_pSimulation );


	// Create and attach the needed render parameters to the entity.  This consists of
	// the sampler and the texture being used for the particle rendering.

	SamplerStateConfigDX11 SamplerConfig;
	LinearSampler = pRenderer->CreateSamplerState( &SamplerConfig );
	GetBody()->Parameters.SetSamplerParameter( L"LinearSampler", LinearSampler );

	ParticleTexture = pRenderer->LoadTexture( L"Particle.png" );
	GetBody()->Parameters.SetShaderResourceParameter( L"ParticleTexture", ParticleTexture );


	GetBody()->Visual.SetGeometry( m_pExecutor );
	GetBody()->Visual.SetMaterial( m_pMaterial );
}
//--------------------------------------------------------------------------------
ParticleSystemActor::~ParticleSystemActor()
{
}
//--------------------------------------------------------------------------------