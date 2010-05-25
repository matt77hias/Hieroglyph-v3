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
#include "SamplerParameterDX11.h"
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
SamplerParameterDX11::SamplerParameterDX11()
{
	m_iSampler = -1;
}
//--------------------------------------------------------------------------------
SamplerParameterDX11::SamplerParameterDX11( SamplerParameterDX11& copy )
{
	m_iSampler = copy.m_iSampler;
}
//--------------------------------------------------------------------------------
SamplerParameterDX11::~SamplerParameterDX11()
{
}
//--------------------------------------------------------------------------------
void SamplerParameterDX11::SetParameterData( void* pData )
{
	m_iSampler = *reinterpret_cast<int*>( pData );
}
//--------------------------------------------------------------------------------
ParameterType SamplerParameterDX11::GetParameterType()
{
	return( SAMPLER );
}
//--------------------------------------------------------------------------------
int SamplerParameterDX11::GetIndex()
{
	return( m_iSampler );
}
//--------------------------------------------------------------------------------