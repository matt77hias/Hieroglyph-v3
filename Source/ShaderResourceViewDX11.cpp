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
#include "ShaderResourceViewDX11.h"
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
ShaderResourceViewDX11::ShaderResourceViewDX11( ShaderResourceViewComPtr pView )
{
	m_pShaderResourceView = pView;
}
//--------------------------------------------------------------------------------
ShaderResourceViewDX11::~ShaderResourceViewDX11()
{
}
//--------------------------------------------------------------------------------
ID3D11ShaderResourceView* ShaderResourceViewDX11::GetSRV()
{
	return( m_pShaderResourceView.Get() );
}
//--------------------------------------------------------------------------------
