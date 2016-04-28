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
#include "VertexShaderDX11.h"
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
VertexShaderDX11::VertexShaderDX11( ID3D11VertexShader* pShader )
{
	m_pVertexShader = pShader;
}
//--------------------------------------------------------------------------------
VertexShaderDX11::~VertexShaderDX11()
{
	SAFE_RELEASE( m_pVertexShader );
}
//--------------------------------------------------------------------------------
ShaderType VertexShaderDX11::GetType()
{
	return( VERTEX_SHADER );
}
//--------------------------------------------------------------------------------