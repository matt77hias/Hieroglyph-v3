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
#include "Actor.h"
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
Actor::Actor()
{
	m_pRoot = new Node3D();
	m_pBody = new Entity3D();
	m_pRoot->AttachChild( m_pBody );
}
//--------------------------------------------------------------------------------
Actor::~Actor()
{
	delete m_pRoot;
	delete m_pBody;
}
//--------------------------------------------------------------------------------
Node3D* Actor::GetNode()
{
	return( m_pRoot );
}
//--------------------------------------------------------------------------------
Entity3D* Actor::GetBody()
{
	return( m_pBody );
}
//--------------------------------------------------------------------------------
