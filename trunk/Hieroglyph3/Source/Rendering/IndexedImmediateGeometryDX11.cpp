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
#include "IndexedImmediateGeometryDX11.h"
#include "IndexBufferDX11.h"
#include "BufferConfigDX11.h"
#include "Log.h"
#include "GlyphString.h"
#include "PipelineManagerDX11.h"
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
IndexedImmediateGeometryDX11::IndexedImmediateGeometryDX11( )
	: m_uiMaxIndexCount( 0 ),
	  m_uiIndexCount( 0 ),
	  m_pIndexArray( nullptr )
{
	// Initialize our buffer to a reasonable size
	SetMaxIndexCount( 4096 );
}
//--------------------------------------------------------------------------------
IndexedImmediateGeometryDX11::~IndexedImmediateGeometryDX11()
{
	SAFE_DELETE_ARRAY( m_pIndexArray );
}
//--------------------------------------------------------------------------------
void IndexedImmediateGeometryDX11::Execute( PipelineManagerDX11* pPipeline, IParameterManager* pParamManager )
{
	if ( m_uiIndexCount > 0 ) {

		UploadVertexData( pPipeline );
		UploadIndexData( pPipeline );
	
		pPipeline->InputAssemblerStage.ClearDesiredState();

		// Set the Input Assembler state, then perform the draw call.
		int layout = GetInputLayout( pPipeline->ShaderStages[VERTEX_SHADER]->DesiredState.GetShaderProgram() );
		pPipeline->InputAssemblerStage.DesiredState.SetInputLayout( layout );
		pPipeline->InputAssemblerStage.DesiredState.SetPrimitiveTopology( m_ePrimType );
		pPipeline->InputAssemblerStage.DesiredState.SetVertexBuffer( 0, m_VB->m_iResource, 0, sizeof( ImmediateVertexDX11 ) );
		pPipeline->InputAssemblerStage.DesiredState.SetIndexBuffer( m_IB->m_iResource );

		pPipeline->ApplyInputResources();

		pPipeline->DrawIndexed( m_uiIndexCount, 0, 0 );
	}
}
//--------------------------------------------------------------------------------
void IndexedImmediateGeometryDX11::ResetGeometry()
{
	// Reset the vertex and index count here to prepare for the next drawing pass.

	ImmediateGeometryDX11::ResetGeometry();
	m_uiIndexCount = 0;
}
//--------------------------------------------------------------------------------
void IndexedImmediateGeometryDX11::UploadIndexData( PipelineManagerDX11* pPipeline )
{

	if ( m_uiIndexCount > 0 ) {
		
		// Map the index buffer for writing

		D3D11_MAPPED_SUBRESOURCE resource = 
			pPipeline->MapResource( m_IB, 0, D3D11_MAP_WRITE_DISCARD, 0 );

		// Only copy as much of the data as you actually have filled up
	
		memcpy( resource.pData, m_pIndexArray, m_uiIndexCount * sizeof( unsigned int ) );

		// Unmap the vertex buffer

		pPipeline->UnMapResource( m_IB, 0 );
	}
}
//--------------------------------------------------------------------------------
void IndexedImmediateGeometryDX11::AddIndex( const unsigned int index )
{
	// This method does not check the value that is passed as an index against
	// the number of vertices that are out there.  Instead it only checks that
	// we don't exceed the array of indices.  User's are allowed to do what they
	// want with this data, but have to ensure that the indices are correct!

	if ( m_uiIndexCount < m_uiMaxIndexCount ) {
		m_pIndexArray[m_uiIndexCount] = index;
		m_uiIndexCount++;
	}
}
//--------------------------------------------------------------------------------
void IndexedImmediateGeometryDX11::AddIndices( const unsigned int i1, const unsigned int i2 )
{
	AddIndex( i1 );
	AddIndex( i2 );
}
//--------------------------------------------------------------------------------
void IndexedImmediateGeometryDX11::AddIndices( const unsigned int i1, const unsigned int i2, const unsigned int i3 )
{
	AddIndex( i1 );
	AddIndex( i2 );
	AddIndex( i3 );
}
//--------------------------------------------------------------------------------
void IndexedImmediateGeometryDX11::SetMaxIndexCount( unsigned int maxIndices )
{
	// if this is different than the current size, then release the existing
	// local array, plus resize the resource.

	if ( maxIndices != m_uiMaxIndexCount ) {

		// Remember the maximum number of vertices to allow
		m_uiMaxIndexCount = maxIndices;
		m_uiIndexCount = 0;

		// Release system memory for the old array so that we can create a new one
		SAFE_DELETE_ARRAY( m_pIndexArray );
		m_pIndexArray = new unsigned int[m_uiMaxIndexCount];

		// Delete the existing resource if it already existed
		if ( nullptr != m_IB ) {
			RendererDX11::Get()->DeleteResource( m_IB );
			m_IB = nullptr;
		}

		// Create the new vertex buffer, with the dynamic flag set to true
		BufferConfigDX11 ibuffer;
		ibuffer.SetDefaultIndexBuffer( m_uiMaxIndexCount * sizeof( unsigned int ), true );
		m_IB = RendererDX11::Get()->CreateIndexBuffer( &ibuffer, nullptr );
	}
}
//--------------------------------------------------------------------------------
unsigned int IndexedImmediateGeometryDX11::GetMaxIndexCount()
{
	return( m_uiMaxIndexCount );
}
//--------------------------------------------------------------------------------
unsigned int IndexedImmediateGeometryDX11::GetIndexCount()
{
	return( m_uiIndexCount );
}
//--------------------------------------------------------------------------------
unsigned int IndexedImmediateGeometryDX11::GetPrimitiveCount()
{
	unsigned int count = 0;
	unsigned int referencedVertices = m_uiIndexCount;

	switch ( m_ePrimType )
	{
	case D3D11_PRIMITIVE_TOPOLOGY_UNDEFINED:
		break;
	case D3D11_PRIMITIVE_TOPOLOGY_POINTLIST:
		count = referencedVertices; break;
	case D3D11_PRIMITIVE_TOPOLOGY_LINELIST:
		count = referencedVertices / 2; break;
	case D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP:
		count = referencedVertices - 1; break;
	case D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST:
		count = referencedVertices / 3; break;
	case D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP:
		count = referencedVertices - 2; break;
	case D3D11_PRIMITIVE_TOPOLOGY_LINELIST_ADJ:
		count = referencedVertices / 4; break;
	case D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP_ADJ:
		count = referencedVertices - 3; break;
	case D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST_ADJ:
		count = referencedVertices / 6; break;
	case D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP_ADJ:
		count = ( referencedVertices - 3 ) / 2; break;
	case D3D11_PRIMITIVE_TOPOLOGY_1_CONTROL_POINT_PATCHLIST:
		count = referencedVertices; break;
	case D3D11_PRIMITIVE_TOPOLOGY_2_CONTROL_POINT_PATCHLIST:
		count = referencedVertices / 2; break;
	case D3D11_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST:
		count = referencedVertices / 3; break;
	case D3D11_PRIMITIVE_TOPOLOGY_4_CONTROL_POINT_PATCHLIST:
		count = referencedVertices / 4; break;
	case D3D11_PRIMITIVE_TOPOLOGY_5_CONTROL_POINT_PATCHLIST:
		count = referencedVertices / 5; break;
	case D3D11_PRIMITIVE_TOPOLOGY_6_CONTROL_POINT_PATCHLIST:
		count = referencedVertices / 6; break;
	case D3D11_PRIMITIVE_TOPOLOGY_7_CONTROL_POINT_PATCHLIST:
		count = referencedVertices / 7; break;
	case D3D11_PRIMITIVE_TOPOLOGY_8_CONTROL_POINT_PATCHLIST:
		count = referencedVertices / 8; break;
	case D3D11_PRIMITIVE_TOPOLOGY_9_CONTROL_POINT_PATCHLIST:
		count = referencedVertices / 9; break;
	case D3D11_PRIMITIVE_TOPOLOGY_10_CONTROL_POINT_PATCHLIST:
		count = referencedVertices / 10; break;
	case D3D11_PRIMITIVE_TOPOLOGY_11_CONTROL_POINT_PATCHLIST:
		count = referencedVertices / 11; break;
	case D3D11_PRIMITIVE_TOPOLOGY_12_CONTROL_POINT_PATCHLIST:
		count = referencedVertices / 12; break;
	case D3D11_PRIMITIVE_TOPOLOGY_13_CONTROL_POINT_PATCHLIST:
		count = referencedVertices / 13; break;
	case D3D11_PRIMITIVE_TOPOLOGY_14_CONTROL_POINT_PATCHLIST:
		count = referencedVertices / 14; break;
	case D3D11_PRIMITIVE_TOPOLOGY_15_CONTROL_POINT_PATCHLIST:
		count = referencedVertices / 15; break;
	case D3D11_PRIMITIVE_TOPOLOGY_16_CONTROL_POINT_PATCHLIST:
		count = referencedVertices / 16; break;
	case D3D11_PRIMITIVE_TOPOLOGY_17_CONTROL_POINT_PATCHLIST:
		count = referencedVertices / 17; break;
	case D3D11_PRIMITIVE_TOPOLOGY_18_CONTROL_POINT_PATCHLIST:
		count = referencedVertices / 18; break;
	case D3D11_PRIMITIVE_TOPOLOGY_19_CONTROL_POINT_PATCHLIST:
		count = referencedVertices / 19; break;
	case D3D11_PRIMITIVE_TOPOLOGY_20_CONTROL_POINT_PATCHLIST:
		count = referencedVertices / 20; break;
	case D3D11_PRIMITIVE_TOPOLOGY_21_CONTROL_POINT_PATCHLIST:
		count = referencedVertices / 21; break;
	case D3D11_PRIMITIVE_TOPOLOGY_22_CONTROL_POINT_PATCHLIST:
		count = referencedVertices / 22; break;
	case D3D11_PRIMITIVE_TOPOLOGY_23_CONTROL_POINT_PATCHLIST:
		count = referencedVertices / 23; break;
	case D3D11_PRIMITIVE_TOPOLOGY_24_CONTROL_POINT_PATCHLIST:
		count = referencedVertices / 24; break;
	case D3D11_PRIMITIVE_TOPOLOGY_25_CONTROL_POINT_PATCHLIST:
		count = referencedVertices / 25; break;
	case D3D11_PRIMITIVE_TOPOLOGY_26_CONTROL_POINT_PATCHLIST:
		count = referencedVertices / 26; break;
	case D3D11_PRIMITIVE_TOPOLOGY_27_CONTROL_POINT_PATCHLIST:
		count = referencedVertices / 27; break;
	case D3D11_PRIMITIVE_TOPOLOGY_28_CONTROL_POINT_PATCHLIST:
		count = referencedVertices / 28; break;
	case D3D11_PRIMITIVE_TOPOLOGY_29_CONTROL_POINT_PATCHLIST:
		count = referencedVertices / 29; break;
	case D3D11_PRIMITIVE_TOPOLOGY_30_CONTROL_POINT_PATCHLIST:
		count = referencedVertices / 30; break;
	case D3D11_PRIMITIVE_TOPOLOGY_31_CONTROL_POINT_PATCHLIST:
		count = referencedVertices / 31; break;
	case D3D11_PRIMITIVE_TOPOLOGY_32_CONTROL_POINT_PATCHLIST:
		count = referencedVertices / 32; break;
	}

	return( count );
}
//--------------------------------------------------------------------------------