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
#include "MatrixArrayParameterDX11.h"
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
MatrixArrayParameterDX11::MatrixArrayParameterDX11( int count )
{
	if ( count < 1 )
		count = 1;

	m_iMatrixCount = count; 
	m_pMatrices = new Matrix4f[count];
}
//--------------------------------------------------------------------------------
MatrixArrayParameterDX11::MatrixArrayParameterDX11( MatrixArrayParameterDX11& copy )
{
	if ( this->m_iMatrixCount != copy.m_iMatrixCount )
	{
		delete [] m_pMatrices;
		m_pMatrices = new Matrix4f[copy.m_iMatrixCount];
		m_iMatrixCount = copy.m_iMatrixCount;
	}

	memcpy( m_pMatrices, copy.m_pMatrices, m_iMatrixCount * sizeof( Matrix4f) );
}
//--------------------------------------------------------------------------------
MatrixArrayParameterDX11& MatrixArrayParameterDX11::operator=( MatrixArrayParameterDX11& parameter )
{
	if ( this->m_iMatrixCount != parameter.m_iMatrixCount )
	{
		delete [] m_pMatrices;
		m_pMatrices = new Matrix4f[parameter.m_iMatrixCount];
		m_iMatrixCount = parameter.m_iMatrixCount;
	}

	memcpy( m_pMatrices, parameter.m_pMatrices, m_iMatrixCount * sizeof( Matrix4f) );

   return *this;  // Assignment operator returns left side.
}
//--------------------------------------------------------------------------------
MatrixArrayParameterDX11::~MatrixArrayParameterDX11()
{
	delete [] m_pMatrices;
}
//--------------------------------------------------------------------------------
void MatrixArrayParameterDX11::SetParameterData( void* pData )
{
	// TODO: This isn't very safe - the caller could supply less than the correct 
	//       amount of matrices...  I need a better way to set this parameter data.

	memcpy( m_pMatrices, pData, m_iMatrixCount * sizeof( Matrix4f ) );
	//m_Matrix = *reinterpret_cast<Matrix4f*>( pData );
}
//--------------------------------------------------------------------------------
ParameterType MatrixArrayParameterDX11::GetParameterType()
{
	return( MATRIX_ARRAY );
}
//--------------------------------------------------------------------------------
int MatrixArrayParameterDX11::GetMatrixCount()
{
	return( m_iMatrixCount );
}
//--------------------------------------------------------------------------------
Matrix4f* MatrixArrayParameterDX11::GetMatrices()
{
	return( m_pMatrices );
}
//--------------------------------------------------------------------------------
void MatrixArrayParameterDX11::UpdateValue( RenderParameterDX11* pParameter )
{
	if ( pParameter )
	{
		if ( ( pParameter->GetParameterType() == MATRIX_ARRAY ) && ( pParameter->GetName() == this->GetName() ) )
		{
			MatrixArrayParameterDX11* pBuffer = (MatrixArrayParameterDX11*)pParameter;
			if ( this->GetMatrixCount() == pBuffer->GetMatrixCount() )
			{
				memcpy( (void*)m_pMatrices, (void*)pBuffer->GetMatrices(), m_iMatrixCount * sizeof( Matrix4f ) );
			}
		}
	}
}
//--------------------------------------------------------------------------------