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
#include "OutputMergerStageDX11.h"
#include "RenderTargetViewDX11.h"
#include "DepthStencilViewDX11.h"
#include "UnorderedAccessViewDX11.h"
#include "Log.h"
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
OutputMergerStageDX11::OutputMergerStageDX11()
{
}
//--------------------------------------------------------------------------------
OutputMergerStageDX11::~OutputMergerStageDX11()
{
}
//--------------------------------------------------------------------------------
void OutputMergerStageDX11::SetFeautureLevel( D3D_FEATURE_LEVEL level )
{
	m_FeatureLevel = level;
	CurrentState.SetFeautureLevel( level );
	DesiredState.SetFeautureLevel( level );
}
//--------------------------------------------------------------------------------
void OutputMergerStageDX11::ClearDesiredState( )
{
	DesiredState.ClearState();
}
//--------------------------------------------------------------------------------
void OutputMergerStageDX11::ClearCurrentState( )
{
	CurrentState.ClearState();
}
//--------------------------------------------------------------------------------
void OutputMergerStageDX11::ApplyDesiredState( ID3D11DeviceContext* pContext )
{
	ApplyDesiredRenderTargetStates( pContext );
	ApplyDesiredBlendAndDepthStencilStates( pContext );
}
//--------------------------------------------------------------------------------
void OutputMergerStageDX11::ApplyDesiredRenderTargetStates( ID3D11DeviceContext* pContext )
{
	int rtvCount = CurrentState.CompareRenderTargets( DesiredState );
	int uavCount = CurrentState.CompareUnorderedAccessViews( DesiredState );

	if ( rtvCount > 0 || uavCount > 0 ) {

		RendererDX11* pRenderer = RendererDX11::Get();

		ID3D11RenderTargetView*	rtvs[D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT];
		ID3D11UnorderedAccessView* uavs[D3D11_PS_CS_UAV_REGISTER_COUNT];
		ID3D11DepthStencilView* dsv = 0;

		for ( int i = 0; i < D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT; i++ ) {

			RenderTargetViewDX11* pRenderTargetView = pRenderer->GetRenderTargetViewByIndex( DesiredState.GetRenderTarget( i ) );
			
			if ( pRenderTargetView != 0 ) {
				rtvCount = i+1; // Record the number of non-null rtvs...
				rtvs[i] = pRenderTargetView->m_pRenderTargetView;
			} else {
				rtvs[i] = 0;
			}
		}


		for ( int i = 0; i < D3D11_PS_CS_UAV_REGISTER_COUNT; i++ ) {

			UnorderedAccessViewDX11* pUnorderedAccessView = pRenderer->GetUnorderedAccessViewByIndex( DesiredState.GetUnorderedAccessView( i ) );
			
			if ( pUnorderedAccessView != 0 ) {
				uavCount = i+1; // Record the number of non-null uavs...
				uavs[i] = pUnorderedAccessView->m_pUnorderedAccessView;
			} else {
				uavs[i] = 0;
			}
		}

		
		DepthStencilViewDX11* pDepthStencilView = pRenderer->GetDepthStencilViewByIndex( DesiredState.GetDepthStencilTarget() );

		if ( pDepthStencilView ) {
			dsv = pDepthStencilView->m_pDepthStencilView;
		} else {
			dsv = 0;
		}

		// TODO: convert this to bind the UAVs too...
		pContext->OMSetRenderTargets( D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT, rtvs, dsv );

		// 
		//pContext->OMSetRenderTargetsAndUnorderedAccessViews( rtvCount, rtvs, dsv, 
		//	rtvCount, uavCount, uavs, (UINT*)&DesiredState.UAVInitialCounts );

		for ( int i = 0; i < D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT; i++ ) {
			CurrentState.SetRenderTarget( i, DesiredState.GetRenderTarget( i ) );
		}

		for ( int i = 0; i < D3D11_PS_CS_UAV_REGISTER_COUNT; i++ ) {
			CurrentState.SetUnorderedAccessView( i, DesiredState.GetUnorderedAccessView( i ), DesiredState.GetInitialCount( i ) );
		}

		CurrentState.SetDepthStencilTarget( DesiredState.GetDepthStencilTarget() );
	}
	
}
//--------------------------------------------------------------------------------
void OutputMergerStageDX11::ApplyDesiredBlendAndDepthStencilStates( ID3D11DeviceContext* pContext )
{
	RendererDX11* pRenderer = RendererDX11::Get();

	if ( CurrentState.CompareBlendState( DesiredState ) == 1 ) {

		ID3D11BlendState* pBlendState = pRenderer->GetBlendState( DesiredState.GetBlendState() )->m_pState;

		// TODO: Add in the blend factors as states to the OutputMergerStageStateDX11 class!
		if ( pBlendState )
		{
			float afBlendFactors[] = { 1.0f, 1.0f, 1.0f, 1.0f };
			pContext->OMSetBlendState( pBlendState, afBlendFactors, 0xFFFFFFFF );
		}

		CurrentState.SetBlendState( DesiredState.GetBlendState() );
	}

	if ( CurrentState.CompareDepthStencilState( DesiredState ) == 1 ) {

		ID3D11DepthStencilState* pDepthState = pRenderer->GetDepthState( DesiredState.GetDepthStencilState() )->m_pState;

		if ( pDepthState )
		{
			pContext->OMSetDepthStencilState( pDepthState, DesiredState.GetStencilReference() );
		}

		CurrentState.SetDepthStencilState( DesiredState.GetDepthStencilState(), DesiredState.GetStencilReference() );
	}
}
//--------------------------------------------------------------------------------
const OutputMergerStageStateDX11& OutputMergerStageDX11::GetCurrentState() const
{
	return( CurrentState );
}
//--------------------------------------------------------------------------------
//
////--------------------------------------------------------------------------------
//void OutputMergerStageDX11::BindResources( ID3D11DeviceContext* pContext )
//{
//	// Update all of the render targets and depth stencil targets
//	
//	// Find the highest index that doesn't match.
//	int max = 0;
//	for ( int i = D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT-1; i >= 0; i-- )
//	{
//		if ( RenderTargetViews[i] != APIRenderTargetViews[i] )
//		{
//			max = i+1;
//			break;
//		}
//	}
//
//    if ( DepthTargetViews != APIDepthTargetViews )
//        max++;
//
//	// If any targets are different then copy them over.
//	if ( max > 0 )
//		pContext->OMSetRenderTargets( D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT, RenderTargetViews, DepthTargetViews );
//	
//	// Update the API views to know what to update next time.
//	for ( int i = 0; i < D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT; i++ )
//		APIRenderTargetViews[i] = RenderTargetViews[i];
//
//    APIDepthTargetViews = DepthTargetViews;
//}
////--------------------------------------------------------------------------------
//void OutputMergerStageDX11::SetToDefaultState()
//{
//	memset( APIRenderTargetViews, 0, D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT * sizeof( APIRenderTargetViews[0] ) );
//}
////--------------------------------------------------------------------------------
//void OutputMergerStageDX11::ClearResources()
//{
//	// Clear out all array elements in our cached arrays.  This will be used to 
//	// write nulls into the context later on.
//
//	memset( RenderTargetViews, 0, D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT * sizeof( RenderTargetViews[0] ) );
//	DepthTargetViews = 0;
//	memset( UnorderedAccessViews, 0, sizeof( UnorderedAccessViews[0] ) * D3D11_PS_CS_UAV_REGISTER_COUNT );
//	for ( int i = 0; i < D3D11_PS_CS_UAV_REGISTER_COUNT; i++ ) UAVInitialCounts[i] = -1;
//}
////--------------------------------------------------------------------------------
//void OutputMergerStageDX11::UnbindResources( ID3D11DeviceContext* pContext )
//{
//	// Clear out the resource settings.
//
//	ClearResources();
//
//	// Bind the changes to the pipeline.
//
//	BindResources( pContext );
//}
////--------------------------------------------------------------------------------
//unsigned int OutputMergerStageDX11::GetViewsSetCount()
//{
//	unsigned int count = 0;
//
//	for ( int i = 0; i < D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT; i++ )
//	{
//		if ( RenderTargetViews[i] != 0 )
//			count++;
//	}
//
//	return( count );
//}
////--------------------------------------------------------------------------------
//unsigned int OutputMergerStageDX11::GetViewsBoundCount()
//{
//	unsigned int count = 0;
//
//	for ( int i = 0; i < D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT; i++ )
//	{
//		if ( APIRenderTargetViews[i] != 0 )
//			count++;
//	}
//
//	return( count );
//}
////--------------------------------------------------------------------------------