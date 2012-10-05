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
#include "InstancedQuadGeometryDX11.h"
#include "VertexBufferDX11.h"
#include "BufferConfigDX11.h"
#include "Log.h"
#include "GlyphString.h"
#include "PipelineManagerDX11.h"
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
InstancedQuadGeometryDX11::InstancedQuadGeometryDX11( )
{
	// Initialize our buffer to a reasonable size
	QuadBuffer.SetMaxVertexCount( 128 );


	// Fill in the vertex element descriptions based on each element of our format
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

	for ( int i = 0; i < sizeof(elementsArray) / sizeof(D3D11_INPUT_ELEMENT_DESC); i++ ) {
		m_elements.add( elementsArray[i] );
	}


	// Create the vertex buffer.  This buffer provides the basic corner vertices
	// of the quad, and their initial texture coordinates.  These values are
	// repeated in each of the instances, and are then modified in the vertex shader
	// by the instance data and constant buffer data.

	SpriteVertex verts[] =
	{
		{ Vector2f(0.0f, 0.0f), Vector2f(0.0f, 0.0f) },
		{ Vector2f(1.0f, 0.0f), Vector2f(1.0f, 0.0f) },
		{ Vector2f(1.0f, 1.0f), Vector2f(1.0f, 1.0f) },
		{ Vector2f(0.0f, 1.0f), Vector2f(0.0f, 1.0f) }
	};

	D3D11_SUBRESOURCE_DATA initData;
	initData.pSysMem = verts;
	initData.SysMemPitch = 0;
	initData.SysMemSlicePitch = 0;

	// Specify an immutable buffer that will only hold the initial data throughout
	// its lifetime.
	BufferConfigDX11 vbConfig;
	vbConfig.SetDefaultVertexBuffer( 4 * sizeof( SpriteVertex ), false );

	m_pVertexBuffer = RendererDX11::Get()->CreateVertexBuffer( &vbConfig, &initData );

	if ( m_pVertexBuffer->m_iResource == -1 ) {
		Log::Get().Write( L"Failed to create sprite vertex buffer" );
	}


	// Create the index buffer.  This buffer defines a quad with two triangles,
	// which is the basic unit used during the instanced drawing.  Each instance
	// will consist of two primitives, which produces six total indices needed.

	UINT indices[] = { 0, 1, 2, 3, 0, 2 };

	initData.pSysMem = indices;
	initData.SysMemPitch = 0;
	initData.SysMemSlicePitch = 0;

	// Specify an immutable buffer that will only hold the initial data throughout
	// its lifetime.
	BufferConfigDX11 ibConfig;
	ibConfig.SetDefaultIndexBuffer( 6 * sizeof(UINT), false );

	m_pIndexBuffer = RendererDX11::Get()->CreateIndexBuffer( &ibConfig, &initData );

	if ( m_pIndexBuffer->m_iResource == -1 ) {
		Log::Get().Write( L"Failed to create sprite index buffer" );
	}

}
//--------------------------------------------------------------------------------
InstancedQuadGeometryDX11::~InstancedQuadGeometryDX11()
{
}
//--------------------------------------------------------------------------------
void InstancedQuadGeometryDX11::Execute( PipelineManagerDX11* pPipeline, IParameterManager* pParamManager )
{
	if ( QuadBuffer.GetVertexCount() > 0 ) {

		QuadBuffer.UploadVertexData( pPipeline );
	
		pPipeline->InputAssemblerStage.ClearDesiredState();

		// Set the Input Assembler state, then perform the draw call.
		int layout = GetInputLayout( pPipeline->ShaderStages[VERTEX_SHADER]->DesiredState.GetShaderProgram() );
		pPipeline->InputAssemblerStage.DesiredState.SetInputLayout( layout );
		pPipeline->InputAssemblerStage.DesiredState.SetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );

		pPipeline->InputAssemblerStage.DesiredState.SetVertexBuffer( 0, m_pVertexBuffer->m_iResource, 0, sizeof( SpriteVertex ) );
		pPipeline->InputAssemblerStage.DesiredState.SetVertexBuffer( 1, QuadBuffer.GetVertexBuffer()->m_iResource, 0, sizeof( SpriteDrawData ) );
		pPipeline->InputAssemblerStage.DesiredState.SetIndexBuffer( m_pIndexBuffer->m_iResource );
		pPipeline->InputAssemblerStage.DesiredState.SetIndexBufferFormat( DXGI_FORMAT_R32_UINT );
	
		pPipeline->ApplyInputResources();

		pPipeline->DrawIndexedInstanced( 6, QuadBuffer.GetVertexCount(), 0, 0, 0 );
	}
}
//--------------------------------------------------------------------------------
void InstancedQuadGeometryDX11::ResetGeometry()
{
	// Reset the vertex count here to prepare for the next drawing pass.
	QuadBuffer.ResetVertices();
}
//--------------------------------------------------------------------------------
void InstancedQuadGeometryDX11::AddQuad( const SpriteDrawData& data )
{
	QuadBuffer.AddVertex( data );
}
//--------------------------------------------------------------------------------
unsigned int InstancedQuadGeometryDX11::GetQuadCount()
{
	return( QuadBuffer.GetVertexCount() );
}
//--------------------------------------------------------------------------------
