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
#include "VectorParameterDX11.h"
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
VectorParameterDX11::VectorParameterDX11()
{
	for ( int i = 0; i <= NUM_THREADS; i++ )
		m_Vector[i].MakeZero();
}
//--------------------------------------------------------------------------------
VectorParameterDX11::VectorParameterDX11( VectorParameterDX11& copy )
{
	for ( int i = 0; i <= NUM_THREADS; i++ )
		m_Vector[i] = copy.m_Vector[i];
}
//--------------------------------------------------------------------------------
VectorParameterDX11::~VectorParameterDX11()
{
}
//--------------------------------------------------------------------------------
void VectorParameterDX11::SetParameterData( void* pData, unsigned int threadID )
{
	assert( threadID >= 0 );
	assert( threadID < NUM_THREADS+1 );

	if ( 0 != memcmp( pData, &(m_Vector[threadID]), sizeof(Vector4f) ) ) {
		m_auiValueID[threadID]++;
		m_Vector[threadID] = *reinterpret_cast<Vector4f*>( pData );
	}
}
//--------------------------------------------------------------------------------
//void VectorParameterDX11::ResetParameterData( void* pData, unsigned int threadID )
//{
//	assert( threadID >= 0 );
//	assert( threadID < NUM_THREADS+1 );
//
//	if ( m_Vector[threadID] == *reinterpret_cast<Vector4f*>( pData ) ) {
//		m_auiValueID[threadID]++;
//		m_Vector[threadID].MakeZero();
//	}
//}
//--------------------------------------------------------------------------------
const ParameterType VectorParameterDX11::GetParameterType()
{
	return( VECTOR );
}
//--------------------------------------------------------------------------------
Vector4f VectorParameterDX11::GetVector( unsigned int threadID )
{
	assert( threadID >= 0 );
	assert( threadID < NUM_THREADS+1 );

	return( m_Vector[threadID] );
}
//--------------------------------------------------------------------------------
void VectorParameterDX11::SetVector( Vector4f v, unsigned int threadID )
{
	if ( v != m_Vector[threadID] ) {
		m_auiValueID[threadID]++;
		m_Vector[threadID] = v;
	}
}
//--------------------------------------------------------------------------------
//void VectorParameterDX11::UpdateValue( RenderParameterDX11* pParameter, unsigned int threadID )
//{
//	assert( threadID >= 0 );
//	assert( threadID < NUM_THREADS+1 );
//
//	if ( pParameter )
//	{
//		if ( ( pParameter->GetParameterType() == VECTOR ) && ( pParameter->GetName() == this->GetName() ) )
//		{
//			VectorParameterDX11* pVector = (VectorParameterDX11*)pParameter;
//			if ( m_Vector[threadID] != pVector->GetVector( threadID ) ) {
//				m_auiValueID[threadID]++;
//				m_Vector[threadID] = pVector->GetVector();
//			}
//		}
//	}
//}
//--------------------------------------------------------------------------------