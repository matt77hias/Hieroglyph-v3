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
#include "DomainStageDX11.h"
#include "DomainShaderDX11.h"
#include "RendererDX11.h"
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
DomainStageDX11::DomainStageDX11()
{
}
//--------------------------------------------------------------------------------
DomainStageDX11::~DomainStageDX11()
{
}
//--------------------------------------------------------------------------------
ShaderType DomainStageDX11::GetType()
{
	return( DOMAIN_SHADER );
}
//--------------------------------------------------------------------------------
void DomainStageDX11::BindShaderProgram( ID3D11DeviceContext* pContext )
{
	RendererDX11* pRenderer = RendererDX11::Get();
	ShaderDX11* pShaderDX11 = pRenderer->GetShader( DesiredState.ShaderProgram.GetState() );

	ID3D11DomainShader* pShader = 0;
	
	if ( pShaderDX11 ) {
		pShader = reinterpret_cast<DomainShaderDX11*>( pShaderDX11 )->m_pDomainShader;
	}

	pContext->DSSetShader( pShader, 0, 0 );
}
//--------------------------------------------------------------------------------
void DomainStageDX11::BindConstantBuffers( ID3D11DeviceContext* pContext, int count )
{
	pContext->DSSetConstantBuffers( 
		DesiredState.ConstantBuffers.GetStartSlot(),
		DesiredState.ConstantBuffers.GetRange(),
		DesiredState.ConstantBuffers.GetFirstSlotLocation() );
}
//--------------------------------------------------------------------------------
void DomainStageDX11::BindSamplerStates( ID3D11DeviceContext* pContext, int count )
{
	pContext->DSSetSamplers( 
		DesiredState.SamplerStates.GetStartSlot(),
		DesiredState.SamplerStates.GetRange(),
		DesiredState.SamplerStates.GetFirstSlotLocation() );
}
//--------------------------------------------------------------------------------
void DomainStageDX11::BindShaderResourceViews( ID3D11DeviceContext* pContext, int count )
{
	pContext->DSSetShaderResources( 
		DesiredState.ShaderResourceViews.GetStartSlot(),
		DesiredState.ShaderResourceViews.GetRange(),
		DesiredState.ShaderResourceViews.GetFirstSlotLocation() ); 
}
//--------------------------------------------------------------------------------
void DomainStageDX11::BindUnorderedAccessViews( ID3D11DeviceContext* pContext, int count )
{
	// Do nothing - the geometry shader doesn't support UAV's!
}
//--------------------------------------------------------------------------------

