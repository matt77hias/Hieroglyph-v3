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
#include "ReflectiveSphereEntity.h"
#include "Log.h"
#include "GeometryDX11.h"
#include "RenderParameterDX11.h"
#include "ParameterManagerDX11.h"
#include "RasterizerStateConfigDX11.h"
#include "BlendStateConfigDX11.h"
#include "DepthStencilStateConfigDX11.h"
#include "SamplerStateConfigDX11.h"
#include "GeometryLoaderDX11.h"
#include "Texture2dConfigDX11.h"
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
ReflectiveSphereEntity::ReflectiveSphereEntity()
{
	RendererDX11* pRenderer11 = RendererDX11::Get();

	// Get the geometry to render
	GeometryDX11* pGeometry = 0;
	pGeometry = GeometryLoaderDX11::loadMS3DFile2( L"../Data/Models/UnitSphere2.ms3d" );
	pGeometry->LoadToBuffers();
	pGeometry->SetPrimitiveType( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
	

	// Create the material for use by the entity.
	MaterialDX11* pMaterial = new MaterialDX11();

	// Create and fill the effect that will be used for rendering the reflective material
	// using the paraboloid maps generated with the ViewParaboloidEnvMap render view.
	RenderEffectDX11* pEffect = new RenderEffectDX11();

	pEffect->m_iVertexShader = 
		pRenderer11->LoadShader( VERTEX_SHADER,
		std::wstring( L"../Data/Shaders/DualParaboloidEnvMapLookup.hlsl" ),
		std::wstring( L"VSMAIN" ),
		std::wstring( L"vs_5_0" ) );
	pEffect->m_iPixelShader = 
		pRenderer11->LoadShader( PIXEL_SHADER,
		std::wstring( L"../Data/Shaders/DualParaboloidEnvMapLookup.hlsl" ),
		std::wstring( L"PSMAIN" ),
		std::wstring( L"ps_5_0" ) );

static int size = 256;
	// Create the resources to be used for generating the paraboloid maps.
	Texture2dConfigDX11 ColorConfig;
	ColorConfig.SetColorBuffer( size, size );
	ColorConfig.SetArraySize( 2 );
	ColorConfig.SetBindFlags( D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET );
	ResourcePtr ColorTarget = pRenderer11->CreateTexture2D( &ColorConfig, 0 );


	Texture2dConfigDX11 DepthConfig;
	DepthConfig.SetDepthBuffer( size, size );
	DepthConfig.SetArraySize( 2 );
	ResourcePtr DepthTarget = pRenderer11->CreateTexture2D( &DepthConfig, 0 );

	m_pParaboloidView = new ViewParaboloidEnvMap( *pRenderer11, ColorTarget, DepthTarget );
	m_pParaboloidView->SetEntity( this );
	m_pParaboloidView->SetBackColor( Vector4f( 0.1f, 0.1f, 0.3f, 1.0f ) );

	// Enable the material to render the given view type, and set its effect.
	pMaterial->Params[VT_PERSPECTIVE].bRender = true;
	pMaterial->Params[VT_PERSPECTIVE].pEffect = pEffect;
	pMaterial->Params[VT_PERSPECTIVE].vViews.add( m_pParaboloidView );

	// Create and fill the effect that will be used for a reflective object 
	// to be rendered into a paraboloid map, which will sample its own paraboloid
	// maps...
	RenderEffectDX11* pParabGenEffect = new RenderEffectDX11();

	pParabGenEffect->m_iVertexShader = 
		pRenderer11->LoadShader( VERTEX_SHADER,
		std::wstring( L"../Data/Shaders/DualParaboloidEnvMapLookupGen.hlsl" ),
		std::wstring( L"VSMAIN" ),
		std::wstring( L"vs_5_0" ) );
	pParabGenEffect->m_iGeometryShader = 
		pRenderer11->LoadShader( GEOMETRY_SHADER,
		std::wstring( L"../Data/Shaders/DualParaboloidEnvMapLookupGen.hlsl" ),
		std::wstring( L"GSMAIN" ),
		std::wstring( L"gs_5_0" ) );
	pParabGenEffect->m_iPixelShader = 
		pRenderer11->LoadShader( PIXEL_SHADER,
		std::wstring( L"../Data/Shaders/DualParaboloidEnvMapLookupGen.hlsl" ),
		std::wstring( L"PSMAIN" ),
		std::wstring( L"ps_5_0" ) );

	// Enable the material to render the given view type, and set its effect.
	pMaterial->Params[VT_DUAL_PARABOLOID_ENVMAP].bRender = true;
	pMaterial->Params[VT_DUAL_PARABOLOID_ENVMAP].pEffect = pParabGenEffect;
	pMaterial->Params[VT_DUAL_PARABOLOID_ENVMAP].vViews.add( m_pParaboloidView );


	//ParticleTexture = pRenderer11->LoadTexture( L"../Data/Textures/Particle.png" );
	
	SamplerStateConfigDX11 SamplerConfig;
	SamplerConfig.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
	SamplerConfig.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
	SamplerConfig.BorderColor[0] = 1.0f;
	SamplerConfig.BorderColor[1] = 0.0f;
	SamplerConfig.BorderColor[2] = 0.0f;
	SamplerConfig.BorderColor[3] = 0.0f;
	ParaboloidSampler = pRenderer11->CreateSamplerState( &SamplerConfig );

	//pRenderer11->m_pParamMgr->SetShaderResourceParameter( L"ParticleTexture", ParticleTexture );
	pRenderer11->m_pParamMgr->SetSamplerParameter( L"ParaboloidSampler", &ParaboloidSampler );


	// Set any parameters that will be needed by the shaders used above.
	
//	Vector4f DispatchSize = Vector4f( (float)DispatchSizeX, (float)DispatchSizeZ, (float)DispatchSizeX * 16, (float)DispatchSizeZ * 16 );
//	pRenderer11->m_pParamMgr->SetVectorParameter( std::wstring( L"DispatchSize" ), &DispatchSize );

//	Vector4f FinalColor = Vector4f( 0.5f, 1.0f, 0.5f, 1.0f );
//	pRenderer11->m_pParamMgr->SetVectorParameter( std::wstring( L"FinalColor" ), &FinalColor );

	pGeometry->GenerateInputLayout( pEffect->m_iVertexShader );
	pGeometry->GenerateInputLayout( pParabGenEffect->m_iVertexShader );

	pEffect->ConfigurePipeline( pRenderer11->pImmPipeline, pRenderer11->m_pParamMgr );
	pParabGenEffect->ConfigurePipeline( pRenderer11->pImmPipeline, pRenderer11->m_pParamMgr );


	this->SetGeometry( pGeometry );
	this->SetMaterial( pMaterial, false );

}
//--------------------------------------------------------------------------------
ReflectiveSphereEntity::~ReflectiveSphereEntity()
{
}
//--------------------------------------------------------------------------------
void ReflectiveSphereEntity::PreRender( RendererDX11* pRenderer, VIEWTYPE view )
{
	// Perform the pre-render function only if the material has been set
	if ( m_sParams.pMaterial )
		m_sParams.pMaterial->PreRender( pRenderer, view );
}
//--------------------------------------------------------------------------------
void ReflectiveSphereEntity::Render( PipelineManagerDX11* pPipelineManager, ParameterManagerDX11* pParamManager, VIEWTYPE view )
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

			// Send the geometry to the renderer using the appropriate
			// material view effect.
			pPipelineManager->Draw( *m_sParams.pMaterial->Params[view].pEffect, *m_sParams.pGeometry, pParamManager );
		}
	}
}
//--------------------------------------------------------------------------------
