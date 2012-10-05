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
#include "ImmediateGeometryDX11.h"
#include "VertexBufferDX11.h"
#include "BufferConfigDX11.h"
#include "Log.h"
#include "GlyphString.h"
#include "PipelineManagerDX11.h"
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
ImmediateGeometryDX11::ImmediateGeometryDX11( )
{
	m_Color = Vector4f( 0.0f, 1.0f, 0.0f, 1.0f );
	m_ePrimType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	// Fill in the vertex element descriptions based on each element of our format

	D3D11_INPUT_ELEMENT_DESC e1;
	D3D11_INPUT_ELEMENT_DESC e2;
	D3D11_INPUT_ELEMENT_DESC e3;
	D3D11_INPUT_ELEMENT_DESC e4;

	e1.SemanticName = "POSITION";
	e1.SemanticIndex = 0;
	e1.Format = DXGI_FORMAT_R32G32B32_FLOAT;
	e1.InputSlot = 0;
	e1.AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	e1.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	e1.InstanceDataStepRate = 0;

	e2.SemanticName = "NORMAL";
	e2.SemanticIndex = 0;
	e2.Format = DXGI_FORMAT_R32G32B32_FLOAT;
	e2.InputSlot = 0;
	e2.AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	e2.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	e2.InstanceDataStepRate = 0;

	e3.SemanticName = "COLOR";
	e3.SemanticIndex = 0;
	e3.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	e3.InputSlot = 0;
	e3.AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	e3.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	e3.InstanceDataStepRate = 0;

	e4.SemanticName = "TEXCOORD";
	e4.SemanticIndex = 0;
	e4.Format = DXGI_FORMAT_R32G32_FLOAT;
	e4.InputSlot = 0;
	e4.AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	e4.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	e4.InstanceDataStepRate = 0;

	m_elements.add( e1 );
	m_elements.add( e2 );
	m_elements.add( e3 );
	m_elements.add( e4 );
}
//--------------------------------------------------------------------------------
ImmediateGeometryDX11::~ImmediateGeometryDX11()
{
}
//--------------------------------------------------------------------------------
void ImmediateGeometryDX11::Execute( PipelineManagerDX11* pPipeline, IParameterManager* pParamManager )
{
	if ( VertexBuffer.GetVertexCount() > 0 ) {

		VertexBuffer.UploadVertexData( pPipeline );
	
		pPipeline->InputAssemblerStage.ClearDesiredState();

		// Set the Input Assembler state, then perform the draw call.
		int layout = GetInputLayout( pPipeline->ShaderStages[VERTEX_SHADER]->DesiredState.GetShaderProgram() );
		pPipeline->InputAssemblerStage.DesiredState.SetInputLayout( layout );
		pPipeline->InputAssemblerStage.DesiredState.SetPrimitiveTopology( m_ePrimType );
		pPipeline->InputAssemblerStage.DesiredState.SetVertexBuffer( 0, VertexBuffer.GetVertexBuffer()->m_iResource, 0, sizeof( ImmediateVertexDX11 ) );
	
		pPipeline->ApplyInputResources();

		pPipeline->Draw( VertexBuffer.GetVertexCount(), 0 );
	}
}
//--------------------------------------------------------------------------------
void ImmediateGeometryDX11::ResetGeometry()
{
	// Reset the vertex count here to prepare for the next drawing pass.
	VertexBuffer.ResetVertices();
}
//--------------------------------------------------------------------------------
void ImmediateGeometryDX11::AddVertex( const ImmediateVertexDX11& vertex )
{
	VertexBuffer.AddVertex( vertex );
}
//--------------------------------------------------------------------------------
void ImmediateGeometryDX11::AddVertex( const Vector3f& position )
{
	ImmediateVertexDX11 vertex;

	vertex.position = position;
	vertex.normal = Vector3f( 0.0f, 1.0f, 0.0f );
	vertex.color = m_Color;
	vertex.texcoords = Vector2f( 0.0f, 0.0f );

	VertexBuffer.AddVertex( vertex );
}
//--------------------------------------------------------------------------------
void ImmediateGeometryDX11::AddVertex( const Vector3f& position, const Vector4f& color )
{
	ImmediateVertexDX11 vertex;

	vertex.position = position;
	vertex.normal = Vector3f( 0.0f, 1.0f, 0.0f );
	vertex.color = color;
	vertex.texcoords = Vector2f( 0.0f, 0.0f );
	
	VertexBuffer.AddVertex( vertex );
}
//--------------------------------------------------------------------------------
void ImmediateGeometryDX11::AddVertex( const Vector3f& position, const Vector2f& texcoords )
{
	ImmediateVertexDX11 vertex;

	vertex.position = position;
	vertex.normal = Vector3f( 0.0f, 1.0f, 0.0f );
	vertex.color = m_Color;
	vertex.texcoords = texcoords;
	
	VertexBuffer.AddVertex( vertex );
}
//--------------------------------------------------------------------------------
void ImmediateGeometryDX11::AddVertex( const Vector3f& position, const Vector4f& color, const Vector2f& texcoords )
{
	ImmediateVertexDX11 vertex;

	vertex.position = position;
	vertex.normal = Vector3f( 0.0f, 1.0f, 0.0f );
	vertex.color = color;
	vertex.texcoords = texcoords;
	
	VertexBuffer.AddVertex( vertex );
}
//--------------------------------------------------------------------------------
void ImmediateGeometryDX11::AddVertex( const Vector3f& position, const Vector3f& normal )
{
	ImmediateVertexDX11 vertex;

	vertex.position = position;
	vertex.normal = normal;
	vertex.color = m_Color;
	vertex.texcoords = Vector2f( 0.0f, 0.0f );
	
	VertexBuffer.AddVertex( vertex );
}
//--------------------------------------------------------------------------------
void ImmediateGeometryDX11::AddVertex( const Vector3f& position, const Vector3f& normal, const Vector4f& color )
{
	ImmediateVertexDX11 vertex;

	vertex.position = position;
	vertex.normal = normal;
	vertex.color = color;
	vertex.texcoords = Vector2f( 0.0f, 0.0f );
	
	VertexBuffer.AddVertex( vertex );
}
//--------------------------------------------------------------------------------
void ImmediateGeometryDX11::AddVertex( const Vector3f& position, const Vector3f& normal, const Vector2f& texcoords )
{
	ImmediateVertexDX11 vertex;

	vertex.position = position;
	vertex.normal = normal;
	vertex.color = m_Color;
	vertex.texcoords = texcoords;
	
	VertexBuffer.AddVertex( vertex );
}
//--------------------------------------------------------------------------------
void ImmediateGeometryDX11::AddVertex( const Vector3f& position, const Vector3f& normal, const Vector4f& color, const Vector2f& texcoords )
{
	ImmediateVertexDX11 vertex;

	vertex.position = position;
	vertex.normal = normal;
	vertex.color = color;
	vertex.texcoords = texcoords;
	
	VertexBuffer.AddVertex( vertex );
}
//--------------------------------------------------------------------------------
D3D11_PRIMITIVE_TOPOLOGY ImmediateGeometryDX11::GetPrimitiveType()
{
	return( m_ePrimType );
}
//--------------------------------------------------------------------------------
void ImmediateGeometryDX11::SetPrimitiveType( D3D11_PRIMITIVE_TOPOLOGY type )
{
	m_ePrimType = type;
}
//--------------------------------------------------------------------------------
void ImmediateGeometryDX11::SetColor( const Vector4f& color )
{
	m_Color = color;
}
//--------------------------------------------------------------------------------
Vector4f ImmediateGeometryDX11::GetColor( )
{
	return( m_Color );
}
//--------------------------------------------------------------------------------
unsigned int ImmediateGeometryDX11::GetVertexCount()
{
	return( VertexBuffer.GetVertexCount() );
}
//--------------------------------------------------------------------------------
unsigned int ImmediateGeometryDX11::GetPrimitiveCount()
{
	unsigned int count = 0;
	unsigned int referencedVertices = VertexBuffer.GetVertexCount();

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