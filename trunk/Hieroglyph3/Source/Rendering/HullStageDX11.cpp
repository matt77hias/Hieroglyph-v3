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
#include "HullStageDX11.h"
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
void HullStageDX11::BindConstantBuffers( ID3D11DeviceContext* pContext, int count )
{
	pContext->HSSetConstantBuffers( 0, count, ConstantBuffers );
}
//--------------------------------------------------------------------------------
void HullStageDX11::BindSamplerStates( ID3D11DeviceContext* pContext, int count )
{
	pContext->HSSetSamplers( 0, count, SamplerStates );
}
//--------------------------------------------------------------------------------
void HullStageDX11::BindShaderResourceViews( ID3D11DeviceContext* pContext, int count )
{
	pContext->HSSetShaderResources( 0, count, ShaderResourceViews ); 
}
//--------------------------------------------------------------------------------
void HullStageDX11::BindUnorderedAccessViews( ID3D11DeviceContext* pContext, int count )
{
	// Do nothing - the hull shader doesn't support UAV's!
}
//--------------------------------------------------------------------------------


