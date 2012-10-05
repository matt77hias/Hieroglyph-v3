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
{

}
//--------------------------------------------------------------------------------
IndexedImmediateGeometryDX11::~IndexedImmediateGeometryDX11()
{
}
//--------------------------------------------------------------------------------
void IndexedImmediateGeometryDX11::Execute( PipelineManagerDX11* pPipeline, IParameterManager* pParamManager )
{
	if ( IndexBuffer.GetIndexCount() > 0 ) {

		VertexBuffer.UploadVertexData( pPipeline );
		IndexBuffer.UploadIndexData( pPipeline );
	
		pPipeline->InputAssemblerStage.ClearDesiredState();

		// Set the Input Assembler state, then perform the draw call.
		int layout = GetInputLayout( pPipeline->ShaderStages[VERTEX_SHADER]->DesiredState.GetShaderProgram() );
		pPipeline->InputAssemblerStage.DesiredState.SetInputLayout( layout );
		pPipeline->InputAssemblerStage.DesiredState.SetPrimitiveTopology( m_ePrimType );
		pPipeline->InputAssemblerStage.DesiredState.SetVertexBuffer( 0, VertexBuffer.GetVertexBuffer()->m_iResource, 0, sizeof( ImmediateVertexDX11 ) );
		pPipeline->InputAssemblerStage.DesiredState.SetIndexBuffer( IndexBuffer.GetIndexBuffer()->m_iResource );

		pPipeline->ApplyInputResources();

		pPipeline->DrawIndexed( IndexBuffer.GetIndexCount(), 0, 0 );
	}
}
//--------------------------------------------------------------------------------
void IndexedImmediateGeometryDX11::ResetGeometry()
{
	// Reset the vertex and index count here to prepare for the next drawing pass.

	VertexBuffer.ResetVertices();
	IndexBuffer.ResetIndices();
}
//--------------------------------------------------------------------------------
void IndexedImmediateGeometryDX11::AddIndex( const unsigned int index )
{
	// This method does not check the value that is passed as an index against
	// the number of vertices that are out there. User's are allowed to do what they
	// want with this data, but have to ensure that the indices are correct!

	IndexBuffer.AddIndex( index );
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
unsigned int IndexedImmediateGeometryDX11::GetIndexCount()
{
	return( IndexBuffer.GetIndexCount() );
}
//--------------------------------------------------------------------------------
unsigned int IndexedImmediateGeometryDX11::GetPrimitiveCount()
{
	unsigned int count = 0;
	unsigned int referencedIndices = IndexBuffer.GetIndexCount();

	switch ( m_ePrimType )
	{
	case D3D11_PRIMITIVE_TOPOLOGY_UNDEFINED:
		break;
	case D3D11_PRIMITIVE_TOPOLOGY_POINTLIST:
		count = referencedIndices; break;
	case D3D11_PRIMITIVE_TOPOLOGY_LINELIST:
		count = referencedIndices / 2; break;
	case D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP:
		count = referencedIndices - 1; break;
	case D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST:
		count = referencedIndices / 3; break;
	case D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP:
		count = referencedIndices - 2; break;
	case D3D11_PRIMITIVE_TOPOLOGY_LINELIST_ADJ:
		count = referencedIndices / 4; break;
	case D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP_ADJ:
		count = referencedIndices - 3; break;
	case D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST_ADJ:
		count = referencedIndices / 6; break;
	case D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP_ADJ:
		count = ( referencedIndices - 3 ) / 2; break;
	case D3D11_PRIMITIVE_TOPOLOGY_1_CONTROL_POINT_PATCHLIST:
		count = referencedIndices; break;
	case D3D11_PRIMITIVE_TOPOLOGY_2_CONTROL_POINT_PATCHLIST:
		count = referencedIndices / 2; break;
	case D3D11_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST:
		count = referencedIndices / 3; break;
	case D3D11_PRIMITIVE_TOPOLOGY_4_CONTROL_POINT_PATCHLIST:
		count = referencedIndices / 4; break;
	case D3D11_PRIMITIVE_TOPOLOGY_5_CONTROL_POINT_PATCHLIST:
		count = referencedIndices / 5; break;
	case D3D11_PRIMITIVE_TOPOLOGY_6_CONTROL_POINT_PATCHLIST:
		count = referencedIndices / 6; break;
	case D3D11_PRIMITIVE_TOPOLOGY_7_CONTROL_POINT_PATCHLIST:
		count = referencedIndices / 7; break;
	case D3D11_PRIMITIVE_TOPOLOGY_8_CONTROL_POINT_PATCHLIST:
		count = referencedIndices / 8; break;
	case D3D11_PRIMITIVE_TOPOLOGY_9_CONTROL_POINT_PATCHLIST:
		count = referencedIndices / 9; break;
	case D3D11_PRIMITIVE_TOPOLOGY_10_CONTROL_POINT_PATCHLIST:
		count = referencedIndices / 10; break;
	case D3D11_PRIMITIVE_TOPOLOGY_11_CONTROL_POINT_PATCHLIST:
		count = referencedIndices / 11; break;
	case D3D11_PRIMITIVE_TOPOLOGY_12_CONTROL_POINT_PATCHLIST:
		count = referencedIndices / 12; break;
	case D3D11_PRIMITIVE_TOPOLOGY_13_CONTROL_POINT_PATCHLIST:
		count = referencedIndices / 13; break;
	case D3D11_PRIMITIVE_TOPOLOGY_14_CONTROL_POINT_PATCHLIST:
		count = referencedIndices / 14; break;
	case D3D11_PRIMITIVE_TOPOLOGY_15_CONTROL_POINT_PATCHLIST:
		count = referencedIndices / 15; break;
	case D3D11_PRIMITIVE_TOPOLOGY_16_CONTROL_POINT_PATCHLIST:
		count = referencedIndices / 16; break;
	case D3D11_PRIMITIVE_TOPOLOGY_17_CONTROL_POINT_PATCHLIST:
		count = referencedIndices / 17; break;
	case D3D11_PRIMITIVE_TOPOLOGY_18_CONTROL_POINT_PATCHLIST:
		count = referencedIndices / 18; break;
	case D3D11_PRIMITIVE_TOPOLOGY_19_CONTROL_POINT_PATCHLIST:
		count = referencedIndices / 19; break;
	case D3D11_PRIMITIVE_TOPOLOGY_20_CONTROL_POINT_PATCHLIST:
		count = referencedIndices / 20; break;
	case D3D11_PRIMITIVE_TOPOLOGY_21_CONTROL_POINT_PATCHLIST:
		count = referencedIndices / 21; break;
	case D3D11_PRIMITIVE_TOPOLOGY_22_CONTROL_POINT_PATCHLIST:
		count = referencedIndices / 22; break;
	case D3D11_PRIMITIVE_TOPOLOGY_23_CONTROL_POINT_PATCHLIST:
		count = referencedIndices / 23; break;
	case D3D11_PRIMITIVE_TOPOLOGY_24_CONTROL_POINT_PATCHLIST:
		count = referencedIndices / 24; break;
	case D3D11_PRIMITIVE_TOPOLOGY_25_CONTROL_POINT_PATCHLIST:
		count = referencedIndices / 25; break;
	case D3D11_PRIMITIVE_TOPOLOGY_26_CONTROL_POINT_PATCHLIST:
		count = referencedIndices / 26; break;
	case D3D11_PRIMITIVE_TOPOLOGY_27_CONTROL_POINT_PATCHLIST:
		count = referencedIndices / 27; break;
	case D3D11_PRIMITIVE_TOPOLOGY_28_CONTROL_POINT_PATCHLIST:
		count = referencedIndices / 28; break;
	case D3D11_PRIMITIVE_TOPOLOGY_29_CONTROL_POINT_PATCHLIST:
		count = referencedIndices / 29; break;
	case D3D11_PRIMITIVE_TOPOLOGY_30_CONTROL_POINT_PATCHLIST:
		count = referencedIndices / 30; break;
	case D3D11_PRIMITIVE_TOPOLOGY_31_CONTROL_POINT_PATCHLIST:
		count = referencedIndices / 31; break;
	case D3D11_PRIMITIVE_TOPOLOGY_32_CONTROL_POINT_PATCHLIST:
		count = referencedIndices / 32; break;
	}

	return( count );
}
//--------------------------------------------------------------------------------