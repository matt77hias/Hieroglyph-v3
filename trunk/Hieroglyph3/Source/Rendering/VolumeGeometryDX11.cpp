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
#include "VolumeGeometryDX11.h"
#include "IndexBufferDX11.h"
#include "BufferConfigDX11.h"
#include "Log.h"
#include "GlyphString.h"
#include "PipelineManagerDX11.h"
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
VolumeGeometryDX11::VolumeGeometryDX11( )
{

	D3D11_INPUT_ELEMENT_DESC e1;
	D3D11_INPUT_ELEMENT_DESC e2;

	e1.SemanticName = "POSITION";
	e1.SemanticIndex = 0;
	e1.Format = DXGI_FORMAT_R32G32B32_FLOAT;
	e1.InputSlot = 0;
	e1.AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	e1.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	e1.InstanceDataStepRate = 0;

	e2.SemanticName = "TEXCOORD";
	e2.SemanticIndex = 0;
	e2.Format = DXGI_FORMAT_R32G32B32_FLOAT;
	e2.InputSlot = 0;
	e2.AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	e2.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	e2.InstanceDataStepRate = 0;

	m_elements.add( e1 );
	m_elements.add( e2 );
}
//--------------------------------------------------------------------------------
VolumeGeometryDX11::~VolumeGeometryDX11()
{
}
//--------------------------------------------------------------------------------
void VolumeGeometryDX11::Execute( PipelineManagerDX11* pPipeline, IParameterManager* pParamManager )
{
	if ( IndexBuffer.GetIndexCount() > 0 ) {

		VertexBuffer.UploadVertexData( pPipeline );
		IndexBuffer.UploadIndexData( pPipeline );
	
		pPipeline->InputAssemblerStage.ClearDesiredState();

		// Set the Input Assembler state, then perform the draw call.
		int layout = GetInputLayout( pPipeline->ShaderStages[VERTEX_SHADER]->DesiredState.GetShaderProgram() );
		pPipeline->InputAssemblerStage.DesiredState.SetInputLayout( layout );
		pPipeline->InputAssemblerStage.DesiredState.SetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
		pPipeline->InputAssemblerStage.DesiredState.SetVertexBuffer( 0, VertexBuffer.GetVertexBuffer()->m_iResource, 0, sizeof( VolumeVertexDX11 ) );
		pPipeline->InputAssemblerStage.DesiredState.SetIndexBuffer( IndexBuffer.GetIndexBuffer()->m_iResource );

		pPipeline->ApplyInputResources();

		pPipeline->DrawIndexed( IndexBuffer.GetIndexCount(), 0, 0 );
	}
}
//--------------------------------------------------------------------------------
void VolumeGeometryDX11::ResetGeometry()
{
	// Reset the vertex and index count here to prepare for the next drawing pass.

	VertexBuffer.ResetVertices();
	IndexBuffer.ResetIndices();
}
//--------------------------------------------------------------------------------
void VolumeGeometryDX11::AddVertex( const VolumeVertexDX11& vertex )
{
	VertexBuffer.AddVertex( vertex );
}
//--------------------------------------------------------------------------------
void VolumeGeometryDX11::AddIndices( const unsigned int i1, const unsigned int i2, const unsigned int i3 )
{
	IndexBuffer.AddIndex( i1 );
	IndexBuffer.AddIndex( i2 );
	IndexBuffer.AddIndex( i3 );
}
//--------------------------------------------------------------------------------
unsigned int VolumeGeometryDX11::GetVertexCount()
{
	return( VertexBuffer.GetVertexCount() );
}
//--------------------------------------------------------------------------------
unsigned int VolumeGeometryDX11::GetIndexCount()
{
	return( IndexBuffer.GetIndexCount() );
}
//--------------------------------------------------------------------------------
unsigned int VolumeGeometryDX11::GetPrimitiveCount()
{
	return( IndexBuffer.GetIndexCount() / 3 );
}
//--------------------------------------------------------------------------------