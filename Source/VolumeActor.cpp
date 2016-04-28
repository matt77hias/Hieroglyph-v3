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
#include "VolumeActor.h"
#include "Log.h"
#include "Texture3dConfigDX11.h"
#include "MaterialGeneratorDX11.h"
#include "PerlinNoise.h"
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
VolumeActor::VolumeActor()
{
	// Create the texture that we will be using as the volume.

	PerlinNoise noise;
	noise.initialize();

	const int DIM = 32;

	float* fData = new float[DIM*DIM*DIM];

	for ( int z = 0; z < DIM; z++ ) {
		for ( int y = 0; y < DIM; y++ ) {
			for ( int x = 0; x < DIM; x++ ) {
				fData[z*DIM*DIM+y*DIM+x] = noise.noise3( (float)x * 0.25f, (float)y * 0.25f, (float)z * 0.25f );
			}
		}
	}

	D3D11_SUBRESOURCE_DATA data;
	data.pSysMem = fData;
	data.SysMemPitch = sizeof( float ) * DIM;
	data.SysMemSlicePitch = sizeof( float ) * DIM * DIM;


	Texture3dConfigDX11 config;
	config.SetFormat( DXGI_FORMAT_R32_FLOAT );
	config.SetBindFlags( D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS );
	config.SetWidth( DIM );
	config.SetHeight( DIM );
	config.SetDepth( DIM );
	
	m_VolumeTexture = RendererDX11::Get()->CreateTexture3D( &config, &data );

	delete [] fData;
	
	// Generate the volume geometry to display what is in the volume texture.

	m_pGeometry = VolumeGeometryPtr( new DrawIndexedExecutorDX11<VolumeTextureVertexDX11::Vertex>() );
	m_pGeometry->SetPrimitiveType( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
	m_pGeometry->SetLayoutElements( VolumeTextureVertexDX11::GetElementCount(), VolumeTextureVertexDX11::Elements );

	VolumeTextureVertexDX11::Vertex v;
	
	// [-X,+Y,-Z] Offset 0
	v.position = Vector3f( -1.0f, 1.0f, -1.0f );
	v.texcoords = Vector3f( 0.0f, 0.0f, 0.0f );
	m_pGeometry->AddVertex( v );

	// [+X,+Y,-Z] Offset 1
	v.position = Vector3f( 1.0f, 1.0f, -1.0f );
	v.texcoords = Vector3f( 1.0f, 0.0f, 0.0f );
	m_pGeometry->AddVertex( v );

	// [+X,-Y,-Z] Offset 2
	v.position = Vector3f( 1.0f, -1.0f, -1.0f );
	v.texcoords = Vector3f( 1.0f, 1.0f, 0.0f );
	m_pGeometry->AddVertex( v );

	// [-X,-Y,-Z] Offset 3
	v.position = Vector3f( -1.0f, -1.0f, -1.0f );
	v.texcoords = Vector3f( 0.0f, 1.0f, 0.0f );
	m_pGeometry->AddVertex( v );

	// [-X,+Y,-Z] Offset 4
	v.position = Vector3f( -1.0f, 1.0f, 1.0f );
	v.texcoords = Vector3f( 0.0f, 0.0f, 1.0f );
	m_pGeometry->AddVertex( v );

	// [+X,+Y,-Z] Offset 5
	v.position = Vector3f( 1.0f, 1.0f, 1.0f );
	v.texcoords = Vector3f( 1.0f, 0.0f, 1.0f );
	m_pGeometry->AddVertex( v );

	// [+X,-Y,-Z] Offset 6
	v.position = Vector3f( 1.0f, -1.0f, 1.0f );
	v.texcoords = Vector3f( 1.0f, 1.0f, 1.0f );
	m_pGeometry->AddVertex( v );

	// [-X,-Y,-Z] Offset 7
	v.position = Vector3f( -1.0f, -1.0f, 1.0f );
	v.texcoords = Vector3f( 0.0f, 1.0f, 1.0f );
	m_pGeometry->AddVertex( v );

	// -Z Face
	m_pGeometry->AddIndices( 0, 1, 2 );
	m_pGeometry->AddIndices( 0, 2, 3 );

	// +Z Face
	m_pGeometry->AddIndices( 5, 4, 7 );
	m_pGeometry->AddIndices( 5, 7, 6 );

	// -X Face
	m_pGeometry->AddIndices( 4, 0, 3 );
	m_pGeometry->AddIndices( 4, 3, 7 );

	// +X Face
	m_pGeometry->AddIndices( 1, 5, 6 );
	m_pGeometry->AddIndices( 1, 6, 2 );

	// -Y Face
	m_pGeometry->AddIndices( 3, 2, 6 );
	m_pGeometry->AddIndices( 3, 6, 7 );

	// +Y Face
	m_pGeometry->AddIndices( 5, 1, 0 );
	m_pGeometry->AddIndices( 5, 0, 4 );

	GetBody()->Visual.SetGeometry( m_pGeometry );
	GetBody()->Visual.SetMaterial( MaterialGeneratorDX11::GenerateVolumeGeometryMaterial( *RendererDX11::Get() ) );
	GetBody()->Parameters.SetShaderResourceParameter( L"VolumeTexture", m_VolumeTexture );

	Matrix4f ObjectToTextureSpaceMatrix = 
		Matrix4f::TranslationMatrix( 1.0f, -1.0f, 1.0f ) * Matrix4f::ScaleMatrixXYZ( 0.5f, -0.5f, 0.5f );


	TextureSpaceCameraPositionWriter* pCameraTSPositionWriter = new TextureSpaceCameraPositionWriter();
	pCameraTSPositionWriter->SetObjectToTextureSpaceXform( ObjectToTextureSpaceMatrix );
	GetBody()->Parameters.AddRenderParameter( pCameraTSPositionWriter );

	TextureSpaceLightPositionWriter* pLightTSPositionWriter = new TextureSpaceLightPositionWriter();
	pLightTSPositionWriter->SetObjectToTextureSpaceXform( ObjectToTextureSpaceMatrix );
	GetBody()->Parameters.AddRenderParameter( pLightTSPositionWriter );
}
//--------------------------------------------------------------------------------
VolumeActor::~VolumeActor()
{
}
//--------------------------------------------------------------------------------
