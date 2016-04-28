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
#include "OutputMergerStageStateDX11.h"
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
OutputMergerStageStateDX11::OutputMergerStageStateDX11() :
	BlendState( -1 ),
	DepthStencilState( -1 ),
	StencilRef( 0 ),
	RenderTargetViews( 0 ),
	DepthTargetViews( 0 ),
	UnorderedAccessViews( 0 ),
	UAVInitialCounts( -1 ),
	m_FeatureLevel( D3D_FEATURE_LEVEL_9_1 ),
	m_pSisterState( nullptr )
{
	ClearState();
}
//--------------------------------------------------------------------------------
OutputMergerStageStateDX11::~OutputMergerStageStateDX11()
{
	
}
//--------------------------------------------------------------------------------
void OutputMergerStageStateDX11::SetFeautureLevel( D3D_FEATURE_LEVEL level )
{
	m_FeatureLevel = level;
}
//--------------------------------------------------------------------------------
int OutputMergerStageStateDX11::GetRenderTargetCount() const
{
	unsigned int count = 0;

	for ( unsigned int i = 0; i < D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT; i++ )
	{
		if ( RenderTargetViews.GetState( i ) != -1 )
			count++;
	}

	return( count );
}
//--------------------------------------------------------------------------------
void OutputMergerStageStateDX11::ClearState( )
{
	BlendState.InitializeState();
	DepthStencilState.InitializeState();
	StencilRef.InitializeState();
	RenderTargetViews.InitializeStates();
	DepthTargetViews.InitializeState();
	UnorderedAccessViews.InitializeStates();
	UAVInitialCounts.InitializeStates();
}
//--------------------------------------------------------------------------------
void OutputMergerStageStateDX11::SetSisterState( OutputMergerStageStateDX11* pState )
{
	m_pSisterState = pState;

	BlendState.SetSister( &m_pSisterState->BlendState );
	DepthStencilState.SetSister( &m_pSisterState->DepthStencilState );
	StencilRef.SetSister( &m_pSisterState->StencilRef );
	RenderTargetViews.SetSister( &m_pSisterState->RenderTargetViews );
	DepthTargetViews.SetSister( &m_pSisterState->DepthTargetViews );
	UnorderedAccessViews.SetSister( &m_pSisterState->UnorderedAccessViews );
	UAVInitialCounts.SetSister( &m_pSisterState->UAVInitialCounts );
}
//--------------------------------------------------------------------------------
void OutputMergerStageStateDX11::ResetUpdateFlags( )
{
	BlendState.ResetTracking();
	DepthStencilState.ResetTracking();
	StencilRef.ResetTracking();
	RenderTargetViews.ResetTracking();
	DepthTargetViews.ResetTracking();
	UnorderedAccessViews.ResetTracking();
	UAVInitialCounts.ResetTracking();
}
//--------------------------------------------------------------------------------
