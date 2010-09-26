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
#include "ShaderResourceParameterDX11.h"
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
ShaderResourceParameterDX11::ShaderResourceParameterDX11()
{
	m_iShaderResourceView = -1;
}
//--------------------------------------------------------------------------------
ShaderResourceParameterDX11::ShaderResourceParameterDX11( ShaderResourceParameterDX11& copy )
{
	m_iShaderResourceView = copy.m_iShaderResourceView;
}
//--------------------------------------------------------------------------------
ShaderResourceParameterDX11::~ShaderResourceParameterDX11()
{
}
//--------------------------------------------------------------------------------
void ShaderResourceParameterDX11::SetParameterData( void* pData )
{
	m_iShaderResourceView = *reinterpret_cast<int*>( pData );
}
//--------------------------------------------------------------------------------
ParameterType ShaderResourceParameterDX11::GetParameterType()
{
	return( SHADER_RESOURCE );
}
//--------------------------------------------------------------------------------
int ShaderResourceParameterDX11::GetIndex()
{
	return( m_iShaderResourceView );
}
//--------------------------------------------------------------------------------
void ShaderResourceParameterDX11::UpdateValue( RenderParameterDX11* pParameter )
{
	if ( pParameter )
	{
		if ( ( pParameter->GetParameterType() == SHADER_RESOURCE ) && ( pParameter->GetName() == this->GetName() ) )
		{
			ShaderResourceParameterDX11* pBuffer = (ShaderResourceParameterDX11*)pParameter;
			m_iShaderResourceView = pBuffer->GetIndex();
		}
	}
}
//--------------------------------------------------------------------------------