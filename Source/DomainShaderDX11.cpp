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
#include "DomainShaderDX11.h"
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
DomainShaderDX11::DomainShaderDX11( ID3D11DomainShader* pShader )
{
	m_pDomainShader = pShader;
}
//--------------------------------------------------------------------------------
DomainShaderDX11::~DomainShaderDX11()
{
	SAFE_RELEASE( m_pDomainShader );
}
//--------------------------------------------------------------------------------
ShaderType DomainShaderDX11::GetType()
{
	return( DOMAIN_SHADER );
}
//--------------------------------------------------------------------------------