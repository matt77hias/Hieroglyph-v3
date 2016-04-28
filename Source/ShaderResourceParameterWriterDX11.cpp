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
#include "ShaderResourceParameterWriterDX11.h"
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
ShaderResourceParameterWriterDX11::ShaderResourceParameterWriterDX11()
{
}
//--------------------------------------------------------------------------------
ShaderResourceParameterWriterDX11::~ShaderResourceParameterWriterDX11()
{
}
//--------------------------------------------------------------------------------
void ShaderResourceParameterWriterDX11::SetRenderParameterRef( ShaderResourceParameterDX11* pParam )
{
	m_pParameter = pParam;
}
//--------------------------------------------------------------------------------
void ShaderResourceParameterWriterDX11::WriteParameter( IParameterManager* pParamMgr )
{
	pParamMgr->SetShaderResourceParameter( m_pParameter, m_Value );
}
//--------------------------------------------------------------------------------
void ShaderResourceParameterWriterDX11::SetValue( ResourcePtr Value )
{
	m_Value = Value;
}
//--------------------------------------------------------------------------------
void ShaderResourceParameterWriterDX11::InitializeParameter( )
{
	m_pParameter->InitializeParameterData( &m_Value->m_iResourceSRV );
}
//--------------------------------------------------------------------------------
RenderParameterDX11* ShaderResourceParameterWriterDX11::GetRenderParameterRef()
{
	return( m_pParameter );
}
//--------------------------------------------------------------------------------
