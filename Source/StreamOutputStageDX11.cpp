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
#include "StreamOutputStageDX11.h"
#include "VertexBufferDX11.h"
#include "Log.h"
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
StreamOutputStageDX11::StreamOutputStageDX11()
{
	DesiredState.SetSisterState( &CurrentState );
}
//--------------------------------------------------------------------------------
StreamOutputStageDX11::~StreamOutputStageDX11()
{
}
//--------------------------------------------------------------------------------
void StreamOutputStageDX11::SetFeautureLevel( D3D_FEATURE_LEVEL level )
{
	m_FeatureLevel = level;
	CurrentState.SetFeautureLevel( level );
	DesiredState.SetFeautureLevel( level );
}
//--------------------------------------------------------------------------------
void StreamOutputStageDX11::ClearDesiredState( )
{
	DesiredState.ClearState();
}
//--------------------------------------------------------------------------------
void StreamOutputStageDX11::ClearCurrentState( )
{
	CurrentState.ClearState();
}
//--------------------------------------------------------------------------------
void StreamOutputStageDX11::ApplyDesiredState( ID3D11DeviceContext* pContext )
{
	RendererDX11* pRenderer = RendererDX11::Get();

	// Bind the vertex buffers
	if ( DesiredState.StreamBuffers.IsUpdateNeeded()
		|| DesiredState.StreamOffsets.IsUpdateNeeded() )
	{
		ID3D11Buffer* Buffers[4] = { NULL };

		for ( unsigned int i = 0; i < sizeof( Buffers ) / sizeof( ID3D11Buffer* ); i++ )
		{
			int index = DesiredState.StreamBuffers.GetState( i );

			VertexBufferDX11* pBuffer = pRenderer->GetVertexBufferByIndex( index );

			if ( pBuffer ) {
				Buffers[i] = static_cast<ID3D11Buffer*>( pBuffer->GetResource() );
			} else {
				Buffers[i] = 0;
			}
		}

		// Start slot is always zero since there is no way to specify buffers
		// starting from a different index.

		UINT startSlot = 0;

		UINT endSlot = max( DesiredState.StreamBuffers.GetEndSlot(),
			DesiredState.StreamOffsets.GetEndSlot() );

		pContext->SOSetTargets( 
			endSlot-startSlot+1, 
			Buffers,
			DesiredState.StreamOffsets.GetSlotLocation( 0 ) );
	}

	DesiredState.ResetUpdateFlags();
	CurrentState = DesiredState;
}
//--------------------------------------------------------------------------------
const StreamOutputStageStateDX11& StreamOutputStageDX11::GetCurrentState() const
{
	return( CurrentState );
}
//--------------------------------------------------------------------------------
