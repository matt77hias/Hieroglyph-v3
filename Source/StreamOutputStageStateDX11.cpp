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
#include "StreamOutputStageStateDX11.h"
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
StreamOutputStageStateDX11::StreamOutputStageStateDX11() :
	StreamBuffers( -1 ),
	StreamOffsets( 0 ),
	m_FeatureLevel( D3D_FEATURE_LEVEL_9_1 ),
	m_pSisterState( nullptr )
{
	ClearState();
}
//--------------------------------------------------------------------------------
StreamOutputStageStateDX11::~StreamOutputStageStateDX11()
{
	
}
//--------------------------------------------------------------------------------
void StreamOutputStageStateDX11::SetFeautureLevel( D3D_FEATURE_LEVEL level )
{
	m_FeatureLevel = level;
}
//--------------------------------------------------------------------------------
int StreamOutputStageStateDX11::GetBufferCount() const
{
	unsigned int count = 0;

	for ( unsigned int i = 0; i < 4; i++ )
	{
		if ( StreamBuffers.GetState( i ) != -1 )
			count++;
	}

	return( count );
}
//--------------------------------------------------------------------------------
void StreamOutputStageStateDX11::ClearState( )
{
	StreamBuffers.InitializeStates();
	StreamOffsets.InitializeStates();
}
//--------------------------------------------------------------------------------
void StreamOutputStageStateDX11::SetSisterState( StreamOutputStageStateDX11* pState )
{
	m_pSisterState = pState;

	StreamBuffers.SetSister( &m_pSisterState->StreamBuffers );
	StreamOffsets.SetSister( &m_pSisterState->StreamOffsets );
}
//--------------------------------------------------------------------------------
void StreamOutputStageStateDX11::ResetUpdateFlags( )
{
	StreamBuffers.ResetTracking();
	StreamOffsets.ResetTracking();
}
//--------------------------------------------------------------------------------
