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
	m_FeatureLevel = D3D_FEATURE_LEVEL_9_1;
	m_ShaderIndex = -1;

	for ( int i = 0; i < D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT; i++ ) ConstantBuffers[i] = 0;
	for ( int i = 0; i < D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT; i++ ) SamplerStates[i] = 0;
	for ( int i = 0; i < D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT; i++ ) ShaderResourceViews[i] = 0;
	for ( int i = 0; i < D3D11_PS_CS_UAV_REGISTER_COUNT; i++ ) UnorderedAccessViews[i] = 0;
	for ( int i = 0; i < D3D11_PS_CS_UAV_REGISTER_COUNT; i++ ) UAVInitCounts[i] = -1;

	iCurrMaxCB = D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - 1;
	iNextMaxCB = 0;
	iCurrMaxSS = D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT - 1;
	iNextMaxSS = 0;
	iCurrMaxSRV = D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT - 1;
	iNextMaxSRV = 0;
	iCurrMaxUAV = D3D11_PS_CS_UAV_REGISTER_COUNT - 1;
	iNextMaxUAV = 0;

	bBuffersDirty = true;
	bSamplersDirty = true;
	bShaderResourceViewsDirty = true;
	bUnorderedAccessViewsDirty = true; 	
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
void ShaderStageDX11::SetConstantBuffer( int index, ID3D11Buffer* pBuffer )
{
	if ( ( index >= 0 ) && ( index < D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT ) )
	{
		ConstantBuffers[index] = pBuffer;
		if ( index > iNextMaxCB ) iNextMaxCB = index;
		bBuffersDirty = true;
	}
}
//--------------------------------------------------------------------------------
void ShaderStageDX11::SetSamplerState( int index, ID3D11SamplerState* pState )
{
	if ( ( index >= 0 ) && ( index < D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT ) )
	{
		SamplerStates[index] = pState;
		if ( index > iNextMaxSS ) iNextMaxSS = index;
		bSamplersDirty = true;
	}
}
//--------------------------------------------------------------------------------
void ShaderStageDX11::SetShaderResourceView( int index, ID3D11ShaderResourceView* pSRV )
{
	if ( ( index >= 0 ) && ( index < D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT ) )
	{
		ShaderResourceViews[index] = pSRV;
		if ( index > iNextMaxSRV ) iNextMaxSRV = index;
		bShaderResourceViewsDirty = true;
	}
}
//--------------------------------------------------------------------------------
void ShaderStageDX11::SetUnorderedAccessView( int index, ID3D11UnorderedAccessView* pUAV, unsigned int initial )
{
	// TODO: Add some member variable to the renderer to indicate the current 
	//       feature level of the device.  This can then be used to limit the
	//       number of unordered access views that can be set at once (i.e. the
	//       number of UAVs on feature level < 11 is limited to 1!

	if ( ( index >= 0 ) && ( index < D3D11_PS_CS_UAV_REGISTER_COUNT ) )
	{
		UnorderedAccessViews[index] = pUAV;
		UAVInitCounts[index] = initial;
		if ( index > iNextMaxUAV ) iNextMaxUAV = index;
		bUnorderedAccessViewsDirty = true;
	}
}
//--------------------------------------------------------------------------------
void ShaderStageDX11::BindResources( ID3D11DeviceContext* pContext )
{
	int index = -1;

	index = iNextMaxCB;
	if ( index < iCurrMaxCB ) index = iCurrMaxCB;

	if ( index >= 0 )
	{
		BindConstantBuffers( pContext, index + 1 );

		// Update the current indices to the 'next' ones, even though you may have
		// set more than iNextMaxCB number of elements.  This allows elements to 
		// be written as nulls, but not affect the element counts afterwards.

		iCurrMaxCB = iNextMaxCB;	
		iNextMaxCB = -1;
        SetToNull( ConstantBuffers, index + 1 );		
	}

	index = iNextMaxSS;
	if ( index < iCurrMaxSS ) index = iCurrMaxSS;

	if ( index >= 0 )
	{
		BindSamplerStates( pContext, index + 1 );

		// Update the current indices to the 'next' ones, even though you may have
		// set more than iNextMaxCB number of elements.  This allows elements to 
		// be written as nulls, but not affect the element counts afterwards.

		iCurrMaxSS = iNextMaxSS;
		iNextMaxSS = -1;
        SetToNull( SamplerStates, index + 1 );		
	}

	index = iNextMaxSRV;
	if ( index < iCurrMaxSRV ) index = iCurrMaxSRV;

	if ( index >= 0 )
	{
		BindShaderResourceViews( pContext, index + 1 ); 

		// Update the current indices to the 'next' ones, even though you may have
		// set more than iNextMaxCB number of elements.  This allows elements to 
		// be written as nulls, but not affect the element counts afterwards.

		iCurrMaxSRV = iNextMaxSRV;
		iNextMaxSRV = -1;
        SetToNull( ShaderResourceViews, index + 1 );		
	}

	index = iNextMaxUAV;
	if ( index < iCurrMaxUAV ) index = iCurrMaxUAV;

	if ( index >= 0 )
	{
		if ( m_FeatureLevel != D3D_FEATURE_LEVEL_11_0 )
			index = 0;

		BindUnorderedAccessViews( pContext, index + 1 );

		// Update the current indices to the 'next' ones, even though you may have
		// set more than iNextMaxXX number of elements.  This allows elements to 
		// be written as nulls, but not affect the element counts afterwards.

		iCurrMaxUAV = iNextMaxUAV;
		iNextMaxUAV = -1;
        SetToNull( UnorderedAccessViews, index + 1 );		
	}
}
//--------------------------------------------------------------------------------
void ShaderStageDX11::UnbindResources( ID3D11DeviceContext* pContext )
{
	// Clear out all array elements in our cached arrays.  This will be used to 
	// write nulls into the context later on.

    SetToNull( ConstantBuffers, iCurrMaxCB + 1 );
	SetToNull( SamplerStates, iCurrMaxSS + 1 );
    SetToNull( ShaderResourceViews, iCurrMaxSRV + 1 );
    SetToNull( UnorderedAccessViews, iCurrMaxUAV + 1 );

	// When all indices are set to null, you only need to set as many elements as 
	// the current settings have (i.e. to clear out all of the currently filled
	// array elements.

	iNextMaxCB = iCurrMaxCB;
	iNextMaxSS = iCurrMaxSS;
	iNextMaxSRV = iCurrMaxSRV;
	iNextMaxUAV = iCurrMaxUAV;

	// Bind the changes to the pipeline.

	BindResources( pContext );
}
//--------------------------------------------------------------------------------
void ShaderStageDX11::SetShaderIndex( int index )
{
	m_ShaderIndex = index;
}
//--------------------------------------------------------------------------------
int ShaderStageDX11::GetShaderIndex()
{
	return( m_ShaderIndex );
}
//--------------------------------------------------------------------------------