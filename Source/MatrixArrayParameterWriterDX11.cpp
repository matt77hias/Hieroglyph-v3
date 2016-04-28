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
#include "MatrixArrayParameterWriterDX11.h"
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
MatrixArrayParameterWriterDX11::MatrixArrayParameterWriterDX11()
{
	m_pValue = 0;
	m_iCount = 0;
}
//--------------------------------------------------------------------------------
MatrixArrayParameterWriterDX11::~MatrixArrayParameterWriterDX11()
{
}
//--------------------------------------------------------------------------------
void MatrixArrayParameterWriterDX11::SetRenderParameterRef( MatrixArrayParameterDX11* pParam )
{
	m_pParameter = pParam;
}
//--------------------------------------------------------------------------------
void MatrixArrayParameterWriterDX11::WriteParameter( IParameterManager* pParamMgr )
{
	pParamMgr->SetMatrixArrayParameter( m_pParameter, m_iCount, m_pValue );
}
//--------------------------------------------------------------------------------
void MatrixArrayParameterWriterDX11::SetValue( Matrix4f* Value )
{
	m_pValue = Value;
}
//--------------------------------------------------------------------------------
void MatrixArrayParameterWriterDX11::SetCount( int count )
{
	m_iCount = count;
}
//--------------------------------------------------------------------------------
void MatrixArrayParameterWriterDX11::InitializeParameter( )
{
	m_pParameter->InitializeParameterData( m_pValue );
}
//--------------------------------------------------------------------------------
RenderParameterDX11* MatrixArrayParameterWriterDX11::GetRenderParameterRef()
{
	return( m_pParameter );
}
//--------------------------------------------------------------------------------
