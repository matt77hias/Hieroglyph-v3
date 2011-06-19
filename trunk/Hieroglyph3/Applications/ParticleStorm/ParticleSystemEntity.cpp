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
#include "ParticleSystemEntity.h"
#include "Log.h"
#include "GeometryDX11.h"
#include "RenderParameterDX11.h"
#include "IParameterManager.h"
#include "RasterizerStateConfigDX11.h"
#include "BlendStateConfigDX11.h"
#include "DepthStencilStateConfigDX11.h"
#include "SamplerStateConfigDX11.h"
#include "ShaderResourceParameterWriterDX11.h"
#include "SamplerParameterWriterDX11.h"
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
ParticleSystemEntity::ParticleSystemEntity()
{
	RendererDX11* pRenderer11 = RendererDX11::Get();

	// Create a geometry object *WITHOUT* any vertex data
	GeometryDX11* pGeometry = new GeometryDX11();

	for ( int i = 0; i<512; i++ )
		pGeometry->AddPoint( PointIndices( i ) );

	pGeometry->LoadToBuffers();
	pGeometry->SetPrimitiveType( D3D11_PRIMITIVE_TOPOLOGY_POINTLIST );
	

	// Create the material for use by the entity.
	MaterialDX11* pMaterial = new MaterialDX11();

	// Create and fill the effect that will be used for this view type
	RenderEffectDX11* pEffect = new RenderEffectDX11();

	pEffect->m_iVertexShader = 
		pRenderer11->LoadShader( VERTEX_SHADER,
		std::wstring( L"../Data/Shaders/ParticleSystemRender.hlsl" ),
		std::wstring( L"VSMAIN" ),
		std::wstring( L"vs_5_0" ) );
	pEffect->m_iGeometryShader = 
		pRenderer11->LoadShader( GEOMETRY_SHADER,
		std::wstring( L"../Data/Shaders/ParticleSystemRender.hlsl" ),
		std::wstring( L"GSMAIN" ),
		std::wstring( L"gs_5_0" ) );
	pEffect->m_iPixelShader = 
		pRenderer11->LoadShader( PIXEL_SHADER,
		std::wstring( L"../Data/Shaders/ParticleSystemRender.hlsl" ),
		std::wstring( L"PSMAIN" ),
		std::wstring( L"ps_5_0" ) );

	// We will use additive blending, so configure each state appropriately.
	// TODO: Should add a preconfigured method to the BlendStateConfigDX11 for
	//       doing this...
	BlendStateConfigDX11 BS;
	BS.RenderTarget[0].BlendEnable = true;
	BS.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
	BS.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
	BS.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD; 
	BS.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ZERO;
	BS.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	BS.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD; 
	BS.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	pEffect->m_iBlendState =
		pRenderer11->CreateBlendState( &BS );

	DepthStencilStateConfigDX11 DS;
	DS.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	pEffect->m_iDepthStencilState =
		pRenderer11->CreateDepthStencilState( &DS );

	// This block can be uncommented to allow wireframe viewing of the particles...
	//RasterizerStateConfigDX11 RS;
	//RS.FillMode = D3D11_FILL_WIREFRAME;
	//pEffect->m_iRasterizerState = 
	//	pRenderer11->CreateRasterizerState( &RS );

	// Create the render view that will run the simulation.  The size should be
	// selected accordingly for the maximum number of particles.

	m_pSimulation = new ViewSimulation( *pRenderer11, /*16384*/2048 * 32 );

	// Enable the material to render the given view type, and set its effect.
	pMaterial->Params[VT_PERSPECTIVE].bRender = true;
	pMaterial->Params[VT_PERSPECTIVE].pEffect = pEffect;
	pMaterial->Params[VT_PERSPECTIVE].vViews.add( m_pSimulation );


	// Create and attach the needed render parameters to the entity.  This consists of
	// the sampler and the texture being used for the particle rendering.

	SamplerStateConfigDX11 SamplerConfig;
	LinearSampler = pRenderer11->CreateSamplerState( &SamplerConfig );

	SamplerParameterWriterDX11* pSamplerWriter = new SamplerParameterWriterDX11();
	// TODO: this requires a cast for the SamplerParameterDX11* to RenderParameterDX11* - I don't know why...
	pSamplerWriter->SetRenderParameterRef( (RenderParameterDX11*)pRenderer11->m_pParamMgr->GetSamplerStateParameterRef( std::wstring( L"LinearSampler" ) ) );
	pSamplerWriter->SetValue( LinearSampler );
	this->AddRenderParameter( pSamplerWriter );

	ParticleTexture = pRenderer11->LoadTexture( L"../Data/Textures/Particle.png" );

	ShaderResourceParameterWriterDX11* pTextureWriter = new ShaderResourceParameterWriterDX11();
	pTextureWriter->SetRenderParameterRef( pRenderer11->m_pParamMgr->GetShaderResourceParameterRef( std::wstring( L"ParticleTexture" ) ) );
	pTextureWriter->SetValue( ParticleTexture );
	this->AddRenderParameter( pTextureWriter );


	this->SetGeometry( pGeometry );
	this->SetMaterial( pMaterial, false );

}
//--------------------------------------------------------------------------------
ParticleSystemEntity::~ParticleSystemEntity()
{
}
//--------------------------------------------------------------------------------
void ParticleSystemEntity::PreRender( RendererDX11* pRenderer, VIEWTYPE view )
{
	// Perform the pre-render function only if the material has been set
	if ( m_sParams.pMaterial )
		m_sParams.pMaterial->PreRender( pRenderer, view );
}
//--------------------------------------------------------------------------------
void ParticleSystemEntity::Render( PipelineManagerDX11* pPipelineManager, IParameterManager* pParamManager, VIEWTYPE view )
{
	// Test if the entity contains any geometry, and it has a material
	if ( ( m_sParams.pGeometry ) && ( m_sParams.pMaterial ) )
	{
		// Only render if the material indicates that you should
		if ( m_sParams.pMaterial->Params[view].bRender )
		{
			// Set the material render parameters.  This is done before the entity
			// render parameters so that unique values can be set by the individual
			// entities, and still allow the material to set parameters for any
			// entities that don't specialize the parameters.
			m_sParams.pMaterial->SetRenderParams( pParamManager, view );

			// Set the entity render parameters
			this->SetRenderParams( pParamManager );

			// Send the geometry to the renderer using the DrawIndirect() method.
			// This uses the indirect args buffer to determine how many particles
			// need to be rendered, allowing this entity to be oblivious to how many
			// particles it currently has...
			RenderEffectDX11* pEffect = m_sParams.pMaterial->Params[view].pEffect;

			pPipelineManager->DrawIndirect( 
				*pEffect, 
				m_pSimulation->GetParticleCountIndirectArgsBuffer(),
				0,
				m_sParams.pGeometry->GetInputLayout( pEffect->m_iVertexShader ),
				m_sParams.pGeometry->m_ePrimType,
				m_sParams.pGeometry->GetVertexSize(),
				pParamManager );
		}
	}
}
//--------------------------------------------------------------------------------
ViewSimulation* ParticleSystemEntity::GetSimulationView()
{
	return( m_pSimulation );
}
//--------------------------------------------------------------------------------