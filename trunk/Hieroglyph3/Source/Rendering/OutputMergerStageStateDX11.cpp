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
	RenderTargetViews( -1 ),
	DepthTargetViews( -1 ),
	UnorderedAccessViews( -1 ),
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
//void OutputMergerStageStateDX11::SetBlendState( int state )
//{
//	// Set the desired state.
//	BlendState = state;
//
//	// Test if an update is needed.
//	if ( nullptr != m_pSisterState ) {
//		if ( m_pSisterState->BlendState != BlendState ) {
//			m_bUpdateBlendState = true;
//		}
//	}
//}
////--------------------------------------------------------------------------------
//void OutputMergerStageStateDX11::SetDepthStencilState( int state, unsigned int stencilRef )
//{
//	// Set the desired state.
//	DepthStencilState = state;
//	StencilRef = stencilRef;
//	
//	// Test if an update is needed.
//	if ( nullptr != m_pSisterState ) {
//		if ( m_pSisterState->DepthStencilState != DepthStencilState
//			|| m_pSisterState->StencilRef != StencilRef ) {
//				m_bUpdateDepthStencilState = true;
//		}
//	}
//}
////--------------------------------------------------------------------------------
//void OutputMergerStageStateDX11::SetRenderTarget( unsigned int slot, int rtv )
//{
//	if ( slot < D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT ) {
//
//		// Update the desired state.
//		RenderTargetViews[slot] = rtv;
//
//		// Test if an update is needed.
//		if ( nullptr != m_pSisterState ) {
//			if ( m_pSisterState->RenderTargetViews[slot] != RenderTargetViews[slot] ) {
//				m_bUpdateRenderTargetState = true;
//			}
//		}
//	}
//}
////--------------------------------------------------------------------------------
//void OutputMergerStageStateDX11::SetDepthStencilTarget( int dsv )
//{
//	// Update the desired state.
//	DepthTargetViews = dsv;
//
//	// Test if an update is needed.
//	if ( nullptr != m_pSisterState ) {
//		if ( m_pSisterState->DepthTargetViews != DepthTargetViews ) {
//			m_bUpdateDepthStencilTarget = true;
//		}
//	}
//}
////--------------------------------------------------------------------------------
//void OutputMergerStageStateDX11::SetUnorderedAccessView( unsigned int slot, int uav, unsigned int initCount )
//{
//	if ( slot < D3D11_PS_CS_UAV_REGISTER_COUNT )
//	{
//		// Update the desired state.
//		UnorderedAccessViews[slot] = uav;
//		UAVInitialCounts[slot] = initCount;
//
//		// Test if an update is needed.
//		if ( nullptr != m_pSisterState ) {
//			if ( m_pSisterState->UnorderedAccessViews[slot] != UnorderedAccessViews[slot]
//				|| m_pSisterState->UAVInitialCounts[slot] != UAVInitialCounts[slot] ) {
//			
//				m_bUpdateUnorderedAccessState = true;
//			}
//		}
//	}
//}
////--------------------------------------------------------------------------------
//int OutputMergerStageStateDX11::GetBlendState() const
//{
//	return( BlendState );
//}
////--------------------------------------------------------------------------------
//int OutputMergerStageStateDX11::GetDepthStencilState() const
//{
//	return( DepthStencilState );
//}
////--------------------------------------------------------------------------------
//unsigned int OutputMergerStageStateDX11::GetStencilReference() const
//{
//	return( StencilRef );
//}
////--------------------------------------------------------------------------------
//int OutputMergerStageStateDX11::GetRenderTarget( unsigned int slot ) const
//{
//	int result = -1;
//
//	if ( slot < D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT ) {
//		result = RenderTargetViews[slot];
//	}
//
//	return( result );
//}
////--------------------------------------------------------------------------------
//int OutputMergerStageStateDX11::GetDepthStencilTarget( ) const
//{
//	return( DepthTargetViews );
//}
////--------------------------------------------------------------------------------
//int OutputMergerStageStateDX11::GetUnorderedAccessView( unsigned int slot ) const
//{
//	int result = -1;
//
//	if ( slot < D3D11_PS_CS_UAV_REGISTER_COUNT ) {
//		result = UnorderedAccessViews[slot];
//	}
//
//	return( result );
//}
////--------------------------------------------------------------------------------
//int OutputMergerStageStateDX11::GetInitialCount( unsigned int slot ) const
//{
//	int result = -1;
//
//	if ( slot < D3D11_PS_CS_UAV_REGISTER_COUNT ) {
//		result = UAVInitialCounts[slot];
//	}
//
//	return( result );
//}
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
//int OutputMergerStageStateDX11::CompareBlendState( OutputMergerStageStateDX11& desired )
//{
//	if ( BlendState == desired.BlendState )
//		return( 0 );
//	else
//		return( 1 );
//}
////--------------------------------------------------------------------------------
//int OutputMergerStageStateDX11::CompareDepthStencilState( OutputMergerStageStateDX11& desired )
//{
//	if ( DepthStencilState == desired.DepthStencilState && StencilRef == desired.StencilRef )
//		return( 0 );
//	else
//		return( 1 );
//}
////--------------------------------------------------------------------------------
//int OutputMergerStageStateDX11::CompareRenderTargets( OutputMergerStageStateDX11& desired )
//{
//	int i = 0;
//	for ( i = 0; i <= D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT-1; i++ )
//	{
//		if ( RenderTargetViews[i] != desired.RenderTargetViews[i] )
//		{
//			break;
//		}
//	}
//
//	return( i+1 );
//}
////--------------------------------------------------------------------------------
//int OutputMergerStageStateDX11::CompareUnorderedAccessViews( OutputMergerStageStateDX11& desired )
//{
//	int count = 0;
//	for ( int i = D3D11_PS_CS_UAV_REGISTER_COUNT-1; i >= 0; i-- )
//	{
//		if ( UnorderedAccessViews[i] != desired.UnorderedAccessViews[i]
//			|| UAVInitialCounts[i] != desired.UAVInitialCounts[i] )
//		{
//			count = i+1;
//			break;
//		}
//	}
//
//	return( count );
//}
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
