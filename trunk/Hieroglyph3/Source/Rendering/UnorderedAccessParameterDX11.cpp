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
#include "UnorderedAccessParameterDX11.h"
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
UnorderedAccessParameterDX11::UnorderedAccessParameterDX11()
{
	m_ParameterData.m_iUnorderedAccessView = -1;
	m_ParameterData.m_iInitialCount = -1;
}
//--------------------------------------------------------------------------------
UnorderedAccessParameterDX11::UnorderedAccessParameterDX11( UnorderedAccessParameterDX11& copy )
{
	m_ParameterData.m_iUnorderedAccessView = copy.m_ParameterData.m_iUnorderedAccessView;
	m_ParameterData.m_iInitialCount = copy.m_ParameterData.m_iInitialCount;
}
//--------------------------------------------------------------------------------
UnorderedAccessParameterDX11::~UnorderedAccessParameterDX11()
{
}
//--------------------------------------------------------------------------------
void UnorderedAccessParameterDX11::SetParameterData( void* pData )
{
	m_ParameterData = *reinterpret_cast<UAVParameterData*>( pData );
}
//--------------------------------------------------------------------------------
ParameterType UnorderedAccessParameterDX11::GetParameterType()
{
	return( UNORDERED_ACCESS );
}
//--------------------------------------------------------------------------------
int UnorderedAccessParameterDX11::GetIndex()
{
	return( m_ParameterData.m_iUnorderedAccessView );
}
//--------------------------------------------------------------------------------
unsigned int UnorderedAccessParameterDX11::GetInitialCount()
{
	return( m_ParameterData.m_iInitialCount );
}
//--------------------------------------------------------------------------------
void UnorderedAccessParameterDX11::UpdateValue( RenderParameterDX11* pParameter )
{
	if ( pParameter )
	{
		if ( ( pParameter->GetParameterType() == UNORDERED_ACCESS ) && ( pParameter->GetName() == this->GetName() ) )
		{
			UnorderedAccessParameterDX11* pBuffer = (UnorderedAccessParameterDX11*)pParameter;
			m_ParameterData.m_iUnorderedAccessView = pBuffer->GetIndex();
			m_ParameterData.m_iInitialCount = pBuffer->GetInitialCount();
		}
	}
}
//--------------------------------------------------------------------------------