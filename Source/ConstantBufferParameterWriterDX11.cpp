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
#include "ConstantBufferParameterWriterDX11.h"
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
ConstantBufferParameterWriterDX11::ConstantBufferParameterWriterDX11()
{
}
//--------------------------------------------------------------------------------
ConstantBufferParameterWriterDX11::~ConstantBufferParameterWriterDX11()
{
}
//--------------------------------------------------------------------------------
void ConstantBufferParameterWriterDX11::SetRenderParameterRef( ConstantBufferParameterDX11* pParam )
{
	m_pParameter = pParam;
}
//--------------------------------------------------------------------------------
void ConstantBufferParameterWriterDX11::SetValue( ResourcePtr Value )
{
	m_Value = Value;
}
//--------------------------------------------------------------------------------
void ConstantBufferParameterWriterDX11::WriteParameter( IParameterManager* pParamMgr )
{
	pParamMgr->SetConstantBufferParameter( m_pParameter, m_Value );
}
//--------------------------------------------------------------------------------
void ConstantBufferParameterWriterDX11::InitializeParameter( )
{
	m_pParameter->InitializeParameterData( &m_Value->m_iResource );
}
//--------------------------------------------------------------------------------
RenderParameterDX11* ConstantBufferParameterWriterDX11::GetRenderParameterRef()
{
	return( m_pParameter );
}
//--------------------------------------------------------------------------------
