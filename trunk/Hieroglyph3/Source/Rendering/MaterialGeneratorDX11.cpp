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
#include "MaterialGeneratorDX11.h"
#include "RasterizerStateConfigDX11.h"
#include "DepthStencilStateConfigDX11.h"
#include "BlendStateConfigDX11.h"
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
MaterialGeneratorDX11::MaterialGeneratorDX11( )
{
}
//--------------------------------------------------------------------------------
MaterialDX11* MaterialGeneratorDX11::GenerateWireFrame( RendererDX11& Renderer )
{
	// Create the material that will be returned
	MaterialDX11* pMaterial = new MaterialDX11();

	// Create and fill the effect that will be used for this view type
	RenderEffectDX11* pEffect = new RenderEffectDX11();

	pEffect->m_iVertexShader = 
		Renderer.LoadShader( VERTEX_SHADER,
		std::wstring( L"../Data/Shaders/BasicTessellation.hlsl" ),
		std::wstring( L"VSMAIN" ),
		std::wstring( L"vs_5_0" ) );
	pEffect->m_iHullShader =
		Renderer.LoadShader( HULL_SHADER,
		std::wstring( L"../Data/Shaders/BasicTessellation.hlsl" ),
		std::wstring( L"HSMAIN" ),
		std::wstring( L"hs_5_0" ) );
	pEffect->m_iDomainShader =
		Renderer.LoadShader( DOMAIN_SHADER,
		std::wstring( L"../Data/Shaders/BasicTessellation.hlsl" ),
		std::wstring( L"DSMAIN" ),
		std::wstring( L"ds_5_0" ) );
	pEffect->m_iPixelShader = 
		Renderer.LoadShader( PIXEL_SHADER,
		std::wstring( L"../Data/Shaders/BasicTessellation.hlsl" ),
		std::wstring( L"PSMAIN" ),
		std::wstring( L"ps_5_0" ) );

	RasterizerStateConfigDX11 RS;
	RS.FillMode = D3D11_FILL_WIREFRAME;

	pEffect->m_iRasterizerState = 
		Renderer.CreateRasterizerState( &RS );

	// Enable the material to render the given view type, and set its effect.
	pMaterial->Params[VT_PERSPECTIVE].bRender = true;
	pMaterial->Params[VT_PERSPECTIVE].pEffect = pEffect;

	return( pMaterial );
}
//--------------------------------------------------------------------------------