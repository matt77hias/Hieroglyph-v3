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
#include "GeometryActor.h"
#include "MaterialGeneratorDX11.h"
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
GeometryActor::GeometryActor()
{
	RendererDX11* pRenderer = RendererDX11::Get();

	m_pGeometry = IndexedImmediateGeometryPtr( new IndexedImmediateGeometryDX11() );
	m_pGeometry->SetColor( Vector4f( 1.0f, 1.0f, 1.0f, 1.0f ) );
	m_pGeometry->SetPrimitiveType( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
	
	GetBody()->SetGeometry( m_pGeometry );
	//GetBody()->SetMaterial( MaterialGeneratorDX11::GenerateImmediateGeometrySolidMaterial( *pRenderer ) );
	GetBody()->SetMaterial( MaterialGeneratorDX11::GenerateImmediateGeometryTexturedMaterial( *pRenderer ) );
}
//--------------------------------------------------------------------------------
GeometryActor::~GeometryActor()
{
}
//--------------------------------------------------------------------------------
void GeometryActor::ResetGeometry()
{
	m_pGeometry->ResetGeometry();
}
//--------------------------------------------------------------------------------
void GeometryActor::SetColor( Vector4f& color )
{
	m_pGeometry->SetColor( color );
}
//--------------------------------------------------------------------------------
void GeometryActor::DrawSphere( Vector3f& center, float radius, unsigned int stacks, unsigned int slices )
{
	if ( stacks < 2 ) stacks = 2;
	if ( slices < 4 ) slices = 4;

	float sliceStep = static_cast<float>( 2.0 * GLYPH_PI ) / static_cast<float>( slices );
	float stackStep = static_cast<float>( GLYPH_PI ) / static_cast<float>( stacks );

	float uStep = 4.0f / static_cast<float>( slices );
	float vStep = 2.0f / static_cast<float>( stacks );

	// Generate all of the vertices according to the specified input parameters.

	for ( unsigned int stack = 0; stack <= stacks; stack++ ) {
		for ( unsigned int slice = 0; slice <= slices; slice++ ) {

			float theta = sliceStep * static_cast<float>( slice );
			float phi = stackStep * static_cast<float>( stack );

			Vector3f position;

			position.x = radius * sinf( phi ) * cosf( theta );
			position.y = radius * cosf( phi );
			position.z = radius * sinf( phi ) * sinf( theta );

			Vector3f normal = position;
			normal.Normalize();

			Vector2f texcoords;
			texcoords.x = uStep * static_cast<float>( slice );
			texcoords.y = vStep * static_cast<float>( stack );

			m_pGeometry->AddVertex( position + center, normal, texcoords );
		}
	}

	// Generate all of the indices according to the specified input parameters.

	for ( unsigned int z = 0; z < stacks; z++ ) {
		for ( unsigned int x = 0; x < slices; x++ ) {

			m_pGeometry->AddIndex( (z+0)*(slices+1) + x+0 );
			m_pGeometry->AddIndex( (z+0)*(slices+1) + x+1 );
			m_pGeometry->AddIndex( (z+1)*(slices+1) + x+0 );

			m_pGeometry->AddIndex( (z+0)*(slices+1) + x+1 );
			m_pGeometry->AddIndex( (z+1)*(slices+1) + x+1 );
			m_pGeometry->AddIndex( (z+1)*(slices+1) + x+0 );

			//m_pGeometry->AddIndex( (z*slices + x) );
			//m_pGeometry->AddIndex( (z*slices + x) + 1 );
			//m_pGeometry->AddIndex( (z*slices + x) + slices );

			//m_pGeometry->AddIndex( (z*slices + x) + 1 );
			//m_pGeometry->AddIndex( (z*slices + x) + slices + 1 );
			//m_pGeometry->AddIndex( (z*slices + x) + slices );
		}
	}

}
//--------------------------------------------------------------------------------