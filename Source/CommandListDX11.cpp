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
#include "CommandListDX11.h"
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
CommandListDX11::CommandListDX11( )
{
	m_pList = 0;
}
//--------------------------------------------------------------------------------
bool CommandListDX11::ListAvailable()
{
	return( m_pList != 0 );
}
//--------------------------------------------------------------------------------
void CommandListDX11::ReleaseList()
{
	SAFE_RELEASE( m_pList );
}
//--------------------------------------------------------------------------------
CommandListDX11::~CommandListDX11()
{
	SAFE_RELEASE( m_pList );
}
//--------------------------------------------------------------------------------
