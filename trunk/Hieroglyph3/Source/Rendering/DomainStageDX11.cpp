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
#include "DomainStageDX11.h"
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
void DomainStageDX11::BindConstantBuffers( ID3D11DeviceContext* pContext, int count )
{
	pContext->DSSetConstantBuffers( 0, count, ConstantBuffers );
}
//--------------------------------------------------------------------------------
void DomainStageDX11::BindSamplerStates( ID3D11DeviceContext* pContext, int count )
{
	pContext->DSSetSamplers( 0, count, SamplerStates );
}
//--------------------------------------------------------------------------------
void DomainStageDX11::BindShaderResourceViews( ID3D11DeviceContext* pContext, int count )
{
	pContext->DSSetShaderResources( 0, count, ShaderResourceViews ); 
}
//--------------------------------------------------------------------------------
void DomainStageDX11::BindUnorderedAccessViews( ID3D11DeviceContext* pContext, int count )
{
	// Do nothing - the geometry shader doesn't support UAV's!
}
//--------------------------------------------------------------------------------

