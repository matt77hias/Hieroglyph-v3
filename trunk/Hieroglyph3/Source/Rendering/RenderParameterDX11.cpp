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
#include "RenderParameterDX11.h"
#include "ConstantBufferParameterDX11.h"
#include "MatrixParameterDX11.h"
#include "MatrixArrayParameterDX11.h"
#include "SamplerParameterDX11.h"
#include "ShaderResourceParameterDX11.h"
#include "UnorderedAccessParameterDX11.h"
#include "VectorParameterDX11.h"
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
RenderParameterDX11::RenderParameterDX11()
{
}
//--------------------------------------------------------------------------------
RenderParameterDX11::RenderParameterDX11( RenderParameterDX11& copy )
{
	m_sParameterName = copy.m_sParameterName;
}
//--------------------------------------------------------------------------------
RenderParameterDX11::~RenderParameterDX11()
{
}
//--------------------------------------------------------------------------------
std::wstring& RenderParameterDX11::GetName()
{
	return( m_sParameterName );
}
//--------------------------------------------------------------------------------
void RenderParameterDX11::SetName( std::wstring& name )
{
	m_sParameterName = name;
}
//--------------------------------------------------------------------------------
RenderParameterDX11* RenderParameterDX11::CreateCopy()
{
	RenderParameterDX11* pParam = 0;

	switch ( this->GetParameterType() )
	{
	case VECTOR:
		pParam = new VectorParameterDX11();
		pParam->SetName( GetName() );
		pParam->UpdateValue( this );
		break;
	case MATRIX:
		pParam = new MatrixParameterDX11();
		pParam->SetName( GetName() );
		pParam->UpdateValue( this );
		break;
	case MATRIX_ARRAY:
		pParam = new MatrixArrayParameterDX11( ((MatrixArrayParameterDX11*)this)->GetMatrixCount() );
		pParam->SetName( GetName() );
		pParam->UpdateValue( this );
		break;
	case SHADER_RESOURCE:
		pParam = new ShaderResourceParameterDX11();
		pParam->SetName( GetName() );
		pParam->UpdateValue( this );
		break;
	case UNORDERED_ACCESS:
		pParam = new UnorderedAccessParameterDX11();
		pParam->SetName( GetName() );
		pParam->UpdateValue( this );
		break;
	case CBUFFER:
		pParam = new ConstantBufferParameterDX11();
		pParam->SetName( GetName() );
		pParam->UpdateValue( this );
		break;
	case SAMPLER:
		pParam = new SamplerParameterDX11();
		pParam->SetName( GetName() );
		pParam->UpdateValue( this );
		break;
	}

	return( pParam );
}
//--------------------------------------------------------------------------------