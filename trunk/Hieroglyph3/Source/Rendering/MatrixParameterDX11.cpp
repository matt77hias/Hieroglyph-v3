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
#include "MatrixParameterDX11.h"
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
MatrixParameterDX11::MatrixParameterDX11()
{
	m_Matrix.MakeIdentity();
}
//--------------------------------------------------------------------------------
MatrixParameterDX11::MatrixParameterDX11( MatrixParameterDX11& copy )
{
	m_Matrix = copy.m_Matrix;
}
//--------------------------------------------------------------------------------
MatrixParameterDX11::~MatrixParameterDX11()
{
}
//--------------------------------------------------------------------------------
void MatrixParameterDX11::SetParameterData( void* pData )
{
	m_Matrix = *reinterpret_cast<Matrix4f*>( pData );
}
//--------------------------------------------------------------------------------
ParameterType MatrixParameterDX11::GetParameterType()
{
	return( MATRIX );
}
//--------------------------------------------------------------------------------
Matrix4f MatrixParameterDX11::GetMatrix()
{
	return( m_Matrix );
}
//--------------------------------------------------------------------------------
void MatrixParameterDX11::UpdateValue( RenderParameterDX11* pParameter )
{
	if ( pParameter )
	{
		if ( ( pParameter->GetParameterType() == MATRIX ) && ( pParameter->GetName() == this->GetName() ) )
		{
			MatrixParameterDX11* pBuffer = (MatrixParameterDX11*)pParameter;
			m_Matrix = pBuffer->GetMatrix();
		}
	}
}
//--------------------------------------------------------------------------------