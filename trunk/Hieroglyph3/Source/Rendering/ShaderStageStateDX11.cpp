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
ShaderStageStateDX11::ShaderStageStateDX11()
{
	m_FeatureLevel = D3D_FEATURE_LEVEL_9_1;

	m_pSisterState = nullptr;

	m_bUpdateShaderIndex = false;
	m_bUpdateConstantBuffers = false;
	m_bUpdateSamplerStates = false;
	m_bUpdateSRVs = false;
	m_bUpdateUAVs = false;
	m_bUpdateUAVCounts = false;

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
	SetShaderProgram( -1 );

	for ( int i = 0; i < D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT; i++ ) SetConstantBuffer( i, nullptr );
	for ( int i = 0; i < D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT; i++ ) SetSamplerState( i, nullptr );
	for ( int i = 0; i < D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT; i++ ) SetShaderResourceView( i, nullptr );
	for ( int i = 0; i < D3D11_PS_CS_UAV_REGISTER_COUNT; i++ ) SetUnorderedAccessView( i, nullptr, -1 );
}
//--------------------------------------------------------------------------------
void ShaderStageStateDX11::SetShaderProgram( int index )
{
	m_ShaderIndex = index;

	// Check to see if the sister state matches or not.  Then indicate it doesn't
	// so that we can update the API state later on.  Notice that the check can
	// only set the update flag, but can't reset it.  This is because many states
	// have multiple parts to them that may or may not be checked in this 
	// comparison.  In this case, if a desired state is set and then reset to 
	// match the current state, then it would still be set in the API.

	if ( nullptr != m_pSisterState ) {
		if ( m_pSisterState->m_ShaderIndex != m_ShaderIndex ) {
			m_bUpdateShaderIndex = true;
		}
	}
}
//--------------------------------------------------------------------------------
int ShaderStageStateDX11::GetShaderProgram()
{
	return( m_ShaderIndex );
}
//--------------------------------------------------------------------------------
void ShaderStageStateDX11::SetConstantBuffer( int index, ID3D11Buffer* pBuffer )
{
	if ( ( index >= 0 ) && ( index < D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT ) ) {
		
		// Update the desired state.
		
		ConstantBuffers[index] = pBuffer;

		// Test if an update is needed.

		if ( nullptr != m_pSisterState ) {
			if ( m_pSisterState->ConstantBuffers[index] != ConstantBuffers[index] ) {
				m_bUpdateConstantBuffers = true;
			}
		}
	}
}
//--------------------------------------------------------------------------------
void ShaderStageStateDX11::SetSamplerState( int index, ID3D11SamplerState* pState )
{
	if ( ( index >= 0 ) && ( index < D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT ) ) {
		
		// Update the desired state.
		
		SamplerStates[index] = pState;

		// Test if an update is needed.

		if ( nullptr != m_pSisterState ) {
			if ( m_pSisterState->SamplerStates[index] != SamplerStates[index] ) {
				m_bUpdateSamplerStates = true;
			}
		}
	}
}
//--------------------------------------------------------------------------------
void ShaderStageStateDX11::SetShaderResourceView( int index, ID3D11ShaderResourceView* pSRV )
{
	if ( ( index >= 0 ) && ( index < D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT ) ) {

		// Update the desired state.

		ShaderResourceViews[index] = pSRV;

		// Test if an update is needed.

		if ( nullptr != m_pSisterState ) {
			if ( m_pSisterState->ShaderResourceViews[index] != ShaderResourceViews[index] ) {
				m_bUpdateSRVs = true;
			}
		}
	}
}
//--------------------------------------------------------------------------------
void ShaderStageStateDX11::SetUnorderedAccessView( int index, ID3D11UnorderedAccessView* pUAV, unsigned int initial )
{
	if ( ( index >= 0 ) && ( index < D3D11_PS_CS_UAV_REGISTER_COUNT ) ) {

		// Update the desired state.

		UnorderedAccessViews[index] = pUAV;
		UAVInitCounts[index] = initial;

		// Test if an update is needed.

		if ( nullptr != m_pSisterState ) {
			if ( m_pSisterState->UnorderedAccessViews[index] != UnorderedAccessViews[index] 
				|| m_pSisterState->UAVInitCounts[index] != UAVInitCounts[index] ) {
				m_bUpdateUAVs = true;
			}
		}
	}
}
//--------------------------------------------------------------------------------
void ShaderStageStateDX11::SetSisterState( ShaderStageStateDX11* pState )
{
	m_pSisterState = pState;
}
//--------------------------------------------------------------------------------
void ShaderStageStateDX11::ResetUpdateFlags( )
{
	m_bUpdateShaderIndex = false;
	m_bUpdateConstantBuffers = false;
	m_bUpdateSamplerStates = false;
	m_bUpdateSRVs = false;
	m_bUpdateUAVs = false;
	m_bUpdateUAVCounts = false;
}
//--------------------------------------------------------------------------------
int ShaderStageStateDX11::CompareShaderProgramState( ShaderStageStateDX11& desired )
{
	if ( m_ShaderIndex == desired.m_ShaderIndex )
		return( 0 );
	else
		return( 1 );
}
//--------------------------------------------------------------------------------
int ShaderStageStateDX11::CompareConstantBufferState( ShaderStageStateDX11& desired )
{
	int count = 0;

	for ( int i = D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - 1; i >= 0; i-- ) {
		if ( ConstantBuffers[i] != desired.ConstantBuffers[i] ) {
			count = i+1; // return the number of buffers to set
			break;
		}
	}

	return( count );
}
//--------------------------------------------------------------------------------
int ShaderStageStateDX11::CompareSamplerStateState( ShaderStageStateDX11& desired )
{
	int count = 0;

	for ( int i = D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT - 1; i >= 0; i-- ) {
		if ( SamplerStates[i] != desired.SamplerStates[i] ) {
			count = i+1; // return the number of samplers to set
			break;
		}
	}

	return( count );
}
//--------------------------------------------------------------------------------
int ShaderStageStateDX11::CompareShaderResourceViewState( ShaderStageStateDX11& desired )
{
	int count = 0;

	for ( int i = D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT - 1; i >= 0; i-- ) {
		if ( ShaderResourceViews[i] != desired.ShaderResourceViews[i] ) {
			count = i+1; // return the number of SRVs to set
			break;
		}
	}

	return( count );
}
//--------------------------------------------------------------------------------
int ShaderStageStateDX11::CompareUnorderedAccessViewState( ShaderStageStateDX11& desired )
{
	int count = 0;

	for ( int i = D3D11_PS_CS_UAV_REGISTER_COUNT - 1; i >= 0; i-- ) {
		if ( UnorderedAccessViews[i] != desired.UnorderedAccessViews[i]
			|| UAVInitCounts[i] != desired.UAVInitCounts[i] ) {
			count = i+1; // return the number of UAVs to set
			break;
		}
	}

	return( count );
}
//--------------------------------------------------------------------------------