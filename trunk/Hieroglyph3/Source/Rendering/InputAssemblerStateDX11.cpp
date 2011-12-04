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
#include "InputAssemblerStateDX11.h"
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
InputAssemblerStateDX11::InputAssemblerStateDX11()
{
	m_FeatureLevel = D3D_FEATURE_LEVEL_9_1;

	ClearState();
}
//--------------------------------------------------------------------------------
InputAssemblerStateDX11::~InputAssemblerStateDX11()
{
	
}
//--------------------------------------------------------------------------------
void InputAssemblerStateDX11::SetFeautureLevel( D3D_FEATURE_LEVEL level )
{
	m_FeatureLevel = level;
}
//--------------------------------------------------------------------------------
void InputAssemblerStateDX11::SetIndexBuffer( int buffer )
{
	IndexBuffer = buffer;
}
//--------------------------------------------------------------------------------
void InputAssemblerStateDX11::SetVertexBuffer( unsigned int slot, int buffer, 
												unsigned int offset, unsigned int stride )
{
	if ( slot < D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT ) {
		VertexBuffers[slot] = buffer;
		VertexStrides[slot] = stride;
		VertexOffsets[slot] = offset;
	} else {
		// TODO: add error reaction here...
	}
}
//--------------------------------------------------------------------------------
void InputAssemblerStateDX11::SetInputLayout( int layout )
{
	InputLayout = layout;
}
//--------------------------------------------------------------------------------
void InputAssemblerStateDX11::SetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY topology )
{
	PrimitiveTopology = topology;
}
//--------------------------------------------------------------------------------
void InputAssemblerStateDX11::ClearState()
{
	IndexBuffer = -1;

	for ( int i = 0; i < D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT; i++ ) {
		VertexBuffers[i] = -1;
		VertexStrides[i] = 0;
		VertexOffsets[i] = 0;
	}

	InputLayout = -1;
	PrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_UNDEFINED;
}
//--------------------------------------------------------------------------------
D3D11_PRIMITIVE_TOPOLOGY InputAssemblerStateDX11::GetPrimitiveTopology()
{
	return( PrimitiveTopology );
}
//--------------------------------------------------------------------------------
UINT InputAssemblerStateDX11::GetVertexStride( UINT slot )
{
	UINT stride = 0;

	if ( slot < D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT ) {
		stride = VertexStrides[slot];
	}

	return( stride );
}
//--------------------------------------------------------------------------------
UINT InputAssemblerStateDX11::GetVertexOffset( UINT slot )
{
	UINT offset = 0;

	if ( slot < D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT ) {
		offset = VertexOffsets[slot];
	}

	return( offset );
}
//--------------------------------------------------------------------------------
int InputAssemblerStateDX11::CompareVertexBufferState( InputAssemblerStateDX11& desired )
{
	int count = 0;

	for ( int i = D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT - 1; i >= 0; i-- ) {
		if ( VertexBuffers[i] != desired.VertexBuffers[i] 
			|| VertexStrides[i] != desired.VertexStrides[i] 
			|| VertexOffsets[i] != desired.VertexOffsets[i] ) {
			count = i+1; // return the number of buffers to set
			break;
		}
	}

	return( count );
}
//--------------------------------------------------------------------------------