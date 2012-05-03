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
#include "RendererDX11.h"
#include "Log.h"
#include "InputAssemblerStageDX11.h"
#include "InputLayoutDX11.h"
#include "VertexBufferDX11.h"
#include "IndexBufferDX11.h"
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
InputAssemblerStageDX11::InputAssemblerStageDX11()
{
	DesiredState.SetSisterState( &CurrentState );
}
//--------------------------------------------------------------------------------
InputAssemblerStageDX11::~InputAssemblerStageDX11()
{
	
}
//--------------------------------------------------------------------------------
void InputAssemblerStageDX11::SetFeautureLevel( D3D_FEATURE_LEVEL level )
{
	m_FeatureLevel = level;
	CurrentState.SetFeautureLevel( level );
	DesiredState.SetFeautureLevel( level );
}
//--------------------------------------------------------------------------------
void InputAssemblerStageDX11::ClearDesiredState()
{
	DesiredState.ClearState();
}
//--------------------------------------------------------------------------------
void InputAssemblerStageDX11::ClearCurrentState()
{
	CurrentState.ClearState();
}
//--------------------------------------------------------------------------------
void InputAssemblerStageDX11::ApplyDesiredState( ID3D11DeviceContext* pContext )
{
	// Bind the input layout first.

	RendererDX11* pRenderer = RendererDX11::Get();

	// Compare the primitive topology of the desired and current states
	if ( true == DesiredState.m_bUpdateInputLayout ) {
		InputLayoutDX11* pLayout = pRenderer->GetInputLayout( DesiredState.InputLayout );

		if ( pLayout ) {
			pContext->IASetInputLayout( pLayout->m_pInputLayout );
		} else {
			Log::Get().Write( L"Tried to bind an invalid input layout ID!" );
		}
	}

	// Bind the primitive topology
	if ( true == DesiredState.m_bUpdateUpdatePrimitiveTopology ) {
		pContext->IASetPrimitiveTopology( DesiredState.PrimitiveTopology );
	}

	// Bind the vertex buffers
	if ( true == DesiredState.m_bUpdateVertexBuffers ) {

		ID3D11Buffer* Buffers[D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT] = { NULL };

		for ( int i = 0; i <= DesiredState.AvailableSlotCount-1; i++ )
		{
			int index = DesiredState.VertexBuffers[i];

			VertexBufferDX11* pBuffer = pRenderer->GetVertexBufferByIndex( index );

			if ( pBuffer ) {
				Buffers[i] = static_cast<ID3D11Buffer*>( pBuffer->GetResource() );
			} else {
				Buffers[i] = 0;
			}
		}

		pContext->IASetVertexBuffers( 0, DesiredState.AvailableSlotCount, Buffers, DesiredState.VertexStrides, DesiredState.VertexOffsets );
	}

	if ( true == DesiredState.m_bUpdateIndexBuffer ) {
	
		// TODO: Add the ability to use different formats and offsets to this function!
		int index = DesiredState.IndexBuffer;

		IndexBufferDX11* pBuffer = pRenderer->GetIndexBufferByIndex( index );

		if ( pBuffer ) {
			ID3D11Buffer* pIndexBuffer = reinterpret_cast<ID3D11Buffer*>( pBuffer->GetResource() );
			pContext->IASetIndexBuffer( pIndexBuffer, DesiredState.IndexBufferFormat, 0 );
		} else {
			pContext->IASetIndexBuffer( 0, DesiredState.IndexBufferFormat, 0 );
		}
	}

	DesiredState.ResetUpdateFlags();
	CurrentState = DesiredState;
}
//--------------------------------------------------------------------------------
