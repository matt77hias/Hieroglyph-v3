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
#include "ShaderResourceParameterDX11.h"
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
ShaderResourceParameterDX11::ShaderResourceParameterDX11()
{
	for ( int i = 0; i <= NUM_THREADS; i++ )
		m_iShaderResourceView[i] = 0;
}
//--------------------------------------------------------------------------------
ShaderResourceParameterDX11::ShaderResourceParameterDX11( ShaderResourceParameterDX11& copy )
{
	for ( int i = 0; i <= NUM_THREADS; i++ )
		m_iShaderResourceView[i] = copy.m_iShaderResourceView[i];
}
//--------------------------------------------------------------------------------
ShaderResourceParameterDX11::~ShaderResourceParameterDX11()
{
}
//--------------------------------------------------------------------------------
void ShaderResourceParameterDX11::SetParameterData( void* pData, unsigned int threadID )
{
	assert( threadID >= 0 );
	assert( threadID < NUM_THREADS+1 );

	m_iShaderResourceView[threadID] = *reinterpret_cast<int*>( pData );
}
//--------------------------------------------------------------------------------
//void ShaderResourceParameterDX11::ResetParameterData( void* pData, unsigned int threadID )
//{
//	assert( threadID >= 0 );
//	assert( threadID < NUM_THREADS+1 );
//
//	if ( m_iShaderResourceView[threadID] == *reinterpret_cast<int*>( pData ) ) {
//		m_iShaderResourceView[threadID] = -1;
//	}
//}
//--------------------------------------------------------------------------------
const ParameterType ShaderResourceParameterDX11::GetParameterType()
{
	return( SHADER_RESOURCE );
}
//--------------------------------------------------------------------------------
int ShaderResourceParameterDX11::GetIndex( unsigned int threadID )
{
	assert( threadID >= 0 );
	assert( threadID < NUM_THREADS+1 );

	return( m_iShaderResourceView[threadID] );
}
//--------------------------------------------------------------------------------
//void ShaderResourceParameterDX11::UpdateValue( RenderParameterDX11* pParameter, unsigned int threadID )
//{
//	assert( threadID >= 0 );
//	assert( threadID < NUM_THREADS+1 );
//
//	if ( pParameter )
//	{
//		if ( ( pParameter->GetParameterType() == SHADER_RESOURCE ) && ( pParameter->GetName() == this->GetName() ) )
//		{
//			ShaderResourceParameterDX11* pBuffer = (ShaderResourceParameterDX11*)pParameter;
//			m_iShaderResourceView[threadID] = pBuffer->GetIndex( threadID );
//		}
//	}
//}
//--------------------------------------------------------------------------------