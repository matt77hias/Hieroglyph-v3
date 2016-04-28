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
#include "VectorParameterWriterDX11.h"
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
VectorParameterWriterDX11::VectorParameterWriterDX11()
	: m_Value( 0.0f, 0.0f, 0.0f, 0.0f )
{
}
//--------------------------------------------------------------------------------
VectorParameterWriterDX11::~VectorParameterWriterDX11()
{
}
//--------------------------------------------------------------------------------
void VectorParameterWriterDX11::SetRenderParameterRef( VectorParameterDX11* pParam )
{
	m_pParameter = pParam;
}
//--------------------------------------------------------------------------------
void VectorParameterWriterDX11::WriteParameter( IParameterManager* pParamMgr )
{
	pParamMgr->SetVectorParameter( m_pParameter, &m_Value );
}
//--------------------------------------------------------------------------------
void VectorParameterWriterDX11::SetValue( const Vector4f& Value )
{
	m_Value = Value;
}
//--------------------------------------------------------------------------------
Vector4f VectorParameterWriterDX11::GetValue()
{
	return( m_Value );
}
//--------------------------------------------------------------------------------
void VectorParameterWriterDX11::InitializeParameter( )
{
	m_pParameter->InitializeParameterData( &m_Value );
}
//--------------------------------------------------------------------------------
RenderParameterDX11* VectorParameterWriterDX11::GetRenderParameterRef()
{
	return( m_pParameter );
}
//--------------------------------------------------------------------------------
