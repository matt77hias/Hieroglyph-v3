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
#include "SpriteRendererDX11.h"
#include "RendererDX11.h"
#include "Log.h"
#include "BlendStateConfigDX11.h"
#include "RasterizerStateConfigDX11.h"
#include "DepthStencilStateConfigDX11.h"
#include "SpriteFontDX11.h"
#include "PipelineManagerDX11.h"
#include "ParameterManagerDX11.h"
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
SpriteRendererDX11::SpriteRendererDX11() :
									m_iInputLayout(-1),
									m_iLinearSamplerState(-1),
									m_iPointSamplerState(-1),
									m_bInitialized(false)
{

}
//--------------------------------------------------------------------------------
SpriteRendererDX11::~SpriteRendererDX11()
{

}
//--------------------------------------------------------------------------------
bool SpriteRendererDX11::Initialize()
{
	// Get the renderer
	RendererDX11* renderer = RendererDX11::Get();

	// Load the shaders
	m_effect.m_iVertexShader = renderer->LoadShader( VERTEX_SHADER,
		std::wstring( L"../Data/Shaders/Sprite.hlsl" ),
		std::wstring( L"SpriteVS" ),
		std::wstring( L"vs_4_0" ) );

	if ( m_effect.m_iVertexShader == -1 )
	{
		Log::Get().Write( L"Failed to load sprite vertex shader" );
		return false;
	}

	m_effect.m_iPixelShader = renderer->LoadShader( PIXEL_SHADER,
		std::wstring( L"../Data/Shaders/Sprite.hlsl" ),
		std::wstring( L"SpritePS" ),
		std::wstring( L"ps_4_0" ) );

	if ( m_effect.m_iPixelShader == -1 )
	{
		Log::Get().Write( L"Failed to load sprite vertex shader" );
		return false;
	}

	// Create an input layout
	TArray<D3D11_INPUT_ELEMENT_DESC> elements;
	D3D11_INPUT_ELEMENT_DESC elementsArray[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TRANSFORM", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 0, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "TRANSFORM", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 16, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "TRANSFORM", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 32, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "TRANSFORM", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 48, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 64, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "SOURCERECT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 80, D3D11_INPUT_PER_INSTANCE_DATA, 1 }
	};
	for ( int i = 0; i < sizeof(elementsArray) / sizeof(D3D11_INPUT_ELEMENT_DESC); ++i)
		elements.add( elementsArray[i] );

	m_iInputLayout = renderer->CreateInputLayout( elements, m_effect.m_iVertexShader );

	if ( m_effect.m_iPixelShader == -1 )
	{
		Log::Get().Write( L"Failed to create sprite input layout" );
		return false;
	}

	// Create the vertex buffer
	SpriteVertex verts[] =
	{
		{ Vector2f(0.0f, 0.0f), Vector2f(0.0f, 0.0f) },
		{ Vector2f(1.0f, 0.0f), Vector2f(1.0f, 0.0f) },
		{ Vector2f(1.0f, 1.0f), Vector2f(1.0f, 1.0f) },
		{ Vector2f(0.0f, 1.0f), Vector2f(0.0f, 1.0f) }
	};

	BufferConfigDX11 vbConfig;
	vbConfig.SetUsage( D3D11_USAGE_IMMUTABLE );
	vbConfig.SetByteWidth( sizeof(SpriteVertex) * 4 );
	vbConfig.SetBindFlags( D3D11_BIND_VERTEX_BUFFER );
	vbConfig.SetCPUAccessFlags( 0 );
	vbConfig.SetMiscFlags( 0 );

	D3D11_SUBRESOURCE_DATA initData;
	initData.pSysMem = verts;
	initData.SysMemPitch = 0;
	initData.SysMemSlicePitch = 0;

	m_pVertexBuffer = renderer->CreateVertexBuffer( &vbConfig, &initData );

	if ( m_pVertexBuffer->m_iResource == -1 )
	{
		Log::Get().Write( L"Failed to create sprite vertex buffer" );
		return false;
	}

	// Create the instance data buffer
	BufferConfigDX11 instanceConfig;
	instanceConfig.SetUsage( D3D11_USAGE_DYNAMIC );
	instanceConfig.SetByteWidth( sizeof(SpriteDrawData) * MaxBatchSize );
	instanceConfig.SetBindFlags( D3D11_BIND_VERTEX_BUFFER );
	instanceConfig.SetCPUAccessFlags( D3D11_CPU_ACCESS_WRITE );
	instanceConfig.SetMiscFlags( 0 );

	m_pInstanceDataBuffer = renderer->CreateVertexBuffer( &instanceConfig, NULL );

	if ( m_pInstanceDataBuffer->m_iResource == -1 )
	{
		Log::Get().Write( L"Failed to create sprite instance data buffer" );
		return false;
	}

	// Create the index buffer
	UINT indices[] = { 0, 1, 2, 3, 0, 2 };

	BufferConfigDX11 ibConfig;
	ibConfig.SetUsage( D3D11_USAGE_IMMUTABLE );
	ibConfig.SetByteWidth( sizeof(UINT) * 6 );
	ibConfig.SetBindFlags( D3D11_BIND_INDEX_BUFFER );
	ibConfig.SetCPUAccessFlags( 0 );
	ibConfig.SetMiscFlags( 0 );

	initData.pSysMem = indices;

	m_pIndexBuffer = renderer->CreateIndexBuffer( &ibConfig, &initData );

	if ( m_pIndexBuffer->m_iResource == -1 )
	{
		Log::Get().Write( L"Failed to create sprite index buffer" );
		return false;
	}

	// Create our states
	RasterizerStateConfigDX11 rsConfig;
	rsConfig.AntialiasedLineEnable = FALSE;
	rsConfig.CullMode = D3D11_CULL_NONE;
	rsConfig.DepthBias = 0;
	rsConfig.DepthBiasClamp = 1.0f;
	rsConfig.DepthClipEnable = false;
	rsConfig.FillMode = D3D11_FILL_SOLID;
	rsConfig.FrontCounterClockwise = false;
	rsConfig.MultisampleEnable = true;
	rsConfig.ScissorEnable = false;
	rsConfig.SlopeScaledDepthBias = 0;
	m_effect.m_iRasterizerState = renderer->CreateRasterizerState( &rsConfig );

	if ( m_effect.m_iRasterizerState == -1 )
	{
		Log::Get().Write( L"Failed to create sprite rasterizer state" );
		return false;
	}

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

	m_effect.m_iBlendState = renderer->CreateBlendState( &blendConfig );

	if ( m_effect.m_iBlendState == -1 )
	{
		Log::Get().Write( L"Failed to create sprite blend state" );
		return false;
	}

	DepthStencilStateConfigDX11 dsConfig;
	dsConfig.DepthEnable = FALSE;
	dsConfig.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	dsConfig.DepthFunc = D3D11_COMPARISON_LESS;
	dsConfig.StencilEnable = false;
	dsConfig.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
	dsConfig.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;
	dsConfig.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	dsConfig.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	dsConfig.FrontFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE;
	dsConfig.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	dsConfig.BackFace = dsConfig.FrontFace;

	m_effect.m_iDepthStencilState = renderer->CreateDepthStencilState( &dsConfig );

	if ( m_effect.m_iDepthStencilState == -1 )
	{
		Log::Get().Write( L"Failed to create sprite depth stencil state" );
		return false;
	}

	// Linear filtering sampler state
	D3D11_SAMPLER_DESC samplerConfig;
	samplerConfig.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerConfig.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerConfig.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerConfig.BorderColor[0] = 0;
	samplerConfig.BorderColor[1] = 0;
	samplerConfig.BorderColor[2] = 0;
	samplerConfig.BorderColor[3] = 0;
	samplerConfig.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerConfig.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerConfig.MaxAnisotropy = 1;
	samplerConfig.MaxLOD = D3D11_FLOAT32_MAX;
	samplerConfig.MinLOD = 0;
	samplerConfig.MipLODBias = 0;

	m_iLinearSamplerState = renderer->CreateSamplerState( &samplerConfig );

	if ( m_iLinearSamplerState == -1 )
	{
		Log::Get().Write( L"Failed to create sprite sampler state" );
		return false;
	}

	// Point filtering sampler state
	samplerConfig.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;

	m_iPointSamplerState = renderer->CreateSamplerState( &samplerConfig );

	if ( m_iLinearSamplerState == -1 )
	{
		Log::Get().Write( L"Failed to create sprite sampler state" );
		return false;
	}

	m_bInitialized = true;

	return true;
}
//--------------------------------------------------------------------------------
void SpriteRendererDX11::Render( PipelineManagerDX11* pipeline,
								 IParameterManager* parameters,
								 ResourcePtr texture,
								 const SpriteDrawData* drawData,
								 UINT numSprites, FilterMode filterMode )
{
	_ASSERT(m_bInitialized);

	RendererDX11::PIXBeginEvent( L"SpriteRendererDX11 Render" );

	// Make sure the draw rects are all valid
	D3D11_TEXTURE2D_DESC desc = texture->m_pTexture2dConfig->GetTextureDesc();
	for ( UINT i = 0; i < numSprites; ++i )
	{
		SpriteDrawRect drawRect = drawData[i].DrawRect;
		_ASSERT( drawRect.X >= 0 && drawRect.X < desc.Width );
		_ASSERT( drawRect.Y >= 0 && drawRect.Y < desc.Height );
		_ASSERT( drawRect.Width > 0 && drawRect.X + drawRect.Width <= desc.Width );
		_ASSERT( drawRect.Height > 0 && drawRect.Y + drawRect.Height <= desc.Height );
	}

	UINT numSpritesToDraw = min( numSprites, MaxBatchSize );

	// Copy in the instance data
	D3D11_MAPPED_SUBRESOURCE mapped;
	mapped = pipeline->MapResource( m_pInstanceDataBuffer->m_iResource,
									0, D3D11_MAP_WRITE_DISCARD, 0 );
	CopyMemory(mapped.pData, drawData, static_cast<size_t>(sizeof(SpriteDrawData) * numSpritesToDraw));
	pipeline->UnMapResource( m_pInstanceDataBuffer->m_iResource, 0 );

	// Set the constants
	Vector4f texAndViewportSize;
	texAndViewportSize.x = static_cast<float>( desc.Width );
	texAndViewportSize.y = static_cast<float>( desc.Height );

	D3D11_VIEWPORT vp = pipeline->GetCurrentViewport();
	texAndViewportSize.z = static_cast<float>( vp.Width );
	texAndViewportSize.w = static_cast<float>( vp.Height );

	parameters->SetVectorParameter( L"TexAndViewportSize", &texAndViewportSize );

	// Set the texture
	parameters->SetShaderResourceParameter( L"SpriteTexture", texture );

	// Set the sampler
	if ( filterMode == Linear )
		parameters->SetSamplerParameter( L"SpriteSampler", &m_iLinearSamplerState );
	else if ( filterMode == Point )
		parameters->SetSamplerParameter( L"SpriteSampler", &m_iPointSamplerState );

	// Draw
	pipeline->DrawInstanced( m_effect, m_pVertexBuffer, D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
								m_pIndexBuffer, m_iInputLayout, sizeof( SpriteVertex ), 6,
								m_pInstanceDataBuffer, sizeof(SpriteDrawData), numSpritesToDraw, parameters );

	RendererDX11::PIXEndEvent();

	// If there's any left to be rendered, do it recursively
	if(numSprites > numSpritesToDraw)
		Render( pipeline, parameters, texture, drawData + numSpritesToDraw, numSprites - numSpritesToDraw );
}
//--------------------------------------------------------------------------------
void SpriteRendererDX11::Render( PipelineManagerDX11* pipeline, 
								 IParameterManager* parameters,
							     ResourcePtr texture, 
								 const Matrix4f& transform,
								 const Vector4f& color, FilterMode filterMode,
								 const SpriteDrawRect* drawRect )
{
	SpriteDrawData data;
	data.Color = color;
	data.Transform = transform;

	if ( drawRect )
		data.DrawRect = *drawRect;
	else
	{
		// Draw the full texture
		D3D11_TEXTURE2D_DESC textureDesc = texture->m_pTexture2dConfig->GetTextureDesc();
		data.DrawRect.X = 0;
		data.DrawRect.Y = 0;
		data.DrawRect.Width = static_cast<float>( textureDesc.Width );
		data.DrawRect.Height = static_cast<float>( textureDesc.Height );
	}

	Render( pipeline, parameters, texture, &data, 1, filterMode );
}
//--------------------------------------------------------------------------------
void SpriteRendererDX11::RenderText( PipelineManagerDX11* pipeline, 
									 IParameterManager* parameters,
									 const SpriteFontDX11& font, const wchar_t* text,
									 const Matrix4f& transform, const Vector4f& color )
{
	RendererDX11::PIXBeginEvent( L"SpriteRenderer RenderText" );

	size_t length = wcslen( text );

	Matrix4f textTransform = Matrix4f::Identity();

	UINT numCharsToDraw = min( length, MaxBatchSize );
	UINT currentDraw = 0;
	for (UINT i = 0; i < numCharsToDraw; ++i)
	{
		wchar_t character = text[i];
		if(character == ' ')
			textTransform[12] += font.SpaceWidth();
		else if(character == '\n')
		{
			textTransform[13] += font.CharHeight();
			textTransform[12] = 0;
		}
		else
		{
			SpriteFontDX11::CharDesc desc = font.GetCharDescriptor(character);

			m_TextDrawData[currentDraw].Transform = textTransform * transform;
			m_TextDrawData[currentDraw].Color = color;
			m_TextDrawData[currentDraw].DrawRect.X = desc.X;
			m_TextDrawData[currentDraw].DrawRect.Y = desc.Y;
			m_TextDrawData[currentDraw].DrawRect.Width = desc.Width;
			m_TextDrawData[currentDraw].DrawRect.Height = desc.Height;
			currentDraw++;

			textTransform[12] += desc.Width + 1;
		}
	}

	// Submit a batch
	Render( pipeline, parameters, font.TextureResource(), m_TextDrawData, currentDraw, Point );

	RendererDX11::PIXEndEvent();

	if( length > numCharsToDraw )
		RenderText( pipeline, parameters, font, text + numCharsToDraw, textTransform, color );
}
//--------------------------------------------------------------------------------