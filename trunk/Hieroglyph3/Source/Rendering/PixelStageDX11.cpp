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
#include "PixelStageDX11.h"
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
PixelStageDX11::PixelStageDX11()
{
}
//--------------------------------------------------------------------------------
PixelStageDX11::~PixelStageDX11()
{
}
//--------------------------------------------------------------------------------
ShaderType PixelStageDX11::GetType()
{
	return( PIXEL_SHADER );
}
//--------------------------------------------------------------------------------
void PixelStageDX11::BindConstantBuffers( ID3D11DeviceContext* pContext, int count )
{
	pContext->PSSetConstantBuffers( 0, count, ConstantBuffers );
}
//--------------------------------------------------------------------------------
void PixelStageDX11::BindSamplerStates( ID3D11DeviceContext* pContext, int count )
{
	pContext->PSSetSamplers( 0, count, SamplerStates );
}
//--------------------------------------------------------------------------------
void PixelStageDX11::BindShaderResourceViews( ID3D11DeviceContext* pContext, int count )
{
	pContext->PSSetShaderResources( 0, count, ShaderResourceViews ); 
}
//--------------------------------------------------------------------------------
void PixelStageDX11::BindUnorderedAccessViews( ID3D11DeviceContext* pContext, int count )
{
	// Do nothing - the pixel shader supports UAV's, but the API isn't clear on how to!
}
//--------------------------------------------------------------------------------

