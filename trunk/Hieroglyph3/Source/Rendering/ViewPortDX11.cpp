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
#include "PCH.h"
#include "ViewPortDX11.h"
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
ViewPortDX11::ViewPortDX11( D3D11_VIEWPORT viewport )
{
	m_ViewPort = viewport;
}
//--------------------------------------------------------------------------------
ViewPortDX11::~ViewPortDX11()
{
}
//--------------------------------------------------------------------------------
