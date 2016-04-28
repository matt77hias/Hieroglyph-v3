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
#include "DiffuseSphereEntity.h"
#include "Log.h"
#include "GeometryDX11.h"
#include "RenderParameterDX11.h"
#include "ParameterManagerDX11.h"
#include "RasterizerStateConfigDX11.h"
#include "BlendStateConfigDX11.h"
#include "DepthStencilStateConfigDX11.h"
#include "SamplerStateConfigDX11.h"
#include "GeometryLoaderDX11.h"
#include "ShaderResourceParameterDX11.h"
#include "ShaderResourceParameterWriterDX11.h"
#include "SamplerParameterDX11.h"
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
ResourcePtr DiffuseSphereEntity::ColorTexture;
ShaderResourceParameterDX11* DiffuseSphereEntity::TextureParameter = NULL;
int DiffuseSphereEntity::LinearSampler = -1;
RenderEffectDX11* DiffuseSphereEntity::RenderEffect = nullptr;
RenderEffectDX11* DiffuseSphereEntity::ParabolaEffect = nullptr;
GeometryPtr DiffuseSphereEntity::SphereGeometry = nullptr;
MaterialPtr DiffuseSphereEntity::DiffuseMaterial = nullptr;
//--------------------------------------------------------------------------------
DiffuseSphereEntity::DiffuseSphereEntity()
{
	Visual.SetGeometry( SphereGeometry );
	Visual.SetMaterial( DiffuseMaterial );
}
//--------------------------------------------------------------------------------
DiffuseSphereEntity::~DiffuseSphereEntity()
{
}
//--------------------------------------------------------------------------------
void DiffuseSphereEntity::LoadResources()
{
    RendererDX11* pRenderer11 = RendererDX11::Get();

    // Get the geometry to render
    //SphereGeometry = GeometryLoaderDX11::loadMS3DFile2( L"UnitSphere2.ms3d" );
    SphereGeometry = GeometryLoaderDX11::loadMS3DFile2( L"box.ms3d" );
    SphereGeometry->LoadToBuffers();
    SphereGeometry->SetPrimitiveType( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );

	DiffuseMaterial = MaterialPtr( new MaterialDX11() );

    RenderEffect = new RenderEffectDX11();
    RenderEffect->SetVertexShader( pRenderer11->LoadShader( VERTEX_SHADER,     
		std::wstring( L"ObjectTexturedVS.hlsl" ),
		std::wstring( L"VSMAIN" ),
		std::wstring( L"vs_5_0" ) ) );

	RenderEffect->SetPixelShader( pRenderer11->LoadShader( PIXEL_SHADER,
		std::wstring( L"ObjectTexturedPS.hlsl" ),
		std::wstring( L"PSMAIN" ),
		std::wstring( L"ps_5_0" ) ) );

	// Enable the material to render the given view type, and set its effect.
	DiffuseMaterial->Params[VT_PERSPECTIVE].bRender = true;
	DiffuseMaterial->Params[VT_PERSPECTIVE].pEffect = RenderEffect;	

    // Create and fill the effect that will be used for a diffuse object 
    // to be rendered into a paraboloid map, which will use the paraboloid
    // projection with its normal sampling technique.

    ParabolaEffect = new RenderEffectDX11();

    ParabolaEffect->SetVertexShader( pRenderer11->LoadShader( VERTEX_SHADER,
		std::wstring( L"DualParaboloidEnvMapGen.hlsl" ),
		std::wstring( L"VSMAIN" ),
		std::wstring( L"vs_5_0" ) ) );

    ParabolaEffect->SetGeometryShader( pRenderer11->LoadShader( GEOMETRY_SHADER,
	    std::wstring( L"DualParaboloidEnvMapGen.hlsl" ),
		std::wstring( L"GSMAIN" ),
		std::wstring( L"gs_5_0" ) ) );

    ParabolaEffect->SetPixelShader( pRenderer11->LoadShader( PIXEL_SHADER,
	    std::wstring( L"DualParaboloidEnvMapGen.hlsl" ),
		std::wstring( L"PSMAIN" ),
		std::wstring( L"ps_5_0" ) ) );

    //RasterizerStateConfigDX11 RS;
    //RS.FillMode = D3D11_FILL_WIREFRAME;
    //RS.CullMode = D3D11_CULL_NONE;

    //ParabolaEffect->m_iRasterizerState = 
    //	pRenderer11->CreateRasterizerState( &RS );

	// Enable the material to render the given view type, and set its effect.
	DiffuseMaterial->Params[VT_DUAL_PARABOLOID_ENVMAP].bRender = true;
	DiffuseMaterial->Params[VT_DUAL_PARABOLOID_ENVMAP].pEffect = ParabolaEffect;

	SphereGeometry->GenerateInputLayout( RenderEffect->GetVertexShader() );
	SphereGeometry->GenerateInputLayout( ParabolaEffect->GetVertexShader() );

	RenderEffect->ConfigurePipeline( pRenderer11->pImmPipeline, pRenderer11->m_pParamMgr );
	ParabolaEffect->ConfigurePipeline( pRenderer11->pImmPipeline, pRenderer11->m_pParamMgr );


    ColorTexture = pRenderer11->LoadTexture( L"Tiles.png" );
	DiffuseMaterial->Parameters.SetShaderResourceParameter( L"ColorTexture", ColorTexture );

    SamplerStateConfigDX11 SamplerConfig;
    SamplerConfig.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
    SamplerConfig.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;

    LinearSampler = pRenderer11->CreateSamplerState( &SamplerConfig );
	DiffuseMaterial->Parameters.SetSamplerParameter( L"LinearSampler", LinearSampler );
}
//--------------------------------------------------------------------------------
