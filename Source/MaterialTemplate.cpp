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
#include "MaterialTemplate.h"
#include "BlendStateConfigDX11.h"
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
MaterialTemplate::MaterialTemplate() :
	material( std::make_shared<MaterialDX11>() ),
	color_mode( ColorMode::VertexColor ),
	lighting_mode( LightingMode::Point ),
	transparency_mode( TransparencyMode::Opaque )
{
	RendererDX11* renderer = RendererDX11::Get();

	BlendStateConfigDX11 blendConfig;

	blendConfig.AlphaToCoverageEnable = false;
	blendConfig.IndependentBlendEnable = false;
	for ( int i = 0; i < 8; ++i )
	{
		blendConfig.RenderTarget[i].BlendEnable = false;
		blendConfig.RenderTarget[i].BlendOp = D3D11_BLEND_OP_ADD;
		blendConfig.RenderTarget[i].SrcBlend = D3D11_BLEND_SRC_ALPHA;
		blendConfig.RenderTarget[i].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
		blendConfig.RenderTarget[i].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		blendConfig.RenderTarget[i].SrcBlendAlpha = D3D11_BLEND_ONE;
		blendConfig.RenderTarget[i].DestBlendAlpha = D3D11_BLEND_ONE;
		blendConfig.RenderTarget[i].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	}

	opaque_blendstate = renderer->CreateBlendState( &blendConfig );

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

	alpha_blendstate = renderer->CreateBlendState( &blendConfig );

	objectcolor_pointlight_vs =
		renderer->LoadShader( VERTEX_SHADER,
			std::wstring( L"object-color.vertex-normal.point-light.perspective.vs.hlsl" ),
			std::wstring( L"VSMAIN" ),
			std::wstring( L"vs_4_0" ) );

	objectcolor_pointlight_ps =
		renderer->LoadShader( PIXEL_SHADER,
			std::wstring( L"object-color.vertex-normal.point-light.perspective.ps.hlsl" ),
			std::wstring( L"PSMAIN" ),
			std::wstring( L"ps_4_0" ) );


	vertexcolor_pointlight_vs =
		renderer->LoadShader( VERTEX_SHADER,
			std::wstring( L"vertex-color.vertex-normal.point-light.perspective.vs.hlsl" ),
			std::wstring( L"VSMAIN" ),
			std::wstring( L"vs_4_0" ) );

	vertexcolor_pointlight_ps =
		renderer->LoadShader( PIXEL_SHADER,
			std::wstring( L"vertex-color.vertex-normal.point-light.perspective.ps.hlsl" ),
			std::wstring( L"PSMAIN" ),
			std::wstring( L"ps_4_0" ) );

	texturecolor_pointlight_vs =
		renderer->LoadShader( VERTEX_SHADER,
			std::wstring( L"textured.vertex-normal.point-light.perspective.vs.hlsl" ),
			std::wstring( L"VSMAIN" ),
			std::wstring( L"vs_4_0" ) );

	texturecolor_pointlight_ps =
		renderer->LoadShader( PIXEL_SHADER,
			std::wstring( L"textured.vertex-normal.point-light.perspective.ps.hlsl" ),
			std::wstring( L"PSMAIN" ),
			std::wstring( L"ps_4_0" ) );

	objectcolor_flat_vs =
		renderer->LoadShader( VERTEX_SHADER,
			std::wstring( L"object-color.vertex-normal.flat.perspective.vs.hlsl" ),
			std::wstring( L"VSMAIN" ),
			std::wstring( L"vs_4_0" ) );

	objectcolor_flat_ps =
		renderer->LoadShader( PIXEL_SHADER,
			std::wstring( L"object-color.vertex-normal.flat.perspective.ps.hlsl" ),
			std::wstring( L"PSMAIN" ),
			std::wstring( L"ps_4_0" ) );

	vertexcolor_flat_vs =
		renderer->LoadShader( VERTEX_SHADER,
			std::wstring( L"vertex-color.vertex-normal.flat.perspective.vs.hlsl" ),
			std::wstring( L"VSMAIN" ),
			std::wstring( L"vs_4_0" ) );

	vertexcolor_flat_ps =
		renderer->LoadShader( PIXEL_SHADER,
			std::wstring( L"vertex-color.vertex-normal.flat.perspective.ps.hlsl" ),
			std::wstring( L"PSMAIN" ),
			std::wstring( L"ps_4_0" ) );

	texturecolor_flat_vs =
		renderer->LoadShader( VERTEX_SHADER,
			std::wstring( L"textured.vertex-normal.flat.perspective.vs.hlsl" ),
			std::wstring( L"VSMAIN" ),
			std::wstring( L"vs_4_0" ) );

	texturecolor_flat_ps =
		renderer->LoadShader( PIXEL_SHADER,
			std::wstring( L"textured.vertex-normal.flat.perspective.ps.hlsl" ),
			std::wstring( L"PSMAIN" ),
			std::wstring( L"ps_4_0" ) );

	material->Params[VT_PERSPECTIVE].pEffect = new RenderEffectDX11();
	material->Params[VT_PERSPECTIVE].bRender = true;
	ApplyShaders();
}
//--------------------------------------------------------------------------------
MaterialTemplate::~MaterialTemplate()
{
}
//--------------------------------------------------------------------------------
void MaterialTemplate::SetColorMode( ColorMode mode )
{
	color_mode = mode;
	ApplyShaders();
}
//--------------------------------------------------------------------------------
void MaterialTemplate::SetLightingMode( LightingMode mode )
{
	lighting_mode = mode;
	ApplyShaders();
}
//--------------------------------------------------------------------------------
void MaterialTemplate::SetTransparencyMode( TransparencyMode mode )
{
	if ( mode == TransparencyMode::Opaque )
	{
		material->Params[VT_PERSPECTIVE].pEffect->m_iBlendState = opaque_blendstate;
	}
	else if ( mode == TransparencyMode::Alpha )
	{
		material->Params[VT_PERSPECTIVE].pEffect->m_iBlendState = alpha_blendstate;
	}
}
//--------------------------------------------------------------------------------
void MaterialTemplate::ApplyShaders()
{
	if ( color_mode == ColorMode::ObjectColor )
	{
		if ( lighting_mode == LightingMode::Flat )
		{
			material->Params[VT_PERSPECTIVE].pEffect->SetVertexShader( objectcolor_flat_vs );
			material->Params[VT_PERSPECTIVE].pEffect->SetPixelShader( objectcolor_flat_ps );
		}
		else if ( lighting_mode == LightingMode::Point )
		{
			material->Params[VT_PERSPECTIVE].pEffect->SetVertexShader( objectcolor_pointlight_vs );
			material->Params[VT_PERSPECTIVE].pEffect->SetPixelShader( objectcolor_pointlight_ps );
		}
	}
	else if ( color_mode == ColorMode::VertexColor )
	{
		if ( lighting_mode == LightingMode::Flat )
		{
			material->Params[VT_PERSPECTIVE].pEffect->SetVertexShader( vertexcolor_flat_vs );
			material->Params[VT_PERSPECTIVE].pEffect->SetPixelShader( vertexcolor_flat_ps );
		}
		else if ( lighting_mode == LightingMode::Point )
		{
			material->Params[VT_PERSPECTIVE].pEffect->SetVertexShader( vertexcolor_pointlight_vs );
			material->Params[VT_PERSPECTIVE].pEffect->SetPixelShader( vertexcolor_pointlight_ps );
		}	
	}
	else if ( color_mode == ColorMode::TexturedColor )
	{
		if ( lighting_mode == LightingMode::Flat )
		{
			material->Params[VT_PERSPECTIVE].pEffect->SetVertexShader( texturecolor_flat_vs );
			material->Params[VT_PERSPECTIVE].pEffect->SetPixelShader( texturecolor_flat_ps );
		}
		else if ( lighting_mode == LightingMode::Point )
		{
			material->Params[VT_PERSPECTIVE].pEffect->SetVertexShader( texturecolor_pointlight_vs );
			material->Params[VT_PERSPECTIVE].pEffect->SetPixelShader( texturecolor_pointlight_ps );
		}
	}
}
//--------------------------------------------------------------------------------