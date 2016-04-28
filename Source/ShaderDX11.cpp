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
ShaderDX11::ShaderDX11() :
	FileName(),
	Function(),
	ShaderModel(),
	ShaderText(),
	m_pCompiledShader( nullptr ),
	m_pReflection( nullptr )
{
}
//--------------------------------------------------------------------------------
ShaderDX11::~ShaderDX11()
{
	SAFE_RELEASE( m_pCompiledShader );
	SAFE_DELETE( m_pReflection );
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
std::wstring ShaderDX11::ToString()
{
	std::wstringstream s;

	s << L"[" << FileName << L"][" << Function << L"][" << ShaderModel << L"]";

	return( s.str() );
}
//--------------------------------------------------------------------------------
