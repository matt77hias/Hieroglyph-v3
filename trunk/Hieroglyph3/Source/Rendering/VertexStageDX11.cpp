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
#include "VertexStageDX11.h"
#include "VertexShaderDX11.h"
#include "RendererDX11.h"
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
VertexStageDX11::VertexStageDX11()
{
}
//--------------------------------------------------------------------------------
VertexStageDX11::~VertexStageDX11()
{
}
//--------------------------------------------------------------------------------
ShaderType VertexStageDX11::GetType()
{
	return( VERTEX_SHADER );
}
//--------------------------------------------------------------------------------
void VertexStageDX11::BindShaderProgram( ID3D11DeviceContext* pContext )
{
	RendererDX11* pRenderer = RendererDX11::Get();
	ShaderDX11* pShaderDX11 = pRenderer->GetShader( DesiredState.m_ShaderIndex );

	ID3D11VertexShader* pShader = 0;
		
	if ( pShaderDX11 ) {
		pShader = reinterpret_cast<VertexShaderDX11*>( pShaderDX11 )->m_pVertexShader;
	}

	pContext->VSSetShader( pShader, 0, 0 );
}
//--------------------------------------------------------------------------------
void VertexStageDX11::BindConstantBuffers( ID3D11DeviceContext* pContext, int count )
{
	pContext->VSSetConstantBuffers( 0, count, DesiredState.ConstantBuffers );
}
//--------------------------------------------------------------------------------
void VertexStageDX11::BindSamplerStates( ID3D11DeviceContext* pContext, int count )
{
	pContext->VSSetSamplers( 0, count, DesiredState.SamplerStates );
}
//--------------------------------------------------------------------------------
void VertexStageDX11::BindShaderResourceViews( ID3D11DeviceContext* pContext, int count )
{
	pContext->VSSetShaderResources( 0, count, DesiredState.ShaderResourceViews ); 
}
//--------------------------------------------------------------------------------
void VertexStageDX11::BindUnorderedAccessViews( ID3D11DeviceContext* pContext, int count )
{
	// Do nothing - the vertex shader doesn't support UAV's!
}
//--------------------------------------------------------------------------------
