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
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
ResourcePtr DiffuseSphereEntity::ArgsBuffer;
ResourcePtr DiffuseSphereEntity::ColorTexture;
ShaderResourceParameterDX11* DiffuseSphereEntity::TextureParameter = NULL;
int DiffuseSphereEntity::LinearSampler = -1;
RenderEffectDX11* DiffuseSphereEntity::RenderEffect = NULL;
RenderEffectDX11* DiffuseSphereEntity::ParabolaEffect = NULL;
GeometryDX11* DiffuseSphereEntity::SphereGeometry = NULL;
//--------------------------------------------------------------------------------
DiffuseSphereEntity::DiffuseSphereEntity()
{
	MaterialDX11* pMaterial = new MaterialDX11();

	// Enable the material to render the given view type, and set its effect.
	pMaterial->Params[VT_PERSPECTIVE].bRender = true;
	pMaterial->Params[VT_PERSPECTIVE].pEffect = RenderEffect;	

	// Enable the material to render the given view type, and set its effect.
	pMaterial->Params[VT_DUAL_PARABOLOID_ENVMAP].bRender = true;
	pMaterial->Params[VT_DUAL_PARABOLOID_ENVMAP].pEffect = ParabolaEffect;
	//pMaterial->Params[VT_DUAL_PARABOLOID_ENVMAP].vViews.add( m_pParaboloidView );

	this->SetGeometry( SphereGeometry );
	this->SetMaterial( pMaterial, false );

    this->AddRenderParameter( TextureParameter );
}
//--------------------------------------------------------------------------------
void DiffuseSphereEntity::LoadResources()
{
    RendererDX11* pRenderer11 = RendererDX11::Get();

    // Get the geometry to render
    //pGeometry = GeometryLoaderDX11::loadMS3DFile2( L"../Data/Models/UnitSphere2.ms3d" );
    SphereGeometry = GeometryLoaderDX11::loadMS3DFile2( L"../Data/Models/box.ms3d" );
    SphereGeometry->LoadToBuffers();
    SphereGeometry->SetPrimitiveType( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );



    RenderEffect = new RenderEffectDX11();
    RenderEffect->m_iVertexShader = 
        pRenderer11->LoadShader( VERTEX_SHADER,
        std::wstring( L"../Data/Shaders/ObjectTexturedVS.hlsl" ),
        std::wstring( L"VSMAIN" ),
        std::wstring( L"vs_5_0" ) );
    RenderEffect->m_iPixelShader = 
        pRenderer11->LoadShader( PIXEL_SHADER,
        std::wstring( L"../Data/Shaders/ObjectTexturedPS.hlsl" ),
        std::wstring( L"PSMAIN" ),
        std::wstring( L"ps_5_0" ) );

    // Create and fill the effect that will be used for a diffuse object 
    // to be rendered into a paraboloid map, which will use the paraboloid
    // projection with its normal sampling technique.
    ParabolaEffect = new RenderEffectDX11();

    ParabolaEffect->m_iVertexShader = 
        pRenderer11->LoadShader( VERTEX_SHADER,
        std::wstring( L"../Data/Shaders/DualParaboloidEnvMapGen.hlsl" ),
        std::wstring( L"VSMAIN" ),
        std::wstring( L"vs_5_0" ) );
    ParabolaEffect->m_iGeometryShader = 
        pRenderer11->LoadShader( GEOMETRY_SHADER,
        std::wstring( L"../Data/Shaders/DualParaboloidEnvMapGen.hlsl" ),
        std::wstring( L"GSMAIN" ),
        std::wstring( L"gs_5_0" ) );
    ParabolaEffect->m_iPixelShader = 
        pRenderer11->LoadShader( PIXEL_SHADER,
        std::wstring( L"../Data/Shaders/DualParaboloidEnvMapGen.hlsl" ),
        std::wstring( L"PSMAIN" ),
        std::wstring( L"ps_5_0" ) );

    //RasterizerStateConfigDX11 RS;
    //RS.FillMode = D3D11_FILL_WIREFRAME;
    //RS.CullMode = D3D11_CULL_NONE;

    //pParabGenEffect->m_iRasterizerState = 
    //	pRenderer11->CreateRasterizerState( &RS );

	SphereGeometry->GenerateInputLayout( RenderEffect->m_iVertexShader );
	SphereGeometry->GenerateInputLayout( ParabolaEffect->m_iVertexShader );

	RenderEffect->ConfigurePipeline( pRenderer11->pImmPipeline, pRenderer11->m_pParamMgr );
	ParabolaEffect->ConfigurePipeline( pRenderer11->pImmPipeline, pRenderer11->m_pParamMgr );



    ColorTexture = pRenderer11->LoadTexture( L"../Data/Textures/Tiles.png" );

    TextureParameter = new ShaderResourceParameterDX11();
    TextureParameter->SetName( L"ColorTexture" );
    TextureParameter->SetParameterData( &ColorTexture->m_iResourceSRV );    

    SamplerStateConfigDX11 SamplerConfig;
    SamplerConfig.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
    SamplerConfig.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
    LinearSampler = pRenderer11->CreateSamplerState( &SamplerConfig );

    pRenderer11->m_pParamMgr->SetSamplerParameter( L"LinearSampler", &LinearSampler );


    // Set any parameters that will be needed by the shaders used above.

    //	Vector4f DispatchSize = Vector4f( (float)DispatchSizeX, (float)DispatchSizeZ, (float)DispatchSizeX * 16, (float)DispatchSizeZ * 16 );
    //	pRenderer11->m_pParamMgr->SetVectorParameter( std::wstring( L"DispatchSize" ), &DispatchSize );

    //	Vector4f FinalColor = Vector4f( 0.5f, 1.0f, 0.5f, 1.0f );
    //	pRenderer11->m_pParamMgr->SetVectorParameter( std::wstring( L"FinalColor" ), &FinalColor );
}
//--------------------------------------------------------------------------------
