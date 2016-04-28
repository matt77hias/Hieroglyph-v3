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
#include "MatrixArrayParameterDX11.h"
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
MatrixArrayParameterDX11::MatrixArrayParameterDX11( int count )
{
	if ( count < 1 )
		count = 1;

	m_iMatrixCount = count; 

	for ( int i = 0; i <= NUM_THREADS; i++ )
		m_pMatrices[i] = new Matrix4f[count];
}
//--------------------------------------------------------------------------------
MatrixArrayParameterDX11::MatrixArrayParameterDX11( MatrixArrayParameterDX11& copy )
{
	if ( this->m_iMatrixCount != copy.m_iMatrixCount )
	{
		for ( int i = 0; i <= NUM_THREADS; i++ )
		{
			delete [] m_pMatrices[i];
			m_pMatrices[i] = new Matrix4f[copy.m_iMatrixCount];
		}
		m_iMatrixCount = copy.m_iMatrixCount;
	}

	for ( int i = 0; i <= NUM_THREADS; i++ )
		memcpy( m_pMatrices[i], copy.m_pMatrices[i], m_iMatrixCount * sizeof( Matrix4f) );
}
//--------------------------------------------------------------------------------
MatrixArrayParameterDX11& MatrixArrayParameterDX11::operator=( MatrixArrayParameterDX11& parameter )
{
	if ( this->m_iMatrixCount != parameter.m_iMatrixCount )
	{
		for ( int i = 0; i <= NUM_THREADS; i++ )
		{
			delete [] m_pMatrices[i];
			m_pMatrices[i] = new Matrix4f[parameter.m_iMatrixCount];
		}
		m_iMatrixCount = parameter.m_iMatrixCount;
	}

	for ( int i = 0; i <= NUM_THREADS; i++ )
		memcpy( m_pMatrices[i], parameter.m_pMatrices[i], m_iMatrixCount * sizeof( Matrix4f) );

   return *this;  // Assignment operator returns left side.
}
//--------------------------------------------------------------------------------
MatrixArrayParameterDX11::~MatrixArrayParameterDX11()
{
	for ( int i = 0; i <= NUM_THREADS; i++ )
		delete [] m_pMatrices[i];
}
//--------------------------------------------------------------------------------
void MatrixArrayParameterDX11::SetParameterData( void* pData, unsigned int threadID )
{
	assert( threadID >= 0 );
	assert( threadID < NUM_THREADS+1 );

	// TODO: This isn't very safe - the caller could supply less than the correct 
	//       amount of matrices...  I need a better way to set this parameter data.

	if ( 0 != memcmp( pData, &(m_pMatrices[threadID]), m_iMatrixCount * sizeof( Matrix4f ) ) ) {
		m_auiValueID[threadID]++;
		memcpy( m_pMatrices[threadID], pData, m_iMatrixCount * sizeof( Matrix4f ) );
	}
	
}
//--------------------------------------------------------------------------------
//void MatrixArrayParameterDX11::ResetParameterData( void* pData, unsigned int threadID )
//{
//	assert( threadID >= 0 );
//	assert( threadID < NUM_THREADS+1 );
//
//	// TODO: This method doesn't do anything at the moment - it doesn't make sense
//	//       to reset a resource that just holds data (this mechanism is intended
//	//       for resource parameters...).
//
//	//m_auiValueID[threadID]++;
//	//memcpy( m_pMatrices[threadID], pData, m_iMatrixCount * sizeof( Matrix4f ) );
//	//m_Matrix = *reinterpret_cast<Matrix4f*>( pData );
//}
//--------------------------------------------------------------------------------
const ParameterType MatrixArrayParameterDX11::GetParameterType()
{
	return( MATRIX_ARRAY );
}
//--------------------------------------------------------------------------------
int MatrixArrayParameterDX11::GetMatrixCount()
{
	return( m_iMatrixCount );
}
//--------------------------------------------------------------------------------
Matrix4f* MatrixArrayParameterDX11::GetMatrices( unsigned int threadID )
{
	assert( threadID >= 0 );
	assert( threadID < NUM_THREADS+1 );

	return( m_pMatrices[threadID] );
}
//--------------------------------------------------------------------------------
//void MatrixArrayParameterDX11::UpdateValue( RenderParameterDX11* pParameter, unsigned int threadID )
//{
//	if ( pParameter )
//	{
//		if ( ( pParameter->GetParameterType() == MATRIX_ARRAY ) && ( pParameter->GetName() == this->GetName() ) )
//		{
//			MatrixArrayParameterDX11* pBuffer = (MatrixArrayParameterDX11*)pParameter;
//			if ( this->GetMatrixCount() == pBuffer->GetMatrixCount() )
//			{
//				if ( 0 != memcmp( pBuffer->GetMatrices( threadID ), &(m_pMatrices[threadID]), m_iMatrixCount * sizeof( Matrix4f ) ) ) {
//					m_auiValueID[threadID]++;
//					memcpy( (void*)m_pMatrices, (void*)pBuffer->GetMatrices( threadID ), m_iMatrixCount * sizeof( Matrix4f ) );
//				}
//			}
//		}
//	}
//}
//--------------------------------------------------------------------------------