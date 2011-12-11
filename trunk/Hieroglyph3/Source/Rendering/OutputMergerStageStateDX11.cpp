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
void OutputMergerStageStateDX11::SetRasterizerState( int state )
{
	RasterizerState = state;
}
//--------------------------------------------------------------------------------
void OutputMergerStageStateDX11::SetViewportCount( int count )
{
	ViewportCount = count;
}
//--------------------------------------------------------------------------------
void OutputMergerStageStateDX11::SetViewport( unsigned int slot, int viewport )
{
	if ( slot < D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE ) {
		Viewports[slot] = viewport;
	}
}
//--------------------------------------------------------------------------------
void OutputMergerStageStateDX11::SetScissorRectCount( int count )
{
	ScissorRectCount = count;
}
//--------------------------------------------------------------------------------
void OutputMergerStageStateDX11::SetScissorRect( unsigned int slot, D3D11_RECT& rect )
{
	if ( slot < D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE ) {
		ScissorRects[slot] = rect;
	}
}
//--------------------------------------------------------------------------------
void OutputMergerStageStateDX11::ClearState()
{
	RasterizerState = -1;
	ViewportCount = 0;

	for ( int i = 0; i < D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE; i++ ) {
		Viewports[i] = -1;
		ScissorRects[i].bottom = ScissorRects[i].top = ScissorRects[i].right = ScissorRects[i].left = 0;		
	}
}
//--------------------------------------------------------------------------------
int OutputMergerStageStateDX11::GetRasterizerState() const
{
	return( RasterizerState );
}
//--------------------------------------------------------------------------------
int OutputMergerStageStateDX11::GetViewportCount() const
{
	return( ViewportCount );
}
//--------------------------------------------------------------------------------
int OutputMergerStageStateDX11::GetViewport( UINT slot ) const
{
	int viewport = -1;

	if ( slot < D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE ) {
		viewport = Viewports[slot];
	}

	return( viewport );
}
//--------------------------------------------------------------------------------
int OutputMergerStageStateDX11::GetScissorRectCount() const
{
	return( ScissorRectCount );
}
//--------------------------------------------------------------------------------
D3D11_RECT OutputMergerStageStateDX11::GetScissorRect( UINT slot ) const
{
	D3D11_RECT rect;
	rect.bottom = rect.top = rect.right = rect.left = 0;

	if ( slot < D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE ) {
		rect = ScissorRects[slot];
	}

	return( rect );
}
//--------------------------------------------------------------------------------
int OutputMergerStageStateDX11::CompareRasterizerState( OutputMergerStageStateDX11& desired )
{
	if ( RasterizerState == desired.RasterizerState )
		return( 0 );
	else
		return( 1 );
}
//--------------------------------------------------------------------------------
int OutputMergerStageStateDX11::CompareViewportState( OutputMergerStageStateDX11& desired )
{
	// If any viewport within the desired viewport count is detected, then
	// the entire array must be set.  Otherwise no setting is needed.

	int viewports = 0;

	for ( int i = 0; i < desired.ViewportCount; i++ ) {
		if ( Viewports[i] != desired.Viewports[i] ) {
			viewports = desired.ViewportCount; // return the number of viewports to set
			break;
		}
	}

	return( viewports );	
}
//--------------------------------------------------------------------------------
int OutputMergerStageStateDX11::CompareScissorRectState( OutputMergerStageStateDX11& desired )
{
	// If any scissor rect within the desired scissor rect count is detected, then
	// the entire array must be set.  Otherwise no setting is needed.

	int rects = 0;

	for ( int i = 0; i < desired.ScissorRectCount; i++ ) {
		if ( ScissorRects[i] != desired.ScissorRects[i] ) {
			rects = desired.ScissorRectCount; // return the number of viewports to set
			break;
		}
	}

	return( rects );	
}
//--------------------------------------------------------------------------------
