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
#include "ResourceDX11.h"
#include "Log.h"
#include "InputAssemblerStageDX11.h"
#include "InputLayoutDX11.h"
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
InputAssemblerStageDX11::InputAssemblerStageDX11()
{
}
//--------------------------------------------------------------------------------
InputAssemblerStageDX11::~InputAssemblerStageDX11()
{
	
}
//--------------------------------------------------------------------------------
void InputAssemblerStageDX11::SetFeautureLevel( D3D_FEATURE_LEVEL level )
{
	m_FeatureLevel = level;
}
//--------------------------------------------------------------------------------
void InputAssemblerStageDX11::SetDesiredState( InputAssemblerStateDX11& state )
{
	m_DesiredState.IndexBuffer = state.IndexBuffer;

	for ( int i = 0; i < D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT; i++ ) {
		m_DesiredState.VertexBuffers[i] = state.VertexBuffers[i];
		m_DesiredState.VertexStrides[i] = state.VertexStrides[i];
		m_DesiredState.VertexOffsets[i] = state.VertexOffsets[i];
	}

	m_DesiredState.InputLayout = state.InputLayout;
	m_DesiredState.PrimitiveTopology = state.PrimitiveTopology;
}
//--------------------------------------------------------------------------------
void InputAssemblerStageDX11::ClearDesiredState()
{
	m_DesiredState.ClearState();
}
//--------------------------------------------------------------------------------
void InputAssemblerStageDX11::ApplyDesiredState( ID3D11DeviceContext* pContext )
{
	// Bind the input layout first.

	RendererDX11* pRenderer = RendererDX11::Get();
	InputLayoutDX11* pLayout = pRenderer->GetInputLayout( m_DesiredState.InputLayout );

	if ( pLayout )
		pContext->IASetInputLayout( pLayout->m_pInputLayout );
	else
		Log::Get().Write( L"Tried to bind an invalid input layout ID!" );


	// Bind the primitive topology

	pContext->IASetPrimitiveTopology( m_DesiredState.PrimitiveTopology );


	// Bind the vertex buffers

	ID3D11Buffer* Buffers[D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT] = { NULL };

	UINT slot = 0;
	while ( slot < D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT
		&& m_DesiredState.VertexBuffers[slot] != -1 )
	{
		int index = m_DesiredState.VertexBuffers[slot];

		// Select only the index portion of the handle.
		int TYPE	= index & 0x00FF0000;
		int ID		= index & 0x0000FFFF;

		Buffers[slot] = reinterpret_cast<ID3D11Buffer*>( pRenderer->GetResource( ID )->GetResource() );
		if ( !Buffers[slot] )					
			Log::Get().Write( L"Tried to bind an invalid vertex buffer ID!" );

		slot++;
	}

	pContext->IASetVertexBuffers( 0, slot, Buffers, m_DesiredState.VertexStrides, m_DesiredState.VertexOffsets );


	// TODO: Add the ability to use different formats and offsets to this function!
	int index = m_DesiredState.IndexBuffer;

	// Select only the index portion of the handle.
	int TYPE	= index & 0x00FF0000;
	int ID		= index & 0x0000FFFF;

	ID3D11Buffer* pBuffer = (ID3D11Buffer*)pRenderer->GetResource( ID )->GetResource();

	// If the resource is in range, then attempt to set it
	if ( pBuffer ) {
		pContext->IASetIndexBuffer( pBuffer, DXGI_FORMAT_R32_UINT, 0 );
	}
	else {
		Log::Get().Write( L"Tried to bind an invalid index buffer ID!" );
	}
}
//--------------------------------------------------------------------------------
