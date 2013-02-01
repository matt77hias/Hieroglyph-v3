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
	ShaderDX11* pShaderDX11 = pRenderer->GetShader( DesiredState.m_ShaderIndex );

	ID3D11ComputeShader* pShader = 0;
	
	if ( pShaderDX11 ) {
		pShader = reinterpret_cast<ComputeShaderDX11*>( pShaderDX11 )->m_pComputeShader;
	}

	pContext->CSSetShader( pShader, 0, 0 );
}
//--------------------------------------------------------------------------------
void ComputeStageDX11::BindConstantBuffers( ID3D11DeviceContext* pContext, int count )
{
	pContext->CSSetConstantBuffers( 0, count, DesiredState.ConstantBuffers );
}
//--------------------------------------------------------------------------------
void ComputeStageDX11::BindSamplerStates( ID3D11DeviceContext* pContext, int count )
{
	pContext->CSSetSamplers( 0, count, DesiredState.SamplerStates );
}
//--------------------------------------------------------------------------------
void ComputeStageDX11::BindShaderResourceViews( ID3D11DeviceContext* pContext, int count )
{
	pContext->CSSetShaderResources( 0, count, DesiredState.ShaderResourceViews ); 
}
//--------------------------------------------------------------------------------
void ComputeStageDX11::BindUnorderedAccessViews( ID3D11DeviceContext* pContext, int count )
{
	pContext->CSSetUnorderedAccessViews( 0, count, DesiredState.UnorderedAccessViews, DesiredState.UAVInitCounts );
}
//--------------------------------------------------------------------------------


