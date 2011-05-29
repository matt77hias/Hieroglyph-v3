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
#include "MatrixParameterWriterDX11.h"
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
MatrixParameterWriterDX11::MatrixParameterWriterDX11()
{
	m_Value.MakeIdentity();
}
//--------------------------------------------------------------------------------
MatrixParameterWriterDX11::~MatrixParameterWriterDX11()
{
}
//--------------------------------------------------------------------------------
void MatrixParameterWriterDX11::WriteParameter( IParameterManager* pParamMgr )
{
	pParamMgr->SetMatrixParameter( m_pParameter, &m_Value );
}
//--------------------------------------------------------------------------------
void MatrixParameterWriterDX11::SetValue( Matrix4f& Value )
{
	m_Value = Value;
}
//--------------------------------------------------------------------------------