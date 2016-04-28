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
#include "HullShaderDX11.h"
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
HullShaderDX11::HullShaderDX11( ID3D11HullShader* pShader )
{
	m_pHullShader = pShader;
}
//--------------------------------------------------------------------------------
HullShaderDX11::~HullShaderDX11()
{
	SAFE_RELEASE( m_pHullShader );
}
//--------------------------------------------------------------------------------
ShaderType HullShaderDX11::GetType()
{
	return( HULL_SHADER );
}
//--------------------------------------------------------------------------------