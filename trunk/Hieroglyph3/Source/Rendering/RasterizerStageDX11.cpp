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
#include "RasterizerStageDX11.h"
#include "ViewPortDX11.h"
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
RasterizerStageDX11::RasterizerStageDX11()
{
	// Link the two states together to monitor their changes.
	DesiredState.SetSisterState( &CurrentState );
}
//--------------------------------------------------------------------------------
RasterizerStageDX11::~RasterizerStageDX11()
{
	
}
//--------------------------------------------------------------------------------
void RasterizerStageDX11::SetFeatureLevel( D3D_FEATURE_LEVEL level )
{
	m_FeatureLevel = level;
	CurrentState.SetFeautureLevel( level );
	DesiredState.SetFeautureLevel( level );
}
//--------------------------------------------------------------------------------
void RasterizerStageDX11::ClearDesiredState()
{
	DesiredState.ClearState();
}
//--------------------------------------------------------------------------------
void RasterizerStageDX11::ClearCurrentState()
{
	CurrentState.ClearState();
}
//--------------------------------------------------------------------------------
const RasterizerStageStateDX11& RasterizerStageDX11::GetCurrentState() const
{
	return( CurrentState );
}
//--------------------------------------------------------------------------------
void RasterizerStageDX11::ApplyDesiredState( ID3D11DeviceContext* pContext )
{
	RendererDX11* pRenderer = RendererDX11::Get();

	// Compare the current state vs. the desired state and set it if necesary.
	if ( true == DesiredState.m_bUpdateRasterizerState ) {
		
		RasterizerStatePtr pGlyphRasterizerState = pRenderer->GetRasterizerState( DesiredState.GetRasterizerState() );
		
		if ( pGlyphRasterizerState != nullptr ) {
			ID3D11RasterizerState* pRasterizerState = pGlyphRasterizerState->m_pState;
			pContext->RSSetState( pRasterizerState );
		}
	}

	// Compare the viewport state and set it if necesary.
	int viewports = 0;
	
	if ( true == DesiredState.m_bUpdateViewportState ) {
		
		D3D11_VIEWPORT aViewports[D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE];
		
		for ( int i = 0; i < D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE; i++ ) {

			ViewPortDX11* pViewport = pRenderer->GetViewPort( DesiredState.Viewports[i] );
			
			if ( pViewport ) {
				aViewports[i] = pViewport->m_ViewPort; 
				viewports++;
			}
		}

		pContext->RSSetViewports( viewports, aViewports );
	}


	// Compare the scissor rect state and set it if necesary.
	
	if ( true == DesiredState.m_bUpdateScissorRectState ) {
		
		D3D11_RECT aRects[D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE];
		
		for ( int i = 0; i < D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE; i++ ) {
			aRects[i] = DesiredState.ScissorRects[i]; 
		}

		pContext->RSSetScissorRects( D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE, aRects );
	}

	DesiredState.ResetUpdateFlags();
	CurrentState = DesiredState;
}
//--------------------------------------------------------------------------------
