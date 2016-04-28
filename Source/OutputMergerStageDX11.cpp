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
	DesiredState.SetSisterState( &CurrentState );
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
	int rtvCount = 0;
	int uavCount = 0;

	if ( DesiredState.RenderTargetViews.IsUpdateNeeded() 
		|| DesiredState.UnorderedAccessViews.IsUpdateNeeded()
		|| DesiredState.DepthTargetViews.IsUpdateNeeded() ) {

		RendererDX11* pRenderer = RendererDX11::Get();

		ID3D11RenderTargetView*	rtvs[D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT];
		ID3D11UnorderedAccessView* uavs[D3D11_PS_CS_UAV_REGISTER_COUNT];
		ID3D11DepthStencilView* dsv = 0;

		for ( int i = 0; i < D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT; i++ ) {

			RenderTargetViewDX11& rtv = pRenderer->GetRenderTargetViewByIndex( DesiredState.RenderTargetViews.GetState( i ) );
			rtvs[i] = rtv.m_pRenderTargetView.Get();

			if ( rtvs[i] != nullptr ) {
				rtvCount = i+1; // Record the number of non-null rtvs...
			}
		}

		for ( int i = 0; i < D3D11_PS_CS_UAV_REGISTER_COUNT; i++ ) {

			UnorderedAccessViewDX11& uav = pRenderer->GetUnorderedAccessViewByIndex( DesiredState.UnorderedAccessViews.GetState( i ) );
			uavs[i] = uav.m_pUnorderedAccessView.Get();

			if ( uavs[i] != nullptr ) {
				uavCount = i+1; // Record the number of non-null uavs...
			}
		}

		
		DepthStencilViewDX11& DSV = pRenderer->GetDepthStencilViewByIndex( DesiredState.DepthTargetViews.GetState() );
		dsv = DSV.m_pDepthStencilView.Get();

		// TODO: convert this to bind the UAVs too...
		pContext->OMSetRenderTargets( D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT, rtvs, dsv );
		//pContext->OMSetRenderTargetsAndUnorderedAccessViews( rtvCount, rtvs, dsv, 
		//	rtvCount, uavCount, uavs, (UINT*)&DesiredState.UAVInitialCounts );

		// TODO: Find a better way to copy the state from desired to current...

		for ( int i = 0; i < D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT; i++ ) {
			CurrentState.RenderTargetViews.SetState( i, DesiredState.RenderTargetViews.GetState( i ) );
		}

		for ( int i = 0; i < D3D11_PS_CS_UAV_REGISTER_COUNT; i++ ) {
			CurrentState.UnorderedAccessViews.SetState( i, DesiredState.UnorderedAccessViews.GetState( i ) );
			CurrentState.UAVInitialCounts.SetState( i, DesiredState.UAVInitialCounts.GetState( i ) );
		}

		CurrentState.DepthTargetViews.SetState( DesiredState.DepthTargetViews.GetState() );

		DesiredState.RenderTargetViews.ResetTracking();
		DesiredState.UnorderedAccessViews.ResetTracking();
		DesiredState.UAVInitialCounts.ResetTracking();
		DesiredState.DepthTargetViews.ResetTracking();
	}
}
//--------------------------------------------------------------------------------
void OutputMergerStageDX11::ApplyDesiredBlendAndDepthStencilStates( ID3D11DeviceContext* pContext )
{
	RendererDX11* pRenderer = RendererDX11::Get();

	if ( DesiredState.BlendState.IsUpdateNeeded() ) {

		BlendStateComPtr pGlyphBlendState = pRenderer->GetBlendState( DesiredState.BlendState.GetState() );

		if ( nullptr != pGlyphBlendState ) {
			
			ID3D11BlendState* pBlendState = pGlyphBlendState.Get();

			// TODO: Add in the blend factors as states to the OutputMergerStageStateDX11 class!
			if ( pBlendState ) {
				float afBlendFactors[] = { 1.0f, 1.0f, 1.0f, 1.0f };
				pContext->OMSetBlendState( pBlendState, afBlendFactors, 0xFFFFFFFF );
			}

			CurrentState.BlendState.SetState( DesiredState.BlendState.GetState() );
			DesiredState.BlendState.ResetTracking();
		}
	}

	if ( DesiredState.DepthStencilState.IsUpdateNeeded() || DesiredState.StencilRef.IsUpdateNeeded() ) {

		DepthStencilStateComPtr pGlyphDepthStencilState = pRenderer->GetDepthState( DesiredState.DepthStencilState.GetState() );
		
		if ( nullptr != pGlyphDepthStencilState ) {

			ID3D11DepthStencilState* pDepthState = pGlyphDepthStencilState.Get();

			if ( pDepthState ) {
				pContext->OMSetDepthStencilState( pDepthState, DesiredState.StencilRef.GetState() );
			}

			CurrentState.DepthStencilState.SetState( DesiredState.DepthStencilState.GetState() );
			CurrentState.StencilRef.SetState( DesiredState.StencilRef.GetState() );
			DesiredState.DepthStencilState.ResetTracking();
			DesiredState.StencilRef.ResetTracking();
		}
	}
}
//--------------------------------------------------------------------------------
const OutputMergerStageStateDX11& OutputMergerStageDX11::GetCurrentState() const
{
	return( CurrentState );
}
//--------------------------------------------------------------------------------
