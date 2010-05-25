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
#include "IController.h"
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
IController::IController()
{
	m_pEntity = 0;
}
//--------------------------------------------------------------------------------
IController::~IController()
{
}
//--------------------------------------------------------------------------------
void IController::SetEntity( Entity3D* pEntity )
{
	m_pEntity = pEntity;
}
//--------------------------------------------------------------------------------
Entity3D* IController::GetEntity()
{
	return( m_pEntity );
}
//--------------------------------------------------------------------------------