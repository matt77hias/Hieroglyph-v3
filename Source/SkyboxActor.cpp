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
#include "SkyboxActor.h"
#include "Texture2dDX11.h"
#include "BlendStateConfigDX11.h"
#include "DepthStencilStateConfigDX11.h"
#include "SamplerStateConfigDX11.h"
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
SkyboxActor::SkyboxActor( const ResourcePtr& texture, float scale ) :
	m_pMaterial( nullptr ),
	m_pGeometry( nullptr ),
	m_Texture( texture ),
	m_scale( scale )
{
	m_pGeometry = DrawGeometryPtr( new DrawIndexedExecutorDX11<TexturedVertex::Vertex>() );
	m_pGeometry->SetPrimitiveType( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
	m_pGeometry->SetLayoutElements( TexturedVertex::GetElementCount(), TexturedVertex::Elements );
	
	// Create the material
	m_pMaterial = MaterialPtr( new MaterialDX11() );

	// Create and fill the effect that will be used for this view type
	RenderEffectDX11* pEffect = new RenderEffectDX11();

	pEffect->SetVertexShader( RendererDX11::Get()->LoadShader( VERTEX_SHADER,
		std::wstring( L"Skybox.hlsl" ),
		std::wstring( L"VSMAIN" ),
		std::wstring( L"vs_4_0" ) ) );

	pEffect->SetPixelShader( RendererDX11::Get()->LoadShader( PIXEL_SHADER,
		std::wstring( L"Skybox.hlsl" ),
		std::wstring( L"PSMAIN" ),
		std::wstring( L"ps_4_0" ) ) );

	DepthStencilStateConfigDX11 ds;
	ds.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
	pEffect->m_iDepthStencilState = RendererDX11::Get()->CreateDepthStencilState( &ds );


	// Enable the material to render the given view type, and set its effect.
	m_pMaterial->Params[VT_PERSPECTIVE].bRender = true;
	m_pMaterial->Params[VT_PERSPECTIVE].pEffect = pEffect;

	// Add a sampler to the material for sampling the cube map.
	SamplerStateConfigDX11 SamplerConfig;
	int LinearSampler = RendererDX11::Get()->CreateSamplerState( &SamplerConfig );
	m_pMaterial->Parameters.SetSamplerParameter( L"LinearSampler", LinearSampler );

	GetBody()->Visual.SetMaterial( m_pMaterial );
	GetBody()->Visual.SetGeometry( m_pGeometry );

	SetTexture( texture, scale );
}
//--------------------------------------------------------------------------------
SkyboxActor::~SkyboxActor()
{
}
//--------------------------------------------------------------------------------
void SkyboxActor::SetTexture( const ResourcePtr& texture, float scale )
{
	// Record the texture and scale for use later on.
	m_Texture = texture;
	m_scale = scale;

	// Inspect the texture to get its size
	ResourceDX11* pResource = RendererDX11::Get()->GetResourceByIndex( m_Texture->m_iResource );
	assert( pResource->GetType() == RT_TEXTURE2D );

	Texture2dDX11* pTexture = (Texture2dDX11*)pResource;
	D3D11_TEXTURE2D_DESC desc = pTexture->GetActualDescription();

	Vector2f size( static_cast<float>(desc.Width), static_cast<float>(desc.Height) );

	// Create the geometry according to the resolution of the texture and the 
	// scale selection.
	m_pGeometry->ResetGeometry();
	
	TexturedVertex::Vertex v;

	// Top Left Front
	v.position = Vector3f( -1.0f, 1.0f, 1.0f ) * m_scale;
	v.texcoords = Vector2f( 0.0f, 0.0f );
	m_pGeometry->AddVertex( v );

	// Top Right Front
	v.position = Vector3f( 1.0f, 1.0f, 1.0f ) * m_scale;
	v.texcoords = Vector2f( 1.0f, 0.0f );
	m_pGeometry->AddVertex( v );

	// Bottom Left Front
	v.position = Vector3f( -1.0f, -1.0f, 1.0f ) * m_scale;
	v.texcoords = Vector2f( 0.0f, 1.0f );
	m_pGeometry->AddVertex( v );

	// Bottom Right Front
	v.position = Vector3f( 1.0f, -1.0f, 1.0f ) * m_scale;
	v.texcoords = Vector2f( 1.0f, 1.0f );
	m_pGeometry->AddVertex( v );

	// Top Left Back
	v.position = Vector3f( -1.0f, 1.0f, -1.0f ) * m_scale;
	v.texcoords = Vector2f( 0.0f, 0.0f );
	m_pGeometry->AddVertex( v );

	// Top Right Back
	v.position = Vector3f( 1.0f, 1.0f, -1.0f ) * m_scale;
	v.texcoords = Vector2f( 1.0f, 0.0f );
	m_pGeometry->AddVertex( v );

	// Bottom Left Back
	v.position = Vector3f( -1.0f, -1.0f, -1.0f ) * m_scale;
	v.texcoords = Vector2f( 0.0f, 1.0f );
	m_pGeometry->AddVertex( v );

	// Bottom Right Back
	v.position = Vector3f( 1.0f, -1.0f, -1.0f ) * m_scale;
	v.texcoords = Vector2f( 1.0f, 1.0f );
	m_pGeometry->AddVertex( v );
	
	// Front Face
	m_pGeometry->AddIndices( 0, 1, 2 );
	m_pGeometry->AddIndices( 1, 3, 2 );

	// Right Face
	m_pGeometry->AddIndices( 1, 5, 3 );
	m_pGeometry->AddIndices( 5, 7, 3 );

	// Back Face
	m_pGeometry->AddIndices( 5, 4, 6 );
	m_pGeometry->AddIndices( 5, 6, 7 );

	// Left Face
	m_pGeometry->AddIndices( 0, 2, 4 );
	m_pGeometry->AddIndices( 2, 6, 4 );

	// Top Face
	m_pGeometry->AddIndices( 4, 5, 0 );
	m_pGeometry->AddIndices( 5, 1, 0 );

	// Bottom Face
	m_pGeometry->AddIndices( 2, 3, 6 );
	m_pGeometry->AddIndices( 3, 7, 6 );
	
	// Bind the texture to this material for use in rendering.
	m_pMaterial->Parameters.SetShaderResourceParameter( L"SkyboxTexture", m_Texture );
}
//--------------------------------------------------------------------------------
