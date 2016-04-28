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
#include "GeometryShaderDX11.h"
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
GeometryShaderDX11::GeometryShaderDX11( ID3D11GeometryShader* pShader )
{
	m_pGeometryShader = pShader;
}
//--------------------------------------------------------------------------------
GeometryShaderDX11::~GeometryShaderDX11()
{
	SAFE_RELEASE( m_pGeometryShader );
}
//--------------------------------------------------------------------------------
ShaderType GeometryShaderDX11::GetType()
{
	return( GEOMETRY_SHADER );
}
//--------------------------------------------------------------------------------