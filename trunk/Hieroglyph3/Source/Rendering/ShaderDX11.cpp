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
#include "ShaderDX11.h"
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
ShaderDX11::ShaderDX11()
{
	pCompiledShader = nullptr;
	m_pReflection = nullptr;
}
//--------------------------------------------------------------------------------
ShaderDX11::~ShaderDX11()
{
	SAFE_RELEASE( pCompiledShader );
}
//--------------------------------------------------------------------------------
void ShaderDX11::SetReflection( ShaderReflectionDX11* pReflection )
{
	m_pReflection = pReflection;
}
//--------------------------------------------------------------------------------
ShaderReflectionDX11* ShaderDX11::GetReflection( )
{
	return( m_pReflection );
}
//--------------------------------------------------------------------------------
