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
#include "VolumeActor.h"
#include "Log.h"
#include "Texture3dConfigDX11.h"
#include "MaterialGeneratorDX11.h"
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
VolumeActor::VolumeActor()
{
	// Create the texture that we will be using as the volume.

	float fData[32][32][32];

	for ( int z = 0; z < 32; z++ ) {
		for ( int y = 0; y < 32; y++ ) {
			for ( int x = 0; x < 32; x++ ) {
				if ( z > 15 && y > 15 && x > 15 ) {
					fData[z][y][x] = 1.0f;
				} else {
					fData[z][y][x] = 0.0f;
				}
			}
		}
	}

	D3D11_SUBRESOURCE_DATA data;
	data.pSysMem = fData;
	data.SysMemPitch = sizeof( float ) * 32;
	data.SysMemSlicePitch = sizeof( float ) * 32 * 32;


	Texture3dConfigDX11 config;
	config.SetFormat( DXGI_FORMAT_R32_FLOAT );
	config.SetBindFlags( D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS );
	config.SetWidth( 32 );
	config.SetHeight( 32 );
	config.SetDepth( 32 );
	
	m_VolumeTexture = RendererDX11::Get()->CreateTexture3D( &config, &data );

	
	// Generate the volume geometry to display what is in the volume texture.

	m_pGeometry = VolumeGeometryPtr( new VolumeGeometryDX11() );

	VolumeVertexDX11 v;
	
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

	GetBody()->SetGeometry( m_pGeometry );
	GetBody()->SetMaterial( MaterialGeneratorDX11::GenerateVolumeGeometryMaterial( *RendererDX11::Get() ) );
	GetBody()->Parameters.SetShaderResourceParameter( L"VolumeTexture", m_VolumeTexture );

	Matrix4f ObjectToTextureSpaceMatrix = 
		Matrix4f::TranslationMatrix( 1.0f, -1.0f, 1.0f ) * Matrix4f::ScaleMatrixXYZ( 0.5f, -0.5f, 0.5f );


	TextureSpaceCameraPositionWriter* pCameraTSPositionWriter = new TextureSpaceCameraPositionWriter();
	pCameraTSPositionWriter->SetObjectToTextureSpaceXform( ObjectToTextureSpaceMatrix );
	GetBody()->Parameters.AddRenderParameter( pCameraTSPositionWriter );
}
//--------------------------------------------------------------------------------
VolumeActor::~VolumeActor()
{
}
//--------------------------------------------------------------------------------
