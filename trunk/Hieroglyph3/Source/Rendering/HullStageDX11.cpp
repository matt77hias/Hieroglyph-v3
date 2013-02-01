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
#include "HullStageDX11.h"
#include "HullShaderDX11.h"
#include "RendererDX11.h"
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
HullStageDX11::HullStageDX11()
{
}
//--------------------------------------------------------------------------------
HullStageDX11::~HullStageDX11()
{
}
//--------------------------------------------------------------------------------
ShaderType HullStageDX11::GetType()
{
	return( HULL_SHADER );
}
//--------------------------------------------------------------------------------
void HullStageDX11::BindShaderProgram( ID3D11DeviceContext* pContext )
{
	RendererDX11* pRenderer = RendererDX11::Get();
	ShaderDX11* pShaderDX11 = pRenderer->GetShader( DesiredState.m_ShaderIndex );

	ID3D11HullShader* pShader = 0;

	if ( pShaderDX11 ) {
		pShader = reinterpret_cast<HullShaderDX11*>( pShaderDX11 )->m_pHullShader;
	}

	pContext->HSSetShader( pShader, 0, 0 );
}
//--------------------------------------------------------------------------------
void HullStageDX11::BindConstantBuffers( ID3D11DeviceContext* pContext, int count )
{
	pContext->HSSetConstantBuffers( 0, count, DesiredState.ConstantBuffers );
}
//--------------------------------------------------------------------------------
void HullStageDX11::BindSamplerStates( ID3D11DeviceContext* pContext, int count )
{
	pContext->HSSetSamplers( 0, count, DesiredState.SamplerStates );
}
//--------------------------------------------------------------------------------
void HullStageDX11::BindShaderResourceViews( ID3D11DeviceContext* pContext, int count )
{
	pContext->HSSetShaderResources( 0, count, DesiredState.ShaderResourceViews ); 
}
//--------------------------------------------------------------------------------
void HullStageDX11::BindUnorderedAccessViews( ID3D11DeviceContext* pContext, int count )
{
	// Do nothing - the hull shader doesn't support UAV's!
}
//--------------------------------------------------------------------------------


