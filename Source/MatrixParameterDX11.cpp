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
#include "MatrixParameterDX11.h"
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
MatrixParameterDX11::MatrixParameterDX11()
{
	for ( int i = 0; i <= NUM_THREADS; i++ )
		m_Matrix[i].MakeIdentity();
}
//--------------------------------------------------------------------------------
MatrixParameterDX11::MatrixParameterDX11( MatrixParameterDX11& copy )
{
	for ( int i = 0; i <= NUM_THREADS; i++ )
		m_Matrix[i] = copy.m_Matrix[i];
}
//--------------------------------------------------------------------------------
MatrixParameterDX11::~MatrixParameterDX11()
{
}
//--------------------------------------------------------------------------------
void MatrixParameterDX11::SetParameterData( void* pData, unsigned int threadID )
{
	assert( threadID >= 0 );
	assert( threadID < NUM_THREADS+1 );

	if ( 0 != memcmp( pData, &(m_Matrix[threadID]), sizeof(Matrix4f) ) ) {
		m_auiValueID[threadID]++;
		m_Matrix[threadID] = *reinterpret_cast<Matrix4f*>( pData );
	}
}
//--------------------------------------------------------------------------------
//void MatrixParameterDX11::ResetParameterData( void* pData, unsigned int threadID )
//{
//	assert( threadID >= 0 );
//	assert( threadID < NUM_THREADS+1 );
//
//	if ( m_Matrix[threadID] == *reinterpret_cast<Matrix4f*>( pData ) ) {
//		m_auiValueID[threadID]++;
//		m_Matrix[threadID].MakeIdentity();
//	}
//}
//--------------------------------------------------------------------------------
const ParameterType MatrixParameterDX11::GetParameterType()
{
	return( MATRIX );
}
//--------------------------------------------------------------------------------
Matrix4f MatrixParameterDX11::GetMatrix( unsigned int threadID )
{
	assert( threadID >= 0 );
	assert( threadID < NUM_THREADS+1 );

	return( m_Matrix[threadID] );
}
//--------------------------------------------------------------------------------
//void MatrixParameterDX11::UpdateValue( RenderParameterDX11* pParameter, unsigned int threadID )
//{
//	assert( threadID >= 0 );
//	assert( threadID < NUM_THREADS+1 );
//
//	if ( pParameter )
//	{
//		if ( ( pParameter->GetParameterType() == MATRIX ) && ( pParameter->GetName() == this->GetName() ) )
//		{
//			MatrixParameterDX11* pBuffer = (MatrixParameterDX11*)pParameter;
//
//			if ( m_Matrix[threadID] != pBuffer->GetMatrix( threadID ) ) {
//				m_auiValueID[threadID]++;
//				m_Matrix[threadID] = pBuffer->GetMatrix( threadID );
//			}
//		}
//	}
//}
//--------------------------------------------------------------------------------