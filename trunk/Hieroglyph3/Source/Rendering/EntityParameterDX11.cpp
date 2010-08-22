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
#include "EntityParameterDX11.h"
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
EntityParameterDX11::EntityParameterDX11()
{
	m_pEntity = 0;
}
//--------------------------------------------------------------------------------
EntityParameterDX11::EntityParameterDX11( EntityParameterDX11& copy )
{
	m_pEntity = copy.m_pEntity;
}
//--------------------------------------------------------------------------------
EntityParameterDX11::~EntityParameterDX11()
{
}
//--------------------------------------------------------------------------------
void EntityParameterDX11::SetParameterData( void* pData )
{
	m_pEntity = reinterpret_cast<Entity3D*>( pData );
}
//--------------------------------------------------------------------------------
ParameterType EntityParameterDX11::GetParameterType()
{
	return( ENTITY );
}
//--------------------------------------------------------------------------------
Entity3D* EntityParameterDX11::GetEntity()
{
	return( m_pEntity );
}
//--------------------------------------------------------------------------------
void EntityParameterDX11::UpdateValue( RenderParameterDX11* pParameter )
{
	if ( pParameter )
	{
		if ( ( pParameter->GetParameterType() == ENTITY ) && ( pParameter->GetName() == this->GetName() ) )
		{
			EntityParameterDX11* pBuffer = (EntityParameterDX11*)pParameter;
			m_pEntity = pBuffer->GetEntity();
		}
	}
}
//--------------------------------------------------------------------------------
