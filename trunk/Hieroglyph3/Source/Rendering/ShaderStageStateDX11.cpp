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
#include "ShaderStageStateDX11.h"
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
ShaderStageStateDX11::ShaderStageStateDX11()
{
	m_FeatureLevel = D3D_FEATURE_LEVEL_9_1;

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
	m_ShaderIndex = -1;

	for ( int i = 0; i < D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT; i++ ) ConstantBuffers[i] = 0;
	for ( int i = 0; i < D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT; i++ ) SamplerStates[i] = 0;
	for ( int i = 0; i < D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT; i++ ) ShaderResourceViews[i] = 0;
	for ( int i = 0; i < D3D11_PS_CS_UAV_REGISTER_COUNT; i++ ) UnorderedAccessViews[i] = 0;
	for ( int i = 0; i < D3D11_PS_CS_UAV_REGISTER_COUNT; i++ ) UAVInitCounts[i] = -1;
}
//--------------------------------------------------------------------------------
void ShaderStageStateDX11::SetShaderProgram( int index )
{
	m_ShaderIndex = index;
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
		ConstantBuffers[index] = pBuffer;
	}
}
//--------------------------------------------------------------------------------
void ShaderStageStateDX11::SetSamplerState( int index, ID3D11SamplerState* pState )
{
	if ( ( index >= 0 ) && ( index < D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT ) ) {
		SamplerStates[index] = pState;
	}
}
//--------------------------------------------------------------------------------
void ShaderStageStateDX11::SetShaderResourceView( int index, ID3D11ShaderResourceView* pSRV )
{
	if ( ( index >= 0 ) && ( index < D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT ) ) {
		ShaderResourceViews[index] = pSRV;
	}
}
//--------------------------------------------------------------------------------
void ShaderStageStateDX11::SetUnorderedAccessView( int index, ID3D11UnorderedAccessView* pUAV, unsigned int initial )
{
	if ( ( index >= 0 ) && ( index < D3D11_PS_CS_UAV_REGISTER_COUNT ) ) {
		UnorderedAccessViews[index] = pUAV;
		UAVInitCounts[index] = initial;
	}
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