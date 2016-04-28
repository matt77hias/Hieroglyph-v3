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
#include "SamplerParameterDX11.h"
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
SamplerParameterDX11::SamplerParameterDX11()
{
	for ( int i = 0; i <= NUM_THREADS; i++ )
		m_iSampler[i] = -1;
}
//--------------------------------------------------------------------------------
SamplerParameterDX11::SamplerParameterDX11( SamplerParameterDX11& copy )
{
	for ( int i = 0; i <= NUM_THREADS; i++ )
		m_iSampler[i] = copy.m_iSampler[i];
}
//--------------------------------------------------------------------------------
SamplerParameterDX11::~SamplerParameterDX11()
{
}
//--------------------------------------------------------------------------------
void SamplerParameterDX11::SetParameterData( void* pData, unsigned int threadID )
{
	assert( threadID >= 0 );
	assert( threadID < NUM_THREADS+1 );

	m_iSampler[threadID] = *reinterpret_cast<int*>( pData );
}
//--------------------------------------------------------------------------------
//void SamplerParameterDX11::ResetParameterData( void* pData, unsigned int threadID )
//{
//	assert( threadID >= 0 );
//	assert( threadID < NUM_THREADS+1 );
//
//	if ( m_iSampler[threadID] == *reinterpret_cast<int*>( pData ) ) {
//		m_iSampler[threadID] = -1;
//	}
//}
//--------------------------------------------------------------------------------
const ParameterType SamplerParameterDX11::GetParameterType()
{
	return( SAMPLER );
}
//--------------------------------------------------------------------------------
int SamplerParameterDX11::GetIndex( unsigned int threadID )
{
	assert( threadID >= 0 );
	assert( threadID < NUM_THREADS+1 );

	return( m_iSampler[threadID] );
}
//--------------------------------------------------------------------------------
//void SamplerParameterDX11::UpdateValue( RenderParameterDX11* pParameter, unsigned int threadID )
//{
//	assert( threadID >= 0 );
//	assert( threadID < NUM_THREADS+1 );
//
//	if ( pParameter )
//	{
//		if ( ( pParameter->GetParameterType() == SAMPLER ) && ( pParameter->GetName() == this->GetName() ) )
//		{
//			SamplerParameterDX11* pBuffer = (SamplerParameterDX11*)pParameter;
//			m_iSampler[threadID] = pBuffer->GetIndex( threadID );
//		}
//	}
//}
//--------------------------------------------------------------------------------