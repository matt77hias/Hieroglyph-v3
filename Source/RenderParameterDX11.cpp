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
#include "RenderParameterDX11.h"
//#include "ConstantBufferParameterDX11.h"
//#include "MatrixParameterDX11.h"
//#include "MatrixArrayParameterDX11.h"
//#include "SamplerParameterDX11.h"
//#include "ShaderResourceParameterDX11.h"
//#include "UnorderedAccessParameterDX11.h"
//#include "VectorParameterDX11.h"
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
RenderParameterDX11::RenderParameterDX11()
{
	for ( int i = 0; i < NUM_THREADS+1; i++ ) {
		m_auiValueID[i] = 0;
	}
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
void RenderParameterDX11::SetName( const std::wstring& name )
{
	m_sParameterName = name;
}
//--------------------------------------------------------------------------------
void RenderParameterDX11::InitializeParameterData( void* pData )
{
	for ( int i = 0; i <= NUM_THREADS; i++ )
		SetParameterData( pData, i );
}
//--------------------------------------------------------------------------------
//void RenderParameterDX11::UnInitializeParameterData( void* pData )
//{
//	for ( int i = 0; i <= NUM_THREADS; i++ )
//		ResetParameterData( pData, i );
//}
//--------------------------------------------------------------------------------
unsigned int RenderParameterDX11::GetValueID( unsigned int threadID )
{
	assert( threadID >= 0 );
	assert( threadID < NUM_THREADS+1 );

	return( m_auiValueID[threadID] );
}
//--------------------------------------------------------------------------------
//RenderParameterDX11* RenderParameterDX11::CreateCopy()
//{
//	RenderParameterDX11* pParam = 0;
//
//	switch ( this->GetParameterType() )
//	{
//	case VECTOR:
//		pParam = new VectorParameterDX11();
//		pParam->SetName( GetName() );
//		pParam->UpdateValue( this );
//		break;
//	case MATRIX:
//		pParam = new MatrixParameterDX11();
//		pParam->SetName( GetName() );
//		pParam->UpdateValue( this );
//		break;
//	case MATRIX_ARRAY:
//		pParam = new MatrixArrayParameterDX11( ((MatrixArrayParameterDX11*)this)->GetMatrixCount() );
//		pParam->SetName( GetName() );
//		pParam->UpdateValue( this );
//		break;
//	case SHADER_RESOURCE:
//		pParam = new ShaderResourceParameterDX11();
//		pParam->SetName( GetName() );
//		pParam->UpdateValue( this );
//		break;
//	case UNORDERED_ACCESS:
//		pParam = new UnorderedAccessParameterDX11();
//		pParam->SetName( GetName() );
//		pParam->UpdateValue( this );
//		break;
//	case CBUFFER:
//		pParam = new ConstantBufferParameterDX11();
//		pParam->SetName( GetName() );
//		pParam->UpdateValue( this );
//		break;
//	case SAMPLER:
//		pParam = new SamplerParameterDX11();
//		pParam->SetName( GetName() );
//		pParam->UpdateValue( this );
//		break;
//	}
//
//	return( pParam );
//}
//--------------------------------------------------------------------------------