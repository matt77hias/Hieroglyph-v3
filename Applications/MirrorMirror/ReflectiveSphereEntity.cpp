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
#include "SamplerParameterDX11.h"
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
ReflectiveSphereEntity::ReflectiveSphereEntity()
{
	RendererDX11* pRenderer11 = RendererDX11::Get();

	// Get the geometry to render
	GeometryPtr pGeometry = GeometryLoaderDX11::loadMS3DFile2( L"UnitSphere2.ms3d" );
	pGeometry->LoadToBuffers();
	pGeometry->SetPrimitiveType( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
	

	// Create the material for use by the entity.
	MaterialPtr pMaterial = MaterialPtr( new MaterialDX11() );


	// Create and fill the effect that will be used for rendering the reflective material
	// using the paraboloid maps generated with the ViewParaboloidEnvMap render view.

	RenderEffectDX11* pEffect = new RenderEffectDX11();

	pEffect->SetVertexShader( pRenderer11->LoadShader( VERTEX_SHADER,
		std::wstring( L"DualParaboloidEnvMapLookup.hlsl" ),
		std::wstring( L"VSMAIN" ),
		std::wstring( L"vs_5_0" ) ) );

	pEffect->SetPixelShader( pRenderer11->LoadShader( PIXEL_SHADER,
		std::wstring( L"DualParaboloidEnvMapLookup.hlsl" ),
		std::wstring( L"PSMAIN" ),
		std::wstring( L"ps_5_0" ) ) );

	// Specify the size of the paraboloid maps to use.

	static int size = 256;


	// Create the resources to be used for generating the paraboloid maps.  This
	// consists of a depth buffer and a color buffer array with two elements.  The
	// resources are always created with a square shape.

	Texture2dConfigDX11 ColorConfig;
	ColorConfig.SetColorBuffer( size, size );
	ColorConfig.SetArraySize( 2 );
	ColorConfig.SetBindFlags( D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET );
	ResourcePtr ColorTarget = pRenderer11->CreateTexture2D( &ColorConfig, 0 );

	Texture2dConfigDX11 DepthConfig;
	DepthConfig.SetDepthBuffer( size, size );
	DepthConfig.SetArraySize( 2 );
	ResourcePtr DepthTarget = pRenderer11->CreateTexture2D( &DepthConfig, 0 );

	
	// Create the render view that will be used to generate the paraboloid maps.

	m_pParaboloidView = new ViewParaboloidEnvMap( *pRenderer11, ColorTarget, DepthTarget );
	m_pParaboloidView->SetEntity( this );
	m_pParaboloidView->SetBackColor( Vector4f( 0.1f, 0.1f, 0.3f, 1.0f ) );

	// Enable the material to render the given view type, and set its effect.
	
	pMaterial->Params[VT_PERSPECTIVE].bRender = true;
	pMaterial->Params[VT_PERSPECTIVE].pEffect = pEffect;
	pMaterial->Params[VT_PERSPECTIVE].Tasks.push_back( m_pParaboloidView );

	
	// Create and fill the effect that will be used for a reflective object 
	// to be rendered into a paraboloid map, which will sample its own paraboloid
	// maps...
	
	RenderEffectDX11* pParabGenEffect = new RenderEffectDX11();

	pParabGenEffect->SetVertexShader( pRenderer11->LoadShader( VERTEX_SHADER,
		std::wstring( L"DualParaboloidEnvMapLookupGen.hlsl" ),
		std::wstring( L"VSMAIN" ),
		std::wstring( L"vs_5_0" ) ) );

	pParabGenEffect->SetGeometryShader(	pRenderer11->LoadShader( GEOMETRY_SHADER,
		std::wstring( L"DualParaboloidEnvMapLookupGen.hlsl" ),
		std::wstring( L"GSMAIN" ),
		std::wstring( L"gs_5_0" ) ) );
	
	pParabGenEffect->SetPixelShader( pRenderer11->LoadShader( PIXEL_SHADER,
		std::wstring( L"DualParaboloidEnvMapLookupGen.hlsl" ),
		std::wstring( L"PSMAIN" ),
		std::wstring( L"ps_5_0" ) ) );

	// Enable the material to render the given view type, and set its effect.
	
	pMaterial->Params[VT_DUAL_PARABOLOID_ENVMAP].bRender = true;
	pMaterial->Params[VT_DUAL_PARABOLOID_ENVMAP].pEffect = pParabGenEffect;
	pMaterial->Params[VT_DUAL_PARABOLOID_ENVMAP].Tasks.push_back( m_pParaboloidView );

	SamplerStateConfigDX11 SamplerConfig;
	SamplerConfig.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
	SamplerConfig.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
	SamplerConfig.BorderColor[0] = 1.0f;
	SamplerConfig.BorderColor[1] = 0.0f;
	SamplerConfig.BorderColor[2] = 0.0f;
	SamplerConfig.BorderColor[3] = 0.0f;
	m_iParaboloidSampler = pRenderer11->CreateSamplerState( &SamplerConfig );


	// Create the sampler parameter to specify the desired sampling method.

	SamplerParameterDX11* pSamplerParameter = pRenderer11->m_pParamMgr->GetSamplerStateParameterRef( std::wstring( L"ParaboloidSampler" ) );
    pSamplerParameter->InitializeParameterData( &m_iParaboloidSampler );


	// These operations are performed here at initialization time to ensure 
	// that the multithreading systems don't create any contention scenarios.

	pGeometry->GenerateInputLayout( pEffect->GetVertexShader() );
	pGeometry->GenerateInputLayout( pParabGenEffect->GetVertexShader() );

	pEffect->ConfigurePipeline( pRenderer11->pImmPipeline, pRenderer11->m_pParamMgr );
	pParabGenEffect->ConfigurePipeline( pRenderer11->pImmPipeline, pRenderer11->m_pParamMgr );

	Visual.SetGeometry( pGeometry );
	Visual.SetMaterial( pMaterial );
}
//--------------------------------------------------------------------------------
ReflectiveSphereEntity::~ReflectiveSphereEntity()
{
	SAFE_DELETE( m_pParaboloidView );
}
//--------------------------------------------------------------------------------
