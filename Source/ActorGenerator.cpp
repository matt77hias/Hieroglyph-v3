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
#include "ActorGenerator.h"
#include "GeometryLoaderDX11.h"
#include "GeometryGeneratorDX11.h"
#include "MaterialGeneratorDX11.h"
#include "ShaderResourceParameterDX11.h"
#include "IParameterManager.h"
#include "ShaderResourceParameterWriterDX11.h"
#include "SamplerParameterWriterDX11.h"
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
ActorGenerator::ActorGenerator( )
{
}
//--------------------------------------------------------------------------------
Actor* ActorGenerator::GenerateVisualizationTexture2D( RendererDX11& Renderer, 
	ResourcePtr resource, MaterialPtr pMaterial )
{
	// Create the new actor
	Actor* pActor = new Actor();

	// The body of the actor is going to represent the canvas of the visualization
	// frame, while a second entity added to the actor will represent the frame
	// around the canvas.

	Entity3D* pFrame = new Entity3D();
	pActor->GetNode()->AttachChild( pFrame );
	pActor->AddElement( pFrame );

	// Create/load the geometry to put around the visualization (i.e. the picture frame)
	GeometryPtr frameGeometry = GeometryLoaderDX11::loadMS3DFile2( std::wstring( L"ScreenFrame.ms3d" ) );
	frameGeometry->LoadToBuffers();
	pFrame->Visual.SetGeometry( frameGeometry );
		
	// Create the material for the picture frame
	pFrame->Visual.SetMaterial( MaterialGeneratorDX11::GeneratePhong( Renderer ) );

	// Create/load the geometry to put the visualization on (i.e. the picture)
	GeometryPtr screenGeometry = GeometryLoaderDX11::loadMS3DFile2( std::wstring( L"Screen.ms3d" ) );
	screenGeometry->LoadToBuffers();
	pActor->GetBody()->Visual.SetGeometry( screenGeometry );

	// Use the passed in material to render the visualization.  This allows for 
	// a user to create customized visualizations for special format textures.

	if ( pMaterial == NULL ) {

		pMaterial = MaterialPtr( new MaterialDX11() );

		// Create and fill the effect that will be used for this view type
		RenderEffectDX11* pEffect = new RenderEffectDX11();

		pEffect->SetVertexShader( Renderer.LoadShader( VERTEX_SHADER,
			std::wstring( L"ObjectTexturedVS.hlsl" ),
			std::wstring( L"VSMAIN" ),
			std::wstring( L"vs_5_0" ) ) );

		pEffect->SetPixelShader( Renderer.LoadShader( PIXEL_SHADER,
			std::wstring( L"ObjectTexturedPS.hlsl" ),
			std::wstring( L"PSMAIN" ),
			std::wstring( L"ps_5_0" ) ) );

		// Enable the material to render the given view type, and set its effect.
		pMaterial->Params[VT_PERSPECTIVE].bRender = true;
		pMaterial->Params[VT_PERSPECTIVE].pEffect = pEffect;
	}

	pActor->GetBody()->Visual.SetMaterial( pMaterial );
	pActor->GetNode()->Transform.Position() = Vector3f( 0.0f, 2.0f, 0.0f );
	pActor->GetNode()->Update( 0.0f );


	// Add a parameter to the screen that will bind the texture for visualization.
	pActor->GetBody()->Parameters.SetShaderResourceParameter( L"ColorTexture", resource );


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
    pActor->GetBody()->Parameters.SetSamplerParameter( L"LinearSampler", sampler );


	return( pActor );
}
//--------------------------------------------------------------------------------
