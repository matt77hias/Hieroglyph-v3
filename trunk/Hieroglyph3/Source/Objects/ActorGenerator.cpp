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
#include "ActorGenerator.h"
#include "GeometryLoaderDX11.h"
#include "MaterialGeneratorDX11.h"
#include "ShaderResourceParameterDX11.h"
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
ActorGenerator::ActorGenerator( )
{
}
//--------------------------------------------------------------------------------
Actor* ActorGenerator::GenerateVisualizationTexture2D( RendererDX11& Renderer, 
	ResourcePtr resource, MaterialDX11* pMaterial )
{
	// Create the new actor
	Actor* pActor = new Actor();

	// The body of the actor is going to represent the frame around the 
	// visualization, while a second entity added to the actor will represent the
	// visualization canvas itself.

	Entity3D* pFrame = new Entity3D();
	pActor->GetNode()->AttachChild( pFrame );
	pActor->AddElement( pFrame );

	// Create/load the geometry to put around the visualization (i.e. the picture frame)
	pFrame->SetGeometry( GeometryLoaderDX11::loadMS3DFile2( std::wstring( L"../Data/Models/ScreenFrame.ms3d" ) ) );

	// TODO: fix this!
	pFrame->m_sParams.pGeometry->LoadToBuffers();
	
	// Create the material for the picture frame
	pFrame->SetMaterial( MaterialGeneratorDX11::GeneratePhong( Renderer ) );

	// Create/load the geometry to put the visualization on (i.e. the picture)
	pActor->GetBody()->SetGeometry( GeometryLoaderDX11::loadMS3DFile2( std::wstring( L"../Data/Models/Screen.ms3d" ) ) );
	// TODO: fix this!
	pActor->GetBody()->m_sParams.pGeometry->LoadToBuffers();

	// Use the passed in material to render the visualization.  This allows for 
	// a user to create customized visualizations for special format textures.
	pMaterial = new MaterialDX11();

	// Create and fill the effect that will be used for this view type
	RenderEffectDX11* pEffect = new RenderEffectDX11();

	pEffect->m_iVertexShader = 
		Renderer.LoadShader( VERTEX_SHADER,
		std::wstring( L"../Data/Shaders/ObjectTexturedVS.hlsl" ),
		std::wstring( L"VSMAIN" ),
		std::wstring( L"vs_5_0" ) );
	pEffect->m_iPixelShader = 
		Renderer.LoadShader( PIXEL_SHADER,
		std::wstring( L"../Data/Shaders/ObjectTexturedPS.hlsl" ),
		std::wstring( L"PSMAIN" ),
		std::wstring( L"ps_5_0" ) );

	// Enable the material to render the given view type, and set its effect.
	pMaterial->Params[VT_PERSPECTIVE].bRender = true;
	pMaterial->Params[VT_PERSPECTIVE].pEffect = pEffect;


	pActor->GetBody()->SetMaterial( pMaterial );
	pActor->GetNode()->Position() = Vector3f( 0.0f, 2.0f, 0.0f );
	pActor->GetNode()->Rotation().Rotation( Vector3f( 1.50f, -0.707f, 0.0f ) );
	pActor->GetNode()->Update( 0.0f );


	// Add a parameter to the screen that will bind the texture for visualization.
	ShaderResourceParameterDX11* pParameter = new ShaderResourceParameterDX11();
	pParameter->SetName( std::wstring( L"ColorTexture" ) );
	pParameter->SetParameterData( reinterpret_cast<void*>( &resource->m_iResourceSRV ) );

	pActor->GetBody()->AddRenderParameter( pParameter );

	D3D11_SAMPLER_DESC state; 
	state.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	state.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	state.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	state.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	state.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	state.MaxAnisotropy = 1;
	state.MaxLOD = 0;
	state.MinLOD = 0;
	state.MipLODBias = 0;
	
	int sampler = Renderer.CreateSamplerState( &state );
	Renderer.SetSamplerParameter( std::wstring( L"LinearSampler" ), &sampler ); 

	return( pActor );
}
//--------------------------------------------------------------------------------
