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
#include "ShaderStageStateDX11.h"
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
ShaderStageStateDX11::ShaderStageStateDX11() : 
	ShaderProgram( -1 ),
	ConstantBuffers( nullptr ),
	SamplerStates( nullptr ),
	ShaderResourceViews( nullptr ),
	UnorderedAccessViews( nullptr ),
	UAVInitialCounts( -1 ),
	m_FeatureLevel( D3D_FEATURE_LEVEL_9_1 ),
	m_pSisterState( nullptr )
{
	ClearState();
}
//--------------------------------------------------------------------------------
ShaderStageStateDX11::~ShaderStageStateDX11()
{
}
//--------------------------------------------------------------------------------
void ShaderStageStateDX11::SetFeautureLevel( D3D_FEATURE_LEVEL level )
{
	m_FeatureLevel = level;
}
//--------------------------------------------------------------------------------
void ShaderStageStateDX11::ClearState()
{
	ShaderProgram.InitializeState();
	ConstantBuffers.InitializeStates();
	SamplerStates.InitializeStates();
	ShaderResourceViews.InitializeStates();
	UnorderedAccessViews.InitializeStates();
	UAVInitialCounts.InitializeStates();
}
//--------------------------------------------------------------------------------
void ShaderStageStateDX11::SetSisterState( ShaderStageStateDX11* pState )
{
	m_pSisterState = pState;
	ShaderProgram.SetSister( &m_pSisterState->ShaderProgram );
	ConstantBuffers.SetSister( &m_pSisterState->ConstantBuffers );
	SamplerStates.SetSister( &m_pSisterState->SamplerStates );
	ShaderResourceViews.SetSister( &m_pSisterState->ShaderResourceViews );
	UnorderedAccessViews.SetSister( &m_pSisterState->UnorderedAccessViews );
	UAVInitialCounts.SetSister( &m_pSisterState->UAVInitialCounts );
}
//--------------------------------------------------------------------------------
void ShaderStageStateDX11::ResetUpdateFlags( )
{
	ShaderProgram.ResetTracking();
	ConstantBuffers.ResetTracking();
	SamplerStates.ResetTracking();
	ShaderResourceViews.ResetTracking();
	UnorderedAccessViews.ResetTracking();
	UAVInitialCounts.ResetTracking();
}
//--------------------------------------------------------------------------------