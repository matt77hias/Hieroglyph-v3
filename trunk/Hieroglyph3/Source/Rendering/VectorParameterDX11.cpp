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
#include "VectorParameterDX11.h"
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
VectorParameterDX11::VectorParameterDX11()
{
	m_Vector.MakeZero();
}
//--------------------------------------------------------------------------------
VectorParameterDX11::VectorParameterDX11( VectorParameterDX11& copy )
{
	m_Vector = copy.m_Vector;
}
//--------------------------------------------------------------------------------
VectorParameterDX11::~VectorParameterDX11()
{
}
//--------------------------------------------------------------------------------
void VectorParameterDX11::SetParameterData( void* pData )
{
	m_Vector = *reinterpret_cast<Vector4f*>( pData );
}
//--------------------------------------------------------------------------------
ParameterType VectorParameterDX11::GetParameterType()
{
	return( VECTOR );
}
//--------------------------------------------------------------------------------
Vector4f VectorParameterDX11::GetVector()
{
	return( m_Vector );
}
//--------------------------------------------------------------------------------
void VectorParameterDX11::SetVector( Vector4f v )
{
	m_Vector = v;
}
//--------------------------------------------------------------------------------
void VectorParameterDX11::UpdateValue( RenderParameterDX11* pParameter )
{
	if ( pParameter )
	{
		if ( ( pParameter->GetParameterType() == VECTOR ) && ( pParameter->GetName() == this->GetName() ) )
		{
			VectorParameterDX11* pVector = (VectorParameterDX11*)pParameter;
			m_Vector = pVector->GetVector();
		}
	}
}
//--------------------------------------------------------------------------------