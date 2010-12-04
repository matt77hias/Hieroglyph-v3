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
#include "ComputeStageDX11.h"
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
void ComputeStageDX11::BindConstantBuffers( ID3D11DeviceContext* pContext, int count )
{
	pContext->CSSetConstantBuffers( 0, count, ConstantBuffers );
}
//--------------------------------------------------------------------------------
void ComputeStageDX11::BindSamplerStates( ID3D11DeviceContext* pContext, int count )
{
	pContext->CSSetSamplers( 0, count, SamplerStates );
}
//--------------------------------------------------------------------------------
void ComputeStageDX11::BindShaderResourceViews( ID3D11DeviceContext* pContext, int count )
{
	pContext->CSSetShaderResources( 0, count, ShaderResourceViews ); 
}
//--------------------------------------------------------------------------------
void ComputeStageDX11::BindUnorderedAccessViews( ID3D11DeviceContext* pContext, int count )
{
	pContext->CSSetUnorderedAccessViews( 0, count, UnorderedAccessViews, UAVInitCounts /*(UINT*)&UnorderedAccessViews*/ );
}
//--------------------------------------------------------------------------------


