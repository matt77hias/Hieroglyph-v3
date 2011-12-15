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
#include "OutputMergerStageStateDX11.h"
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
OutputMergerStageStateDX11::OutputMergerStageStateDX11()
{
	m_FeatureLevel = D3D_FEATURE_LEVEL_9_1;

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
void OutputMergerStageStateDX11::SetBlendState( int state )
{
	BlendState = state;
}
//--------------------------------------------------------------------------------
void OutputMergerStageStateDX11::SetDepthStencilState( int state, unsigned int stencilRef )
{
	DepthStencilState = state;
	StencilRef = stencilRef;
}
//--------------------------------------------------------------------------------
void OutputMergerStageStateDX11::SetRenderTarget( unsigned int slot, int rtv )
{
	if ( slot < D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT ) {
		RenderTargetViews[slot] = rtv;
	}
}
//--------------------------------------------------------------------------------
void OutputMergerStageStateDX11::SetDepthStencilTarget( int dsv )
{
	DepthTargetViews = dsv;
}
//--------------------------------------------------------------------------------
void OutputMergerStageStateDX11::SetUnorderedAccessView( unsigned int slot, int uav, unsigned int initCount )
{
	if ( slot < D3D11_PS_CS_UAV_REGISTER_COUNT )
	{
		UnorderedAccessViews[slot] = uav;
		UAVInitialCounts[slot] = initCount;
	}
}
//--------------------------------------------------------------------------------
int OutputMergerStageStateDX11::GetBlendState() const
{
	return( BlendState );
}
//--------------------------------------------------------------------------------
int OutputMergerStageStateDX11::GetDepthStencilState() const
{
	return( DepthStencilState );
}
//--------------------------------------------------------------------------------
unsigned int OutputMergerStageStateDX11::GetStencilReference() const
{
	return( StencilRef );
}
//--------------------------------------------------------------------------------
int OutputMergerStageStateDX11::GetRenderTarget( unsigned int slot ) const
{
	int result = -1;

	if ( slot < D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT ) {
		result = RenderTargetViews[slot];
	}

	return( result );
}
//--------------------------------------------------------------------------------
int OutputMergerStageStateDX11::GetDepthStencilTarget( ) const
{
	return( DepthTargetViews );
}
//--------------------------------------------------------------------------------
int OutputMergerStageStateDX11::GetUnorderedAccessView( unsigned int slot ) const
{
	int result = -1;

	if ( slot < D3D11_PS_CS_UAV_REGISTER_COUNT ) {
		result = UnorderedAccessViews[slot];
	}

	return( result );
}
//--------------------------------------------------------------------------------
int OutputMergerStageStateDX11::GetInitialCount( unsigned int slot ) const
{
	int result = -1;

	if ( slot < D3D11_PS_CS_UAV_REGISTER_COUNT ) {
		result = UAVInitialCounts[slot];
	}

	return( result );
}
//--------------------------------------------------------------------------------
int OutputMergerStageStateDX11::GetRenderTargetCount() const
{
	unsigned int count = 0;

	for ( int i = 0; i < D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT; i++ )
	{
		if ( RenderTargetViews[i] != -1 )
			count++;
	}

	return( count );
}
//--------------------------------------------------------------------------------
int OutputMergerStageStateDX11::CompareBlendState( OutputMergerStageStateDX11& desired )
{
	if ( BlendState == desired.BlendState )
		return( 0 );
	else
		return( 1 );
}
//--------------------------------------------------------------------------------
int OutputMergerStageStateDX11::CompareDepthStencilState( OutputMergerStageStateDX11& desired )
{
	if ( DepthStencilState == desired.DepthStencilState && StencilRef == desired.StencilRef )
		return( 0 );
	else
		return( 1 );
}
//--------------------------------------------------------------------------------
int OutputMergerStageStateDX11::CompareRenderTargets( OutputMergerStageStateDX11& desired )
{
	int count = 0;
	for ( int i = D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT-1; i >= 0; i-- )
	{
		if ( RenderTargetViews[i] != desired.RenderTargetViews[i] )
		{
			count = i+1;
			break;
		}
	}

	return( count );
}
//--------------------------------------------------------------------------------
int OutputMergerStageStateDX11::CompareUnorderedAccessViews( OutputMergerStageStateDX11& desired )
{
	int count = 0;
	for ( int i = D3D11_PS_CS_UAV_REGISTER_COUNT-1; i >= 0; i-- )
	{
		if ( UnorderedAccessViews[i] != desired.UnorderedAccessViews[i]
			|| UAVInitialCounts[i] != desired.UAVInitialCounts[i] )
		{
			count = i+1;
			break;
		}
	}

	return( count );
}
//--------------------------------------------------------------------------------
void OutputMergerStageStateDX11::ClearState( )
{
	BlendState = -1;
	DepthStencilState = -1;
	StencilRef = 0;

	for ( int i = 0; i < D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT; i++ ) {
		RenderTargetViews[i] = -1;
	}

	DepthTargetViews = -1;
	
	for ( int i = 0; i < D3D11_PS_CS_UAV_REGISTER_COUNT; i++ ) {
		UnorderedAccessViews[i] = -1;
		UAVInitialCounts[i] = -1;
	}

}
//--------------------------------------------------------------------------------
