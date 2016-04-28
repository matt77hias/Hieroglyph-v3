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
