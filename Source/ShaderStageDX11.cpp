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
#include "ShaderStageDX11.h"
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
static void SetToNull( void* pArray, int num )
{
    const int ptrSize = sizeof( void* );
    memset( pArray, 0, num * ptrSize );
}
//--------------------------------------------------------------------------------
ShaderStageDX11::ShaderStageDX11()
{
	// Link the two states together to monitor their changes.
	DesiredState.SetSisterState( &CurrentState );
}
//--------------------------------------------------------------------------------
ShaderStageDX11::~ShaderStageDX11()
{
	
}
//--------------------------------------------------------------------------------
void ShaderStageDX11::SetFeatureLevel( D3D_FEATURE_LEVEL level )
{
	m_FeatureLevel = level;
}
//--------------------------------------------------------------------------------
void ShaderStageDX11::ClearDesiredState( )
{
	DesiredState.ClearState();
}
//--------------------------------------------------------------------------------
void ShaderStageDX11::ClearCurrentState( )
{
	CurrentState.ClearState();
}
//--------------------------------------------------------------------------------
void ShaderStageDX11::ApplyDesiredState( ID3D11DeviceContext* pContext )
{
	// TODO: remove the 'count' arguments from the Bind* methods, since the number
	//       is tracked internally by the state itself.

	// Compare the current shader vs. the desired shader and set it if necesary.
	if ( DesiredState.ShaderProgram.IsUpdateNeeded() ) {
		BindShaderProgram( pContext );
	}

	// Compare the constant buffer state and set it if necesary.
	if ( DesiredState.ConstantBuffers.IsUpdateNeeded() ) {
		BindConstantBuffers( pContext, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT-1 );
	}

	// Compare the sampler states and set them if necesary.
	if ( DesiredState.SamplerStates.IsUpdateNeeded() ) {
		BindSamplerStates( pContext, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT-1 );
	}

	// Compare the shader resource view states and set them if necesary.
	if ( DesiredState.ShaderResourceViews.IsUpdateNeeded() ) {
		BindShaderResourceViews( pContext, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT-1 ); 
	}

	// Compare the unordered access view states and set them if necesary.
	if ( DesiredState.UnorderedAccessViews.IsUpdateNeeded() 
		|| DesiredState.UAVInitialCounts.IsUpdateNeeded() ) {
		if ( m_FeatureLevel != D3D_FEATURE_LEVEL_11_0 )
			BindUnorderedAccessViews( pContext, 1 );
		else
			BindUnorderedAccessViews( pContext, D3D11_PS_CS_UAV_REGISTER_COUNT-1 );
	}

	// After binding everything, set the current state to the desired state.
	
	DesiredState.ResetUpdateFlags();
	CurrentState = DesiredState;
}
//--------------------------------------------------------------------------------
