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
#include "ISharedObject.h"
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
ISharedObject::ISharedObject( )
{
	m_iRefCount = 0;
}
//--------------------------------------------------------------------------------
ISharedObject::~ISharedObject( )
{

}
//--------------------------------------------------------------------------------
void ISharedObject::AddReference()
{
	m_iRefCount++;
}
//--------------------------------------------------------------------------------
void ISharedObject::Release()
{
	m_iRefCount--;

	if ( m_iRefCount <= 0 )
		delete this;
}
//--------------------------------------------------------------------------------