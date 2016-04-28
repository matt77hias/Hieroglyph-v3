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
#include "ComputeStageDX11.h"
#include "ComputeShaderDX11.h"
#include "RendererDX11.h"
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
ComputeStageDX11::ComputeStageDX11()
{
}
//--------------------------------------------------------------------------------
ComputeStageDX11::~ComputeStageDX11()
{
}
//--------------------------------------------------------------------------------
ShaderType ComputeStageDX11::GetType()
{
	return( COMPUTE_SHADER );
}
//--------------------------------------------------------------------------------
void ComputeStageDX11::BindShaderProgram( ID3D11DeviceContext* pContext )
{
	RendererDX11* pRenderer = RendererDX11::Get();
	ShaderDX11* pShaderDX11 = pRenderer->GetShader( DesiredState.ShaderProgram.GetState() );

	ID3D11ComputeShader* pShader = 0;
	
	if ( pShaderDX11 ) {
		pShader = reinterpret_cast<ComputeShaderDX11*>( pShaderDX11 )->m_pComputeShader;
	}

	pContext->CSSetShader( pShader, 0, 0 );
}
//--------------------------------------------------------------------------------
void ComputeStageDX11::BindConstantBuffers( ID3D11DeviceContext* pContext, int count )
{
	pContext->CSSetConstantBuffers( 
		DesiredState.ConstantBuffers.GetStartSlot(),
		DesiredState.ConstantBuffers.GetRange(),
		DesiredState.ConstantBuffers.GetFirstSlotLocation() );
}
//--------------------------------------------------------------------------------
void ComputeStageDX11::BindSamplerStates( ID3D11DeviceContext* pContext, int count )
{
	pContext->CSSetSamplers( 
		DesiredState.SamplerStates.GetStartSlot(),
		DesiredState.SamplerStates.GetRange(),
		DesiredState.SamplerStates.GetFirstSlotLocation() );
}
//--------------------------------------------------------------------------------
void ComputeStageDX11::BindShaderResourceViews( ID3D11DeviceContext* pContext, int count )
{
	pContext->CSSetShaderResources( 
		DesiredState.ShaderResourceViews.GetStartSlot(),
		DesiredState.ShaderResourceViews.GetRange(),
		DesiredState.ShaderResourceViews.GetFirstSlotLocation() ); 
}
//--------------------------------------------------------------------------------
void ComputeStageDX11::BindUnorderedAccessViews( ID3D11DeviceContext* pContext, int count )
{
	// Here we need to get the start and end slots from both the UAV states and the 
	// UAV initial counts, and take the superset of those to ensure that all of the
	// UAV states are accounted for.

	unsigned int minStartSlot = 
		min( DesiredState.UnorderedAccessViews.GetStartSlot(),
		DesiredState.UAVInitialCounts.GetStartSlot() );

	unsigned int maxEndSlot =
		max( DesiredState.UnorderedAccessViews.GetEndSlot(),
		DesiredState.UAVInitialCounts.GetEndSlot() );

	pContext->CSSetUnorderedAccessViews( 
		minStartSlot,
		maxEndSlot - minStartSlot + 1,
		DesiredState.UnorderedAccessViews.GetSlotLocation( minStartSlot ),
		DesiredState.UAVInitialCounts.GetSlotLocation( minStartSlot ) );
}
//--------------------------------------------------------------------------------


