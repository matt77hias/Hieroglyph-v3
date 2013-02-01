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
#include "InputAssemblerStateDX11.h"
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
InputAssemblerStateDX11::InputAssemblerStateDX11()
{
	m_FeatureLevel = D3D_FEATURE_LEVEL_9_1;
	m_pSisterState = nullptr;
	AvailableSlotCount = 0;

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

	if ( m_FeatureLevel == D3D_FEATURE_LEVEL_11_0 ) {
		AvailableSlotCount = D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT;
	} else {
		AvailableSlotCount = D3D10_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT;
	}

}
//--------------------------------------------------------------------------------
void InputAssemblerStateDX11::SetSisterState( InputAssemblerStateDX11* pState )
{
	m_pSisterState = pState;
}
//--------------------------------------------------------------------------------
void InputAssemblerStateDX11::ResetUpdateFlags( )
{
	m_bUpdateIndexBuffer = false;
	m_bUpdateVertexBuffers = false;
	m_bUpdateInputLayout = false;
	m_bUpdateUpdatePrimitiveTopology = false;
}
//--------------------------------------------------------------------------------
void InputAssemblerStateDX11::SetIndexBuffer( int buffer )
{
	// Update the desired state.
	IndexBuffer = buffer;

	// Test if an update is needed.
	if ( nullptr != m_pSisterState ) {
		if ( m_pSisterState->IndexBuffer != IndexBuffer ) {
			m_bUpdateIndexBuffer = true;
		}
	}
}
//--------------------------------------------------------------------------------
void InputAssemblerStateDX11::SetIndexBufferFormat( DXGI_FORMAT format )
{
	// Update the desired state.
	IndexBufferFormat = format;

	// Test if an update is needed.
	if ( nullptr != m_pSisterState ) {
		if ( m_pSisterState->IndexBufferFormat != IndexBufferFormat ) {
			m_bUpdateIndexBuffer = true;
		}
	}
}
//--------------------------------------------------------------------------------
void InputAssemblerStateDX11::SetVertexBuffer( unsigned int slot, int buffer, 
												unsigned int offset, unsigned int stride )
{

	if ( slot < AvailableSlotCount ) {
		
		// Update the desired state.
		VertexBuffers[slot] = buffer;
		VertexStrides[slot] = stride;
		VertexOffsets[slot] = offset;

		// Test if an update is needed.
		if ( nullptr != m_pSisterState ) {
			if ( m_pSisterState->VertexBuffers[slot] != VertexBuffers[slot]
				|| m_pSisterState->VertexStrides[slot] != VertexStrides[slot]
				|| m_pSisterState->VertexOffsets[slot] != VertexOffsets[slot] ) {

				m_bUpdateVertexBuffers = true;
			}
		}
	}
}
//--------------------------------------------------------------------------------
void InputAssemblerStateDX11::SetInputLayout( int layout )
{
	// Update the desired state.
	InputLayout = layout;

	// Test if an update is needed.
	if ( nullptr != m_pSisterState ) {
		if ( m_pSisterState->InputLayout != InputLayout ) {
			m_bUpdateInputLayout = true;
		}
	}
}
//--------------------------------------------------------------------------------
void InputAssemblerStateDX11::SetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY topology )
{
	// Update the desired state.
	PrimitiveTopology = topology;

	// Test if an update is needed.
	if ( nullptr != m_pSisterState ) {
		if ( m_pSisterState->PrimitiveTopology != PrimitiveTopology ) {
			this->m_bUpdateUpdatePrimitiveTopology = true;
		}
	}	
}
//--------------------------------------------------------------------------------
void InputAssemblerStateDX11::ClearState()
{
	SetIndexBuffer( -1 );

	for ( int i = 0; i < D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT; i++ ) {
		SetVertexBuffer( i, -1, 0, 0 );
	}

	SetInputLayout( -1 );
	SetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_UNDEFINED );
	SetIndexBufferFormat( DXGI_FORMAT_R32_UINT );
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

	if ( slot < AvailableSlotCount ) {
		stride = VertexStrides[slot];
	}

	return( stride );
}
//--------------------------------------------------------------------------------
UINT InputAssemblerStateDX11::GetVertexOffset( UINT slot )
{
	UINT offset = 0;

	if ( slot < AvailableSlotCount ) {
		offset = VertexOffsets[slot];
	}

	return( offset );
}
//--------------------------------------------------------------------------------
int InputAssemblerStateDX11::CompareVertexBufferState( InputAssemblerStateDX11& desired )
{
	int count = 0;

	for ( int i = AvailableSlotCount - 1; i >= 0; i-- ) {
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