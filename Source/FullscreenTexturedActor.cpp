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
#include "FullscreenTexturedActor.h"
#include "Texture2dDX11.h"
#include "BlendStateConfigDX11.h"
#include "SamplerStateConfigDX11.h"
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
FullscreenTexturedActor::FullscreenTexturedActor( ResourcePtr texture ) :
	m_pMaterial( nullptr ),
	m_pGeometry( nullptr ),
	m_Texture( nullptr )
{
	m_pGeometry = DrawGeometryPtr( new DrawExecutorDX11<TexturedVertex::Vertex>() );
	m_pGeometry->SetPrimitiveType( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP );
	m_pGeometry->SetLayoutElements( TexturedVertex::GetElementCount(), TexturedVertex::Elements );
	
	// Create the material
	m_pMaterial = MaterialPtr( new MaterialDX11() );

	// Create and fill the effect that will be used for this view type
	RenderEffectDX11* pEffect = new RenderEffectDX11();

	pEffect->SetVertexShader( RendererDX11::Get()->LoadShader( VERTEX_SHADER,
		std::wstring( L"FullscreenTextured.hlsl" ),
		std::wstring( L"VSMAIN" ),
		std::wstring( L"vs_4_0" ) ) );

	pEffect->SetPixelShader( RendererDX11::Get()->LoadShader( PIXEL_SHADER,
		std::wstring( L"FullscreenTextured.hlsl" ),
		std::wstring( L"PSMAIN" ),
		std::wstring( L"ps_4_0" ) ) );

	//BlendStateConfigDX11 blendConfig;
	//blendConfig.AlphaToCoverageEnable = false;
	//blendConfig.IndependentBlendEnable = false;
	//for ( int i = 0; i < 8; ++i )
	//{
	//	blendConfig.RenderTarget[i].BlendEnable = true;
	//	blendConfig.RenderTarget[i].BlendOp = D3D11_BLEND_OP_ADD;
	//	blendConfig.RenderTarget[i].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	//	blendConfig.RenderTarget[i].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	//	blendConfig.RenderTarget[i].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	//	blendConfig.RenderTarget[i].SrcBlendAlpha = D3D11_BLEND_ONE;
	//	blendConfig.RenderTarget[i].DestBlendAlpha = D3D11_BLEND_ONE;
	//	blendConfig.RenderTarget[i].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	//}
	//pEffect->m_iBlendState = RendererDX11::Get()->CreateBlendState( &blendConfig );


	// Enable the material to render the given view type, and set its effect.
	m_pMaterial->Params[VT_PERSPECTIVE].bRender = true;
	m_pMaterial->Params[VT_PERSPECTIVE].pEffect = pEffect;

	GetBody()->Visual.SetMaterial( m_pMaterial );
	GetBody()->Visual.SetGeometry( m_pGeometry );

	// Create the geometry
	m_pGeometry->ResetGeometry();
	
	TexturedVertex::Vertex v;

	// Top Left
	v.position = Vector3f( -1.0f, 1.0f, 0.0f );
	v.texcoords = Vector2f( 0.0f, 0.0f );
	m_pGeometry->AddVertex( v );

	// Top Right
	v.position = Vector3f( 1.0f, 1.0f, 0.0f );
	v.texcoords = Vector2f( 1.0f, 0.0f );
	m_pGeometry->AddVertex( v );

	// Bottom Left
	v.position = Vector3f( -1.0f, -1.0f, 0.0f );
	v.texcoords = Vector2f( 0.0f, 1.0f );
	m_pGeometry->AddVertex( v );

	// Bottom Right
	v.position = Vector3f( 1.0f, -1.0, 0.0f );
	v.texcoords = Vector2f( 1.0f, 1.0f );
	m_pGeometry->AddVertex( v );

	SetTexture( texture );

	SamplerStateConfigDX11 SamplerConfig;
	SamplerConfig.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	SamplerConfig.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	SamplerConfig.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	SamplerConfig.MaxAnisotropy = 0;

	int LinearSampler = RendererDX11::Get()->CreateSamplerState( &SamplerConfig );
	m_pMaterial->Parameters.SetSamplerParameter( L"LinearSampler", LinearSampler );
}
//--------------------------------------------------------------------------------
FullscreenTexturedActor::~FullscreenTexturedActor()
{
}
//--------------------------------------------------------------------------------
void FullscreenTexturedActor::SetTexture( ResourcePtr texture )
{
	m_Texture = texture;
	m_pMaterial->Parameters.SetShaderResourceParameter( L"ColorTexture", m_Texture );
}
//--------------------------------------------------------------------------------
ResourcePtr FullscreenTexturedActor::GetTexture( )
{
	return( m_Texture );
}
//--------------------------------------------------------------------------------
