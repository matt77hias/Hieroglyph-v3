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
#include "Log.h"
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
OutputMergerStageDX11::OutputMergerStageDX11()
{
	for ( int i = 0; i < D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT; i++ )
	{
		RenderTargetViews[i] = 0;
		APIRenderTargetViews[i] = 0;
	}

	DepthTargetViews = 0;
	for ( int i = 0; i < D3D11_PS_CS_UAV_REGISTER_COUNT; i++ ) UnorderedAccessViews[i] = 0;
	for ( int i = 0; i < D3D11_PS_CS_UAV_REGISTER_COUNT; i++ ) UAVInitialCounts[i] = -1;
    APIDepthTargetViews = NULL;
}
//--------------------------------------------------------------------------------
OutputMergerStageDX11::~OutputMergerStageDX11()
{
	
}
//--------------------------------------------------------------------------------
void OutputMergerStageDX11::SetFeautureLevel( D3D_FEATURE_LEVEL level )
{
	m_FeatureLevel = level;
}
//--------------------------------------------------------------------------------
void OutputMergerStageDX11::BindRenderTarget( int index, ResourcePtr Target )
{
	int RenderID = Target->m_iResourceRTV;

	RendererDX11* pRenderer = RendererDX11::Get();
	
	RenderTargetViewDX11* pView = pRenderer->GetRenderTargetView( RenderID );

	if ( pView )
	{
		ID3D11RenderTargetView* pRenderTarget = { pView->m_pRenderTargetView };

		if ( ( index >= 0 ) && ( index < D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT ) )
		{
			RenderTargetViews[index] = pRenderTarget;
		}
	}
	else
	{
		Log::Get().Write( L"Tried to bind an invalid render target view!" );
	}
}
//--------------------------------------------------------------------------------
void OutputMergerStageDX11::BindDepthTarget( ResourcePtr DepthTarget )
{
	int DepthID = DepthTarget->m_iResourceDSV;

	RendererDX11* pRenderer = RendererDX11::Get();
	
	DepthStencilViewDX11* pView = pRenderer->GetDepthStencilView( DepthID );

	if ( pView )
	{
		ID3D11DepthStencilView* pDepthStencilView = pView->m_pDepthStencilView;

		DepthTargetViews = pDepthStencilView;
	}
	else
	{
		Log::Get().Write( L"Tried to bind an invalid depth stencil view!" );
	}
}
//--------------------------------------------------------------------------------
void OutputMergerStageDX11::SetUnorderedAccessView( int index, ID3D11UnorderedAccessView* pUAV, unsigned int initial )
{
	if ( ( index >= 0 ) && ( index < D3D11_PS_CS_UAV_REGISTER_COUNT ) )
	{
		UnorderedAccessViews[index] = pUAV;
		UAVInitialCounts[index] = initial;
	}
}
//--------------------------------------------------------------------------------
void OutputMergerStageDX11::BindResources( ID3D11DeviceContext* pContext )
{
	// Update all of the render targets and depth stencil targets
	//pContext->OMSetRenderTargetsAndUnorderedAccessViews( 8, RenderTargetViews, DepthTargetViews, 
	//	0, D3D11_PS_CS_UAV_REGISTER_COUNT, UnorderedAccessViews, (UINT*)&UnorderedAccessViews );
	
	// Find the highest index that doesn't match.
	int max = 0;
	for ( int i = D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT-1; i >= 0; i-- )
	{
		if ( RenderTargetViews[i] != APIRenderTargetViews[i] )
		{
			max = i+1;
			break;
		}
	}

    if ( DepthTargetViews != APIDepthTargetViews )
        max++;

	// If any targets are different then copy them over.
	if ( max > 0 )
		pContext->OMSetRenderTargets( D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT, RenderTargetViews, DepthTargetViews );
	
	// Update the API views to know what to update next time.
	for ( int i = 0; i < D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT; i++ )
		APIRenderTargetViews[i] = RenderTargetViews[i];

    APIDepthTargetViews = DepthTargetViews;
}
//--------------------------------------------------------------------------------
void OutputMergerStageDX11::SetToDefaultState()
{
	memset( APIRenderTargetViews, 0, D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT * sizeof( APIRenderTargetViews[0] ) );
}
//--------------------------------------------------------------------------------
void OutputMergerStageDX11::ClearResources()
{
	// Clear out all array elements in our cached arrays.  This will be used to 
	// write nulls into the context later on.

	memset( RenderTargetViews, 0, D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT * sizeof( RenderTargetViews[0] ) );
	DepthTargetViews = 0;
	memset( UnorderedAccessViews, 0, sizeof( UnorderedAccessViews[0] ) * D3D11_PS_CS_UAV_REGISTER_COUNT );
	for ( int i = 0; i < D3D11_PS_CS_UAV_REGISTER_COUNT; i++ ) UAVInitialCounts[i] = -1;
}
//--------------------------------------------------------------------------------
void OutputMergerStageDX11::UnbindResources( ID3D11DeviceContext* pContext )
{
	// Clear out the resource settings.

	ClearResources();

	// Bind the changes to the pipeline.

	BindResources( pContext );
}
//--------------------------------------------------------------------------------
unsigned int OutputMergerStageDX11::GetViewsSetCount()
{
	unsigned int count = 0;

	for ( int i = 0; i < D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT; i++ )
	{
		if ( RenderTargetViews[i] != 0 )
			count++;
	}

	return( count );
}
//--------------------------------------------------------------------------------
unsigned int OutputMergerStageDX11::GetViewsBoundCount()
{
	unsigned int count = 0;

	for ( int i = 0; i < D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT; i++ )
	{
		if ( APIRenderTargetViews[i] != 0 )
			count++;
	}

	return( count );
}
//--------------------------------------------------------------------------------