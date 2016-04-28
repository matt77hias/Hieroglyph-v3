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
#include "PixelShaderDX11.h"
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
PixelShaderDX11::PixelShaderDX11( ID3D11PixelShader* pShader )
{
	m_pPixelShader = pShader;
}
//--------------------------------------------------------------------------------
PixelShaderDX11::~PixelShaderDX11()
{
	SAFE_RELEASE( m_pPixelShader );
}
//--------------------------------------------------------------------------------
ShaderType PixelShaderDX11::GetType()
{
	return( PIXEL_SHADER );
}
//--------------------------------------------------------------------------------