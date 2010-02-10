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
#include "InputLayoutDX11.h"
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
InputLayoutDX11::InputLayoutDX11( ID3D11InputLayout* pLayout )
{
	m_pInputLayout = pLayout;
}
//--------------------------------------------------------------------------------
InputLayoutDX11::~InputLayoutDX11()
{
	SAFE_RELEASE( m_pInputLayout );
}
//--------------------------------------------------------------------------------
