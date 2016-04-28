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
InputAssemblerStateDX11::InputAssemblerStateDX11() :
	IndexBuffer( -1 ),
	IndexBufferFormat( DXGI_FORMAT_R32_UINT ),
	VertexBuffers( -1 ),
	VertexBufferStrides( 0 ),
	VertexBufferOffsets( 0 ),
	InputLayout( -1 ),
	PrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_UNDEFINED ),
	m_FeatureLevel( D3D_FEATURE_LEVEL_9_1 ),
	m_pSisterState( nullptr ),
	AvailableSlotCount( 0 )
{
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

	IndexBuffer.SetSister( &m_pSisterState->IndexBuffer );
	IndexBufferFormat.SetSister( &m_pSisterState->IndexBufferFormat );
	VertexBuffers.SetSister( &m_pSisterState->VertexBuffers );
	VertexBufferStrides.SetSister( &m_pSisterState->VertexBufferStrides );
	VertexBufferOffsets.SetSister( &m_pSisterState->VertexBufferOffsets );
	InputLayout.SetSister( &m_pSisterState->InputLayout );
	PrimitiveTopology.SetSister( &m_pSisterState->PrimitiveTopology );
}
//--------------------------------------------------------------------------------
unsigned int InputAssemblerStateDX11::GetAvailableSlotCount()
{
	return( AvailableSlotCount );
}
//--------------------------------------------------------------------------------
void InputAssemblerStateDX11::ResetUpdateFlags( )
{
	IndexBuffer.ResetTracking();
	IndexBufferFormat.ResetTracking();
	VertexBuffers.ResetTracking();
	VertexBufferStrides.ResetTracking();
	VertexBufferOffsets.ResetTracking();
	InputLayout.ResetTracking();
	PrimitiveTopology.ResetTracking();
}
//--------------------------------------------------------------------------------
void InputAssemblerStateDX11::ClearState()
{
	IndexBuffer.InitializeState();
	IndexBufferFormat.InitializeState();
	VertexBuffers.InitializeStates();
	VertexBufferStrides.InitializeStates();
	VertexBufferOffsets.InitializeStates();
	InputLayout.InitializeState();
	PrimitiveTopology.InitializeState();
}
//--------------------------------------------------------------------------------
