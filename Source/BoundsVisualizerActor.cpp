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
#include "BoundsVisualizerActor.h"
#include "Log.h"
#include "MaterialGeneratorDX11.h"
#include "RasterizerStateConfigDX11.h"
#include "DepthStencilStateConfigDX11.h"
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
BoundsVisualizerActor::BoundsVisualizerActor()
{
	// Generate the visualizer geometry to display what we put into it.

	m_pGeometry = VisualizerGeometryPtr( new DrawIndexedInstancedExecutorDX11<VisualizerVertexDX11::VertexData, VisualizerVertexDX11::InstanceData>() );
	m_pGeometry->SetPrimitiveType( D3D11_PRIMITIVE_TOPOLOGY_LINELIST );
	m_pGeometry->SetLayoutElements( VisualizerVertexDX11::GetElementCount(), VisualizerVertexDX11::Elements );

	VisualizerVertexDX11::VertexData v;
	
	// Generate the vertices to represent a unit sphere.  We are making two
	// bands of vertices, which are essentially circles on in the X=0 and 
	// primary axes.

	Sphere3f sphere( Vector3f( 0.0f, 0.0f, 0.0f ), 1.0f );

	unsigned int baseVertex = 0;

	// Band in the Z = 0 plane
	for ( int i = 0; i < 16; i++ )
	{
		float theta = 0.0f;
		float phi = static_cast<float>( i ) * ( 2.0f * GLYPH_PI * 0.0625f ) - ( GLYPH_PI );
		
		sphere.SamplePosition( v.position, theta, phi );
		m_pGeometry->AddVertex( v );
		m_pGeometry->AddIndices( baseVertex + i, baseVertex + ((i+1)%16) );
	}

	baseVertex = m_pGeometry->GetVertexCount();

	// Band in the Y = 0 plane
	for ( int i = 0; i < 16; i++ )
	{
		float theta = static_cast<float>( i ) * ( 2.0f * GLYPH_PI * 0.0625f );
		float phi = GLYPH_PI * 0.5f;
		
		sphere.SamplePosition( v.position, theta, phi );
		m_pGeometry->AddVertex( v );
		m_pGeometry->AddIndices( baseVertex + i, baseVertex + ((i+1)%16) );
	}

	baseVertex = m_pGeometry->GetVertexCount();

	// Band in the X = 0 plane
	for ( int i = 0; i < 16; i++ )
	{
		float theta = GLYPH_PI * 0.5f;
		float phi = static_cast<float>( i ) * ( 2.0f * GLYPH_PI * 0.0625f ) - ( GLYPH_PI );
		
		sphere.SamplePosition( v.position, theta, phi );
		m_pGeometry->AddVertex( v );
		m_pGeometry->AddIndices( baseVertex + i, baseVertex + ((i+1)%16) );
	}



	//// [-X,+Y,-Z] Offset 0
	//v.position = Vector3f( -1.0f, 1.0f, -1.0f );
	//m_pGeometry->AddVertex( v );

	//// [+X,+Y,-Z] Offset 1
	//v.position = Vector3f( 1.0f, 1.0f, -1.0f );
	//m_pGeometry->AddVertex( v );

	//// [+X,-Y,-Z] Offset 2
	//v.position = Vector3f( 1.0f, -1.0f, -1.0f );
	//m_pGeometry->AddVertex( v );

	//// [-X,-Y,-Z] Offset 3
	//v.position = Vector3f( -1.0f, -1.0f, -1.0f );
	//m_pGeometry->AddVertex( v );

	//// [-X,+Y,-Z] Offset 4
	//v.position = Vector3f( -1.0f, 1.0f, 1.0f );
	//m_pGeometry->AddVertex( v );

	//// [+X,+Y,-Z] Offset 5
	//v.position = Vector3f( 1.0f, 1.0f, 1.0f );
	//m_pGeometry->AddVertex( v );

	//// [+X,-Y,-Z] Offset 6
	//v.position = Vector3f( 1.0f, -1.0f, 1.0f );
	//m_pGeometry->AddVertex( v );

	//// [-X,-Y,-Z] Offset 7
	//v.position = Vector3f( -1.0f, -1.0f, 1.0f );
	//m_pGeometry->AddVertex( v );

	//// -Z Face
	//m_pGeometry->AddIndices( 0, 1, 2 );
	//m_pGeometry->AddIndices( 0, 2, 3 );

	//// +Z Face
	//m_pGeometry->AddIndices( 5, 4, 7 );
	//m_pGeometry->AddIndices( 5, 7, 6 );

	//// -X Face
	//m_pGeometry->AddIndices( 4, 0, 3 );
	//m_pGeometry->AddIndices( 4, 3, 7 );

	//// +X Face
	//m_pGeometry->AddIndices( 1, 5, 6 );
	//m_pGeometry->AddIndices( 1, 6, 2 );

	//// -Y Face
	//m_pGeometry->AddIndices( 3, 2, 6 );
	//m_pGeometry->AddIndices( 3, 6, 7 );

	//// +Y Face
	//m_pGeometry->AddIndices( 5, 1, 0 );
	//m_pGeometry->AddIndices( 5, 0, 4 );

	GetBody()->Visual.SetGeometry( m_pGeometry );
	
	// If no shapes are added to the entity, then it will not be pickable.



	// Create the material that will be returned
	MaterialPtr pMaterial = MaterialPtr( new MaterialDX11() );

	// Create and fill the effect that will be used for this view type
	RenderEffectDX11* pEffect = new RenderEffectDX11();

	RendererDX11* pRenderer = RendererDX11::Get();

	pEffect->SetVertexShader( pRenderer->LoadShader( VERTEX_SHADER,
		std::wstring( L"VertexInstanceColor.hlsl" ),
		std::wstring( L"VSMAIN" ),
		std::wstring( L"vs_4_0" ) ) );

	pEffect->SetPixelShader( pRenderer->LoadShader( PIXEL_SHADER,
		std::wstring( L"VertexInstanceColor.hlsl" ),
		std::wstring( L"PSMAIN" ),
		std::wstring( L"ps_4_0" ) ) );

	RasterizerStateConfigDX11 rs;
	//rs.CullMode = D3D11_CULL_NONE;
	rs.FillMode = D3D11_FILL_WIREFRAME;
	pEffect->m_iRasterizerState = pRenderer->CreateRasterizerState( &rs );

	DepthStencilStateConfigDX11 ds;
	ds.DepthEnable = false;
	pEffect->m_iDepthStencilState = pRenderer->CreateDepthStencilState( &ds );

	// Enable the material to render the given view type, and set its effect.
	pMaterial->Params[VT_PERSPECTIVE].bRender = true;
	pMaterial->Params[VT_PERSPECTIVE].pEffect = pEffect;

	GetBody()->Visual.SetMaterial( pMaterial );
}
//--------------------------------------------------------------------------------
BoundsVisualizerActor::~BoundsVisualizerActor()
{
}
//--------------------------------------------------------------------------------
void BoundsVisualizerActor::UpdateBoundsData( std::vector<Entity3D*>& entities )
{
	// We start out by resetting only the instance data.  The vertex and index 
	// data will remain how it is and will be repeated for each instance.
	
	m_pGeometry->ResetInstances();

	// Loop through all of the entities in the provided list and produce an 
	// instance for each of them.  This will allow for one instance per entity.

	VisualizerVertexDX11::InstanceData i;

	for ( auto pEntity : entities )
	{
		if ( pEntity->Shape.GetNumberOfShapes() > 0 )
		{
			i.transform = pEntity->Transform.WorldMatrix();
			i.color = Vector4f( 1.0f, 0.0f, 0.0f, 1.0f );
			m_pGeometry->AddInstance( i );
		}
	}
}
//--------------------------------------------------------------------------------