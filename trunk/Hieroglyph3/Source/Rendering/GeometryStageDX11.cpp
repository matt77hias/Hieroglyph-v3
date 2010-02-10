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
#include "GeometryStageDX11.h"
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
GeometryStageDX11::GeometryStageDX11()
{
}
//--------------------------------------------------------------------------------
GeometryStageDX11::~GeometryStageDX11()
{
}
//--------------------------------------------------------------------------------
ShaderType GeometryStageDX11::GetType()
{
	return( GEOMETRY_SHADER );
}
//--------------------------------------------------------------------------------
void GeometryStageDX11::BindConstantBuffers( ID3D11DeviceContext* pContext, int count )
{
	pContext->GSSetConstantBuffers( 0, count, ConstantBuffers );
}
//--------------------------------------------------------------------------------
void GeometryStageDX11::BindSamplerStates( ID3D11DeviceContext* pContext, int count )
{
	pContext->GSSetSamplers( 0, count, SamplerStates );
}
//--------------------------------------------------------------------------------
void GeometryStageDX11::BindShaderResourceViews( ID3D11DeviceContext* pContext, int count )
{
	pContext->GSSetShaderResources( 0, count, ShaderResourceViews ); 
}
//--------------------------------------------------------------------------------
void GeometryStageDX11::BindUnorderedAccessViews( ID3D11DeviceContext* pContext, int count )
{
	// Do nothing - the geometry shader doesn't support UAV's!
}
//--------------------------------------------------------------------------------



