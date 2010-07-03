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
#include "ConstantBufferParameterDX11.h"
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
ConstantBufferParameterDX11::ConstantBufferParameterDX11()
{
	m_iCBuffer = -1;
}
//--------------------------------------------------------------------------------
ConstantBufferParameterDX11::ConstantBufferParameterDX11( ConstantBufferParameterDX11& copy )
{
	m_iCBuffer = copy.m_iCBuffer;
}
//--------------------------------------------------------------------------------
ConstantBufferParameterDX11::~ConstantBufferParameterDX11()
{
}
//--------------------------------------------------------------------------------
void ConstantBufferParameterDX11::SetParameterData( void* pData )
{
	m_iCBuffer = *reinterpret_cast<int*>( pData );
}
//--------------------------------------------------------------------------------
ParameterType ConstantBufferParameterDX11::GetParameterType()
{
	return( CBUFFER );
}
//--------------------------------------------------------------------------------
int ConstantBufferParameterDX11::GetIndex()
{
	return( m_iCBuffer );
}
//--------------------------------------------------------------------------------
void ConstantBufferParameterDX11::UpdateValue( RenderParameterDX11* pParameter )
{
	if ( pParameter )
	{
		if ( ( pParameter->GetParameterType() == CBUFFER ) && ( pParameter->GetName() == this->GetName() ) )
		{
			ConstantBufferParameterDX11* pBuffer = (ConstantBufferParameterDX11*)pParameter;
			m_iCBuffer = pBuffer->GetIndex();
		}
	}
}
//--------------------------------------------------------------------------------