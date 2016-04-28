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
#include "ConstantBufferParameterDX11.h"
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
ConstantBufferParameterDX11::ConstantBufferParameterDX11()
{
	for ( int i = 0; i <= NUM_THREADS; i++ )
		m_iCBuffer[i] = -1;
}
//--------------------------------------------------------------------------------
ConstantBufferParameterDX11::ConstantBufferParameterDX11( ConstantBufferParameterDX11& copy )
{
	for ( int i = 0; i <= NUM_THREADS; i++ )
		m_iCBuffer[i] = copy.m_iCBuffer[i];
}
//--------------------------------------------------------------------------------
ConstantBufferParameterDX11::~ConstantBufferParameterDX11()
{
}
//--------------------------------------------------------------------------------
void ConstantBufferParameterDX11::SetParameterData( void* pData, unsigned int threadID )
{
	assert( threadID >= 0 );
	assert( threadID < NUM_THREADS+1 );

	m_iCBuffer[threadID] = *reinterpret_cast<int*>( pData );
}
//--------------------------------------------------------------------------------
//void ConstantBufferParameterDX11::ResetParameterData( void* pData, unsigned int threadID )
//{
//	assert( threadID >= 0 );
//	assert( threadID < NUM_THREADS+1 );
//
//	if ( m_iCBuffer[threadID] == *reinterpret_cast<int*>( pData ) ) {
//		m_iCBuffer[threadID] = -1;
//	}
//}
//--------------------------------------------------------------------------------
const ParameterType ConstantBufferParameterDX11::GetParameterType()
{
	return( CBUFFER );
}
//--------------------------------------------------------------------------------
int ConstantBufferParameterDX11::GetIndex( unsigned int threadID )
{
	assert( threadID >= 0 );
	assert( threadID < NUM_THREADS+1 );

	return( m_iCBuffer[threadID] );
}
//--------------------------------------------------------------------------------
//void ConstantBufferParameterDX11::UpdateValue( RenderParameterDX11* pParameter, unsigned int threadID )
//{
//	assert( threadID >= 0 );
//	assert( threadID < NUM_THREADS+1 );
//
//	if ( pParameter )
//	{
//		if ( ( pParameter->GetParameterType() == CBUFFER ) && ( pParameter->GetName() == this->GetName() ) )
//		{
//			ConstantBufferParameterDX11* pBuffer = (ConstantBufferParameterDX11*)pParameter;
//			m_iCBuffer[threadID] = pBuffer->GetIndex( threadID );
//		}
//	}
//}
//--------------------------------------------------------------------------------