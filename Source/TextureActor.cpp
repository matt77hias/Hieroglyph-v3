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
#include "TextureActor.h"
#include "Texture2dDX11.h"
#include "BlendStateConfigDX11.h"
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
TextureActor::TextureActor( const ResourcePtr& texture, float scale ) :
	m_pMaterial( nullptr ),
	m_pGeometry( nullptr ),
	m_Texture( texture ),
	m_Color( 1.0f, 1.0f, 1.0f, 1.0f ),
	m_scale( scale )
{
	m_pGeometry = DrawGeometryPtr( new DrawExecutorDX11<TexturedVertex::Vertex>() );
	m_pGeometry->SetPrimitiveType( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP );
	m_pGeometry->SetLayoutElements( TexturedVertex::GetElementCount(), TexturedVertex::Elements );
	
	// Create the material
	m_pMaterial = MaterialPtr( new MaterialDX11() );

	// Create and fill the effect that will be used for this view type
	RenderEffectDX11* pEffect = new RenderEffectDX11();

	pEffect->SetVertexShader( RendererDX11::Get()->LoadShader( VERTEX_SHADER,
		std::wstring( L"Textured.hlsl" ),
		std::wstring( L"VSMAIN" ),
		std::wstring( L"vs_4_0" ) ) );

	pEffect->SetPixelShader( RendererDX11::Get()->LoadShader( PIXEL_SHADER,
		std::wstring( L"Textured.hlsl" ),
		std::wstring( L"PSMAIN" ),
		std::wstring( L"ps_4_0" ) ) );

	BlendStateConfigDX11 blendConfig;
	blendConfig.AlphaToCoverageEnable = false;
	blendConfig.IndependentBlendEnable = false;
	for ( int i = 0; i < 8; ++i )
	{
		blendConfig.RenderTarget[i].BlendEnable = true;
		blendConfig.RenderTarget[i].BlendOp = D3D11_BLEND_OP_ADD;
		blendConfig.RenderTarget[i].SrcBlend = D3D11_BLEND_SRC_ALPHA;
		blendConfig.RenderTarget[i].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
		blendConfig.RenderTarget[i].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		blendConfig.RenderTarget[i].SrcBlendAlpha = D3D11_BLEND_ONE;
		blendConfig.RenderTarget[i].DestBlendAlpha = D3D11_BLEND_ONE;
		blendConfig.RenderTarget[i].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	}

	pEffect->m_iBlendState = RendererDX11::Get()->CreateBlendState( &blendConfig );


	// Enable the material to render the given view type, and set its effect.
	m_pMaterial->Params[VT_PERSPECTIVE].bRender = true;
	m_pMaterial->Params[VT_PERSPECTIVE].pEffect = pEffect;

	GetBody()->Visual.SetMaterial( m_pMaterial );
	GetBody()->Visual.SetGeometry( m_pGeometry );

	SetTexture( texture, scale );
	SetColor( m_Color );
}
//--------------------------------------------------------------------------------
TextureActor::~TextureActor()
{
}
//--------------------------------------------------------------------------------
void TextureActor::SetTexture( const ResourcePtr& texture, float scale )
{
	// Record the texture and scale for use later on.
	m_Texture = texture;
	m_scale = scale;

	Vector2f size( 1.0f, 1.0f );

	// Inspect the texture to get its size
	ResourceDX11* pResource = RendererDX11::Get()->GetResourceByIndex( m_Texture->m_iResource );

	if ( pResource != nullptr )
	{
		assert( pResource->GetType() == RT_TEXTURE2D );

		Texture2dDX11* pTexture = (Texture2dDX11*)pResource;
		D3D11_TEXTURE2D_DESC desc = pTexture->GetActualDescription();

		size = Vector2f( static_cast<float>(desc.Width), static_cast<float>(desc.Height) );
	}

	// Create the geometry according to the resolution of the texture and the 
	// scale selection.
	m_pGeometry->ResetGeometry();
	
	TexturedVertex::Vertex v;

	// Top Left
	v.position = Vector3f( -0.5f * size.x, 0.5f * size.y, 0.0f ) * m_scale;
	v.texcoords = Vector2f( 0.0f, 0.0f );
	m_pGeometry->AddVertex( v );

	// Top Right
	v.position = Vector3f( 0.5f * size.x, 0.5f * size.y, 0.0f ) * m_scale;
	v.texcoords = Vector2f( 1.0f, 0.0f );
	m_pGeometry->AddVertex( v );

	// Bottom Left
	v.position = Vector3f( -0.5f * size.x, -0.5f * size.y, 0.0f ) * m_scale;
	v.texcoords = Vector2f( 0.0f, 1.0f );
	m_pGeometry->AddVertex( v );

	// Bottom Right
	v.position = Vector3f( 0.5f * size.x, -0.5f * size.y, 0.0f ) * m_scale;
	v.texcoords = Vector2f( 1.0f, 1.0f );
	m_pGeometry->AddVertex( v );

	
	
	// Bind the texture to this material for use in rendering.
	m_pMaterial->Parameters.SetShaderResourceParameter( L"ColorTexture", m_Texture );
}
//--------------------------------------------------------------------------------
void TextureActor::SetColor( const Vector4f& color )
{
	m_Color = color;
	m_pMaterial->Parameters.SetVectorParameter( L"objectColor", m_Color );
}
//--------------------------------------------------------------------------------
Vector4f TextureActor::GetColor( )
{
	return( m_Color );
}
//--------------------------------------------------------------------------------
