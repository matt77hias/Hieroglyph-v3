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
#include "ResourceDX11.h"
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
unsigned short ResourceDX11::s_usResourceUID = 0;
//--------------------------------------------------------------------------------
ResourceDX11::ResourceDX11( )
{
	m_usInnerID = s_usResourceUID;
	s_usResourceUID++;
}
//--------------------------------------------------------------------------------
ResourceDX11::~ResourceDX11()
{
}
//--------------------------------------------------------------------------------
unsigned short ResourceDX11::GetInnerID()
{
	return( m_usInnerID );
}
//--------------------------------------------------------------------------------