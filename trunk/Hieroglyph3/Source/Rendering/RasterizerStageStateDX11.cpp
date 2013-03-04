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
#include "RasterizerStageStateDX11.h"
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
RasterizerStageStateDX11::RasterizerStageStateDX11() :
	RasterizerState( -1 ),
	ViewportCount( 0 ),
	Viewports( -1 ),
	ScissorRectCount( 0 ),
	ScissorRects( D3D11_RECT() ),
	m_FeatureLevel( D3D_FEATURE_LEVEL_9_1 ),
	m_pSisterState( nullptr )
{
	ClearState();
}
//--------------------------------------------------------------------------------
RasterizerStageStateDX11::~RasterizerStageStateDX11()
{
	
}
//--------------------------------------------------------------------------------
void RasterizerStageStateDX11::SetFeautureLevel( D3D_FEATURE_LEVEL level )
{
	m_FeatureLevel = level;
}
//--------------------------------------------------------------------------------
//void RasterizerStageStateDX11::SetRasterizerState( int state )
//{
//	// Update the desired state.
//
//	RasterizerState = state;
//
//	// Test if an update is needed.
//
//	if ( nullptr != m_pSisterState ) {
//		if ( m_pSisterState->RasterizerState != RasterizerState ) {
//			m_bUpdateRasterizerState = true;
//		}
//	}
//}
////--------------------------------------------------------------------------------
//void RasterizerStageStateDX11::SetViewportCount( int count )
//{
//	ViewportCount = count;
//}
////--------------------------------------------------------------------------------
//void RasterizerStageStateDX11::SetViewport( unsigned int slot, int viewport )
//{
//	if ( slot < D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE ) {
//		
//		// Update the desired state.
//		
//		Viewports[slot] = viewport;
//
//		// Test if an update is needed.
//
//		if ( nullptr != m_pSisterState ) {
//			if ( m_pSisterState->Viewports[slot] != Viewports[slot] ) {
//				m_bUpdateViewportState = true;
//			}
//		}
//	}
//}
////--------------------------------------------------------------------------------
//void RasterizerStageStateDX11::SetScissorRectCount( int count )
//{
//	ScissorRectCount = count;
//}
////--------------------------------------------------------------------------------
//void RasterizerStageStateDX11::SetScissorRect( unsigned int slot, D3D11_RECT& rect )
//{
//	if ( slot < D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE ) {
//
//		// Update the desired state.
//
//		ScissorRects[slot] = rect;
//
//		// Test if an update is needed.
//
//		if ( nullptr != m_pSisterState ) {
//			if ( m_pSisterState->ScissorRects[slot] != ScissorRects[slot] ) {
//				m_bUpdateScissorRectState = true;
//			}
//		}
//	}
//}
//--------------------------------------------------------------------------------
void RasterizerStageStateDX11::ClearState()
{
	RasterizerState.InitializeState();
	ViewportCount.InitializeState();
	Viewports.InitializeStates();
	ScissorRectCount.InitializeState();
	ScissorRects.InitializeStates();		
}
//--------------------------------------------------------------------------------
void RasterizerStageStateDX11::SetSisterState( RasterizerStageStateDX11* pState )
{
	m_pSisterState = pState;

	RasterizerState.SetSister( &m_pSisterState->RasterizerState );
	ViewportCount.SetSister( &m_pSisterState->ViewportCount );
	Viewports.SetSister( &m_pSisterState->Viewports );
	ScissorRectCount.SetSister( &m_pSisterState->ScissorRectCount );
	ScissorRects.SetSister( &m_pSisterState->ScissorRects );		
}
//--------------------------------------------------------------------------------
void RasterizerStageStateDX11::ResetUpdateFlags( )
{
	RasterizerState.ResetTracking();
	ViewportCount.ResetTracking();
	Viewports.ResetTracking();
	ScissorRectCount.ResetTracking();
	ScissorRects.ResetTracking();		
}
//--------------------------------------------------------------------------------
//int RasterizerStageStateDX11::GetRasterizerState() const
//{
//	return( RasterizerState );
//}
////--------------------------------------------------------------------------------
//int RasterizerStageStateDX11::GetViewportCount() const
//{
//	return( ViewportCount );
//}
////--------------------------------------------------------------------------------
//int RasterizerStageStateDX11::GetViewport( UINT slot ) const
//{
//	int viewport = -1;
//
//	if ( slot < D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE ) {
//		viewport = Viewports[slot];
//	}
//
//	return( viewport );
//}
////--------------------------------------------------------------------------------
//int RasterizerStageStateDX11::GetScissorRectCount() const
//{
//	return( ScissorRectCount );
//}
////--------------------------------------------------------------------------------
//D3D11_RECT RasterizerStageStateDX11::GetScissorRect( UINT slot ) const
//{
//	D3D11_RECT rect;
//	rect.bottom = rect.top = rect.right = rect.left = 0;
//
//	if ( slot < D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE ) {
//		rect = ScissorRects[slot];
//	}
//
//	return( rect );
//}
////--------------------------------------------------------------------------------
//int RasterizerStageStateDX11::CompareRasterizerState( RasterizerStageStateDX11& desired )
//{
//	if ( RasterizerState == desired.RasterizerState )
//		return( 0 );
//	else
//		return( 1 );
//}
////--------------------------------------------------------------------------------
//int RasterizerStageStateDX11::CompareViewportState( RasterizerStageStateDX11& desired )
//{
//	// If any viewport within the desired viewport count is detected, then
//	// the entire array must be set.  Otherwise no setting is needed.
//
//	int viewports = 0;
//
//	for ( int i = 0; i < desired.ViewportCount; i++ ) {
//		if ( Viewports[i] != desired.Viewports[i] ) {
//			viewports = desired.ViewportCount; // return the number of viewports to set
//			break;
//		}
//	}
//
//	return( viewports );	
//}
////--------------------------------------------------------------------------------
//int RasterizerStageStateDX11::CompareScissorRectState( RasterizerStageStateDX11& desired )
//{
//	// If any scissor rect within the desired scissor rect count is detected, then
//	// the entire array must be set.  Otherwise no setting is needed.
//
//	int rects = 0;
//
//	for ( int i = 0; i < desired.ScissorRectCount; i++ ) {
//		if ( ScissorRects[i] != desired.ScissorRects[i] ) {
//			rects = desired.ScissorRectCount; // return the number of viewports to set
//			break;
//		}
//	}
//
//	return( rects );	
//}
////--------------------------------------------------------------------------------
