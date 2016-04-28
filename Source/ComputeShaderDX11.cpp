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
#include "ComputeShaderDX11.h"
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
ComputeShaderDX11::ComputeShaderDX11( ID3D11ComputeShader* pShader )
{
	m_pComputeShader = pShader;
}
//--------------------------------------------------------------------------------
ComputeShaderDX11::~ComputeShaderDX11()
{
	SAFE_RELEASE( m_pComputeShader );
}
//--------------------------------------------------------------------------------
ShaderType ComputeShaderDX11::GetType()
{
	return( COMPUTE_SHADER );
}
//--------------------------------------------------------------------------------