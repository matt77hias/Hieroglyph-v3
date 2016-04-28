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
#include "ParameterManagerDX11.h"
#include "VectorParameterDX11.h"
#include "MatrixParameterDX11.h"
#include "MatrixArrayParameterDX11.h"
#include "ShaderResourceParameterDX11.h"
#include "UnorderedAccessParameterDX11.h"
#include "ConstantBufferParameterDX11.h"
#include "SamplerParameterDX11.h"
#include "Log.h"
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
std::map< std::wstring, RenderParameterDX11* >	ParameterManagerDX11::m_Parameters;
//--------------------------------------------------------------------------------
ParameterManagerDX11::ParameterManagerDX11( unsigned int ID )
{
	m_ID = ID;
	m_pParent = 0;

	m_pWorldMatrix = GetMatrixParameterRef( std::wstring( L"WorldMatrix" ) );
	m_pViewMatrix = GetMatrixParameterRef( std::wstring( L"ViewMatrix" ) );
	m_pProjMatrix = GetMatrixParameterRef( std::wstring( L"ProjMatrix" ) );
	m_pWorldViewMatrix = GetMatrixParameterRef( std::wstring( L"WorldViewMatrix" ) );
	m_pViewProjMatrix = GetMatrixParameterRef( std::wstring( L"ViewProjMatrix" ) );
	m_pWorldProjMatrix = GetMatrixParameterRef( std::wstring( L"WorldProjMatrix" ) );
	m_pWorldViewProjMatrix = GetMatrixParameterRef( std::wstring( L"WorldViewProjMatrix" ) );
}
//--------------------------------------------------------------------------------
ParameterManagerDX11::~ParameterManagerDX11()
{
	// Iterate the list of parameters and release them
	std::map< std::wstring, RenderParameterDX11* >::iterator iter = m_Parameters.begin();
	while ( iter != m_Parameters.end() ) {
		SAFE_DELETE( iter->second );
		iter++;
	}

	m_Parameters.clear();
}
//--------------------------------------------------------------------------------
unsigned int ParameterManagerDX11::GetID()
{
	return( m_ID );
}
//--------------------------------------------------------------------------------
//void ParameterManagerDX11::SetParameter( RenderParameterDX11* pParameter )
//{
//	const std::wstring& name = pParameter->GetName();
//	RenderParameterDX11* pCurrent = m_Parameters[name];
//
//	if ( pParameter )
//	{
//		if ( pParameter->GetParameterType() == VECTOR )
//		{
//			// Only create the new parameter if it hasn't already been registered
//			if ( pCurrent == 0 )
//			{
//				pCurrent = new VectorParameterDX11();
//				pCurrent->SetName( name );
//				m_Parameters[name] = reinterpret_cast<RenderParameterDX11*>( pCurrent );
//			}
//
//			*reinterpret_cast<VectorParameterDX11*>( pCurrent )
//				= *reinterpret_cast<VectorParameterDX11*>( pParameter );
//		}
//		if ( pParameter->GetParameterType() == MATRIX )
//		{
//			// Only create the new parameter if it hasn't already been registered
//			if ( pCurrent == 0 )
//			{
//				pCurrent = new MatrixParameterDX11();
//				pCurrent->SetName( name );
//				m_Parameters[name] = reinterpret_cast<RenderParameterDX11*>( pCurrent );
//			}
//
//			*reinterpret_cast<MatrixParameterDX11*>( pCurrent )
//				= *reinterpret_cast<MatrixParameterDX11*>( pParameter );
//		}
//		if ( pParameter->GetParameterType() == MATRIX_ARRAY )
//		{
//			// Only create the new parameter if it hasn't already been registered
//			if ( pCurrent == 0 )
//			{
//				int count = reinterpret_cast<MatrixArrayParameterDX11*>( pParameter )->GetMatrixCount();
//				pCurrent = new MatrixArrayParameterDX11( count );
//				m_Parameters[name] = reinterpret_cast<RenderParameterDX11*>( pCurrent );
//			}
//
//			// This assignment performs a deep copy of the matrix array.
//			*reinterpret_cast<MatrixArrayParameterDX11*>( pCurrent )
//				= *reinterpret_cast<MatrixArrayParameterDX11*>( pParameter );
//		}
//		if ( pParameter->GetParameterType() == SHADER_RESOURCE )
//		{
//			// Only create the new parameter if it hasn't already been registered
//			if ( pCurrent == 0 )
//			{
//				pCurrent = new ShaderResourceParameterDX11();
//				pCurrent->SetName( name );
//				m_Parameters[name] = reinterpret_cast<RenderParameterDX11*>( pCurrent );
//			}
//
//			*reinterpret_cast<ShaderResourceParameterDX11*>( pCurrent )
//				= *reinterpret_cast<ShaderResourceParameterDX11*>( pParameter );
//		}
//		if ( pParameter->GetParameterType() == UNORDERED_ACCESS )
//		{
//			// Only create the new parameter if it hasn't already been registered
//			if ( pCurrent == 0 )
//			{
//				pCurrent = new UnorderedAccessParameterDX11();
//				pCurrent->SetName( name );
//				m_Parameters[name] = reinterpret_cast<RenderParameterDX11*>( pCurrent );
//			}
//
//			*reinterpret_cast<UnorderedAccessParameterDX11*>( pCurrent )
//				= *reinterpret_cast<UnorderedAccessParameterDX11*>( pParameter );
//		}
//		if ( pParameter->GetParameterType() == CBUFFER )
//		{
//			// Only create the new parameter if it hasn't already been registered
//			if ( pCurrent == 0 )
//			{
//				pCurrent = new ConstantBufferParameterDX11();
//				pCurrent->SetName( name );
//				m_Parameters[name] = reinterpret_cast<RenderParameterDX11*>( pCurrent );
//			}
//
//			*reinterpret_cast<ConstantBufferParameterDX11*>( pCurrent )
//				= *reinterpret_cast<ConstantBufferParameterDX11*>( pParameter );
//		}
//		if ( pParameter->GetParameterType() == SAMPLER )
//		{
//			// Only create the new parameter if it hasn't already been registered
//			if ( pCurrent == 0 )
//			{
//				pCurrent = new SamplerParameterDX11();
//				pCurrent->SetName( name );
//				m_Parameters[name] = reinterpret_cast<RenderParameterDX11*>( pCurrent );
//			}
//
//			*reinterpret_cast<SamplerParameterDX11*>( pCurrent )
//				= *reinterpret_cast<SamplerParameterDX11*>( pParameter );
//		}
//	}
//}
//--------------------------------------------------------------------------------
void ParameterManagerDX11::SetVectorParameter( const std::wstring& name, Vector4f* pVector )
{
	RenderParameterDX11* pParameter = m_Parameters[name];

	// Only create the new parameter if it hasn't already been registered
	if ( pParameter == 0 )
	{
		pParameter = new VectorParameterDX11();
		pParameter->SetName( name );
		m_Parameters[name] = reinterpret_cast<RenderParameterDX11*>( pParameter );

		// Initialize the parameter with the current data in all slots
		pParameter->InitializeParameterData( reinterpret_cast<void*>( pVector ) );
	}
	else
	{
		if ( pParameter->GetParameterType() == VECTOR )
			pParameter->SetParameterData( reinterpret_cast<void*>( pVector ), GetID() );
		else
			Log::Get().Write( L"Vector parameter name collision!" );
	}
}
//--------------------------------------------------------------------------------
void ParameterManagerDX11::SetMatrixParameter( const std::wstring& name, Matrix4f* pMatrix )
{
	RenderParameterDX11* pParameter = m_Parameters[name];

	// Only create the new parameter if it hasn't already been registered
	if ( pParameter == 0 )
	{
		pParameter = new MatrixParameterDX11();
		pParameter->SetName( name );
		m_Parameters[name] = reinterpret_cast<RenderParameterDX11*>( pParameter );
		
		// Initialize the parameter with the current data in all slots
		pParameter->InitializeParameterData( reinterpret_cast<void*>( pMatrix ) );
	}
	else
	{
		if ( pParameter->GetParameterType() == MATRIX )
			pParameter->SetParameterData( reinterpret_cast<void*>( pMatrix ), GetID() );
		else
			Log::Get().Write( L"Matrix parameter name collision!" );
	}
}
//--------------------------------------------------------------------------------
void ParameterManagerDX11::SetMatrixArrayParameter( const std::wstring& name, int count, Matrix4f* pMatrix )
{
	RenderParameterDX11* pParameter = m_Parameters[name];

	// Only create the new parameter if it hasn't already been registered
	if ( pParameter == 0 )
	{
		pParameter = new MatrixArrayParameterDX11( count );
		pParameter->SetName( name );
		m_Parameters[name] = reinterpret_cast<RenderParameterDX11*>( pParameter );

		// Initialize the parameter with the current data in all slots
		pParameter->InitializeParameterData( reinterpret_cast<void*>( pMatrix ) );
	}
	else
	{
		if ( pParameter->GetParameterType() == MATRIX_ARRAY )
			pParameter->SetParameterData( reinterpret_cast<void*>( pMatrix ), GetID() );
		else
			Log::Get().Write( L"Matrix Array parameter name collision!" );
	}
}
//--------------------------------------------------------------------------------
void ParameterManagerDX11::SetShaderResourceParameter( const std::wstring& name, ResourcePtr resource )
{
	RenderParameterDX11* pParameter = m_Parameters[name];

	// Only create the new parameter if it hasn't already been registered
	if ( pParameter == 0 )
	{
		pParameter = new ShaderResourceParameterDX11();
		pParameter->SetName( name );
		m_Parameters[name] = reinterpret_cast<RenderParameterDX11*>( pParameter );

		// Initialize the parameter with the current data in all slots
		pParameter->InitializeParameterData( reinterpret_cast<void*>( &resource->m_iResourceSRV ) );
	}
	else
	{
		if ( pParameter->GetParameterType() == SHADER_RESOURCE )
			pParameter->SetParameterData( reinterpret_cast<void*>( &resource->m_iResourceSRV ), GetID() );
		else
			Log::Get().Write( L"Shader resource view parameter name collision!" );
	}
}
//--------------------------------------------------------------------------------
void ParameterManagerDX11::SetUnorderedAccessParameter( const std::wstring& name, ResourcePtr resource, unsigned int initial )
{
	RenderParameterDX11* pParameter = m_Parameters[name];

	// Only create the new parameter if it hasn't already been registered
	if ( pParameter == 0 )
	{
		pParameter = new UnorderedAccessParameterDX11();
		pParameter->SetName( name );
		m_Parameters[name] = reinterpret_cast<RenderParameterDX11*>( pParameter );

		// Initialize the parameter with the current data in all slots
		UAVParameterData data; 
		data.m_iUnorderedAccessView = resource->m_iResourceUAV; 
		data.m_iInitialCount = initial;

		pParameter->InitializeParameterData( reinterpret_cast<void*>( &data ) );
	}
	else
	{
		if ( pParameter->GetParameterType() == UNORDERED_ACCESS )
		{
			UAVParameterData data; 
			data.m_iUnorderedAccessView = resource->m_iResourceUAV; 
			data.m_iInitialCount = initial;
			pParameter->SetParameterData( reinterpret_cast<void*>( &data ), GetID() );
		}
		else
			Log::Get().Write( L"Unordered access view parameter name collision!" );
	}
}
//--------------------------------------------------------------------------------
void ParameterManagerDX11::SetConstantBufferParameter( const std::wstring& name, ResourcePtr resource )
{
	RenderParameterDX11* pParameter = m_Parameters[name];

	// Only create the new parameter if it hasn't already been registered
	if ( pParameter == 0 )
	{
		pParameter = new ConstantBufferParameterDX11();
		pParameter->SetName( name );
		m_Parameters[name] = reinterpret_cast<RenderParameterDX11*>( pParameter );

		// Initialize the parameter with the current data in all slots
		pParameter->InitializeParameterData( reinterpret_cast<void*>( &resource->m_iResource ) );
	}
	else
	{
		if ( pParameter->GetParameterType() == CBUFFER )
			pParameter->SetParameterData( reinterpret_cast<void*>( &resource->m_iResource ), GetID() );
		else
			Log::Get().Write( L"Constant buffer parameter name collision!" );
	}
}
//--------------------------------------------------------------------------------
void ParameterManagerDX11::SetSamplerParameter( const std::wstring& name, int* pID )
{
	RenderParameterDX11* pParameter = m_Parameters[name];

	// Only create the new parameter if it hasn't already been registered
	if ( pParameter == 0 )
	{
		pParameter = new SamplerParameterDX11();
		pParameter->SetName( name );
		m_Parameters[name] = reinterpret_cast<RenderParameterDX11*>( pParameter );

		// Initialize the parameter with the current data in all slots
		pParameter->InitializeParameterData( reinterpret_cast<void*>( pID ) );
	}
	else
	{
		if ( pParameter->GetParameterType() == SAMPLER )
			pParameter->SetParameterData( reinterpret_cast<void*>( pID ), GetID() );
		else
			Log::Get().Write( L"Sampler parameter name collision!" );
	}
}
//--------------------------------------------------------------------------------
void ParameterManagerDX11::SetVectorParameter( RenderParameterDX11* pParameter, Vector4f* pVector )
{
	if ( pParameter->GetParameterType() == VECTOR )
		pParameter->SetParameterData( reinterpret_cast<void*>( pVector ), GetID() );
	else
		Log::Get().Write( L"Vector parameter name collision!" );
}
//--------------------------------------------------------------------------------
void ParameterManagerDX11::SetMatrixParameter( RenderParameterDX11* pParameter, Matrix4f* pMatrix )
{
	if ( pParameter->GetParameterType() == MATRIX )
		pParameter->SetParameterData( reinterpret_cast<void*>( pMatrix ), GetID() );
	else
		Log::Get().Write( L"Matrix parameter name collision!" );
}
//--------------------------------------------------------------------------------
void ParameterManagerDX11::SetMatrixArrayParameter( RenderParameterDX11* pParameter, int count, Matrix4f* pMatrix )
{
	if ( pParameter->GetParameterType() == MATRIX_ARRAY )
		pParameter->SetParameterData( reinterpret_cast<void*>( pMatrix ), GetID() );
	else
		Log::Get().Write( L"Matrix Array parameter name collision!" );
}
//--------------------------------------------------------------------------------
void ParameterManagerDX11::SetShaderResourceParameter( RenderParameterDX11* pParameter, ResourcePtr resource )
{
	if ( pParameter->GetParameterType() == SHADER_RESOURCE )
		pParameter->SetParameterData( reinterpret_cast<void*>( &resource->m_iResourceSRV ), GetID() );
	else
		Log::Get().Write( L"Shader resource view parameter name collision!" );
}
//--------------------------------------------------------------------------------
void ParameterManagerDX11::SetUnorderedAccessParameter( RenderParameterDX11* pParameter, ResourcePtr resource, unsigned int initial )
{
	if ( pParameter->GetParameterType() == UNORDERED_ACCESS )
	{
		UAVParameterData data; 
		data.m_iUnorderedAccessView = resource->m_iResourceUAV; 
		data.m_iInitialCount = initial;
		pParameter->SetParameterData( reinterpret_cast<void*>( &data ), GetID() );
	}
	else
		Log::Get().Write( L"Unordered access view parameter name collision!" );
}
//--------------------------------------------------------------------------------
void ParameterManagerDX11::SetConstantBufferParameter( RenderParameterDX11* pParameter, ResourcePtr resource )
{
	if ( pParameter->GetParameterType() == CBUFFER )
		pParameter->SetParameterData( reinterpret_cast<void*>( &resource->m_iResource ), GetID() );
	else
		Log::Get().Write( L"Constant buffer parameter name collision!" );
}
//--------------------------------------------------------------------------------
void ParameterManagerDX11::SetSamplerParameter( RenderParameterDX11* pParameter, int* pID )
{
	if ( pParameter->GetParameterType() == SAMPLER )
		pParameter->SetParameterData( reinterpret_cast<void*>( pID ), GetID() );
	else
		Log::Get().Write( L"Sampler parameter name collision!" );
}
//--------------------------------------------------------------------------------
Vector4f ParameterManagerDX11::GetVectorParameter( const std::wstring& name )
{
	Vector4f result;
	result.MakeZero();

	// Check for the existence of this parameter.  This search includes any
	// parent parameter managers if the parameter doesn't exist in this one.

	RenderParameterDX11* pParam = GetParameterRef( name );

	// If the parameter is not found, create a new default one.  This goes 
	// into the bottom level manager.

	if ( pParam != 0 )
	{
		if ( pParam->GetParameterType() == VECTOR ) 
			result = reinterpret_cast<VectorParameterDX11*>( pParam )->GetVector( GetID() );
	}
	else
	{
		pParam = new VectorParameterDX11();
		pParam->SetName( name );
		m_Parameters[name] = pParam;
	}

	return( result );
}
//--------------------------------------------------------------------------------
Matrix4f ParameterManagerDX11::GetMatrixParameter( const std::wstring& name )
{
	Matrix4f result;
	result.MakeZero();

	// Check for the existence of this parameter.  This search includes any
	// parent parameter managers if the parameter doesn't exist in this one.

	RenderParameterDX11* pParam = GetParameterRef( name );

	// If the parameter is not found, create a new default one.  This goes 
	// into the bottom level manager.

	if ( pParam != 0 )
	{
		if ( pParam->GetParameterType() == MATRIX ) 
			result = reinterpret_cast<MatrixParameterDX11*>( pParam )->GetMatrix( GetID() );
	}
	else
	{
		pParam = new MatrixParameterDX11();
		pParam->SetName( name );
		m_Parameters[name] = pParam;
	}

	return( result );
}
//--------------------------------------------------------------------------------
Matrix4f* ParameterManagerDX11::GetMatrixArrayParameter( const std::wstring& name, int count )
{
	Matrix4f* pResult = 0;

	// Check for the existence of this parameter.  This search includes any
	// parent parameter managers if the parameter doesn't exist in this one.

	RenderParameterDX11* pParam = GetParameterRef( name );

	// If the parameter is not found, create a new default one.  This goes 
	// into the bottom level manager.

	if ( pParam != 0 )
	{
		if ( pParam->GetParameterType() == MATRIX_ARRAY ) 
			if ( reinterpret_cast<MatrixArrayParameterDX11*>( pParam )->GetMatrixCount() == count )
				pResult = reinterpret_cast<MatrixArrayParameterDX11*>( pParam )->GetMatrices( GetID() );
	}
	else
	{
		pParam = new MatrixArrayParameterDX11( count );
		pParam->SetName( name );
		m_Parameters[name] = pParam;
		pResult = reinterpret_cast<MatrixArrayParameterDX11*>( pParam )->GetMatrices( GetID() );
	}

	return( pResult );
}
//--------------------------------------------------------------------------------
int ParameterManagerDX11::GetShaderResourceParameter( const std::wstring& name )
{
	int result;
	result = -1;

	// Check for the existence of this parameter.  This search includes any
	// parent parameter managers if the parameter doesn't exist in this one.

	RenderParameterDX11* pParam = GetParameterRef( name );

	// If the parameter is not found, create a new default one.  This goes 
	// into the bottom level manager.

	if ( pParam != 0 )
	{
		if ( pParam->GetParameterType() == SHADER_RESOURCE ) 
			result = reinterpret_cast<ShaderResourceParameterDX11*>( pParam )->GetIndex( GetID() );
	}
	else
	{
		pParam = new ShaderResourceParameterDX11();
		pParam->SetName( name );
		m_Parameters[name] = pParam;
	}

	return( result );
}
//--------------------------------------------------------------------------------
int ParameterManagerDX11::GetUnorderedAccessParameter( const std::wstring& name )
{
	int result;
	result = -1;

	// Check for the existence of this parameter.  This search includes any
	// parent parameter managers if the parameter doesn't exist in this one.

	RenderParameterDX11* pParam = GetParameterRef( name );

	// If the parameter is not found, create a new default one.  This goes 
	// into the bottom level manager.

	if ( pParam != 0 )
	{
		if ( pParam->GetParameterType() == UNORDERED_ACCESS ) 
			result = reinterpret_cast<UnorderedAccessParameterDX11*>( pParam )->GetIndex( GetID() );
	}
	else
	{
		pParam = new UnorderedAccessParameterDX11();
		pParam->SetName( name );
		m_Parameters[name] = pParam;
	}

	return( result );
}
//--------------------------------------------------------------------------------
int ParameterManagerDX11::GetConstantBufferParameter( const std::wstring& name )
{
	int result;
	result = -1;

	// Check for the existence of this parameter.  This search includes any
	// parent parameter managers if the parameter doesn't exist in this one.

	RenderParameterDX11* pParam = GetParameterRef( name );

	// If the parameter is not found, create a new default one.  This goes 
	// into the bottom level manager.

	if ( pParam != 0 )
	{
		if ( pParam->GetParameterType() == CBUFFER ) 
			result = reinterpret_cast<ConstantBufferParameterDX11*>( pParam )->GetIndex( GetID() );
	}
	else
	{
		pParam = new ConstantBufferParameterDX11();
		pParam->SetName( name );
		m_Parameters[name] = pParam;
	}

	return( result );
}
//--------------------------------------------------------------------------------
int ParameterManagerDX11::GetSamplerStateParameter( const std::wstring& name )
{
	int result;
	result = -1;

	// Check for the existence of this parameter.  This search includes any
	// parent parameter managers if the parameter doesn't exist in this one.

	RenderParameterDX11* pParam = GetParameterRef( name );

	// If the parameter is not found, create a new default one.  This goes 
	// into the bottom level manager.

	if ( pParam != 0 )
	{
		if ( pParam->GetParameterType() == SAMPLER ) 
			result = reinterpret_cast<SamplerParameterDX11*>( pParam )->GetIndex( GetID() );
	}
	else
	{
		pParam = new SamplerParameterDX11();
		pParam->SetName( name );
		m_Parameters[name] = pParam;
	}

	return( result );	
}
//--------------------------------------------------------------------------------
Vector4f ParameterManagerDX11::GetVectorParameter( RenderParameterDX11* pParam )
{
	assert( pParam != 0 );

	Vector4f result;
	result.MakeZero();

	// If the parameter is not found, create a new default one.  This goes 
	// into the bottom level manager.

	if ( pParam->GetParameterType() == VECTOR ) 
		result = reinterpret_cast<VectorParameterDX11*>( pParam )->GetVector( GetID() );

	return( result );
}
//--------------------------------------------------------------------------------
Matrix4f ParameterManagerDX11::GetMatrixParameter( RenderParameterDX11* pParam )
{
	assert( pParam != 0 );

	Matrix4f result;
	result.MakeZero();

	// If the parameter is not found, create a new default one.  This goes 
	// into the bottom level manager.

	if ( pParam->GetParameterType() == MATRIX ) 
		result = reinterpret_cast<MatrixParameterDX11*>( pParam )->GetMatrix( GetID() );

	return( result );
}
//--------------------------------------------------------------------------------
Matrix4f* ParameterManagerDX11::GetMatrixArrayParameter( RenderParameterDX11* pParam )
{
	assert( pParam != 0 );

	Matrix4f* pResult = 0;

	// If the parameter is not found, create a new default one.  This goes 
	// into the bottom level manager.

	if ( pParam->GetParameterType() == MATRIX_ARRAY ) 
		pResult = reinterpret_cast<MatrixArrayParameterDX11*>( pParam )->GetMatrices( GetID() );

	return( pResult );
}
//--------------------------------------------------------------------------------
int ParameterManagerDX11::GetShaderResourceParameter( RenderParameterDX11* pParam )
{
	assert( pParam != 0 );

	int result = -1;

	// If the parameter is not found, create a new default one.  This goes 
	// into the bottom level manager.

	if ( pParam->GetParameterType() == SHADER_RESOURCE ) 
		result = reinterpret_cast<ShaderResourceParameterDX11*>( pParam )->GetIndex( GetID() );

	return( result );
}
//--------------------------------------------------------------------------------
int ParameterManagerDX11::GetUnorderedAccessParameter( RenderParameterDX11* pParam )
{
	assert( pParam != 0 );

	int result = -1;

	// If the parameter is not found, create a new default one.  This goes 
	// into the bottom level manager.

	if ( pParam->GetParameterType() == UNORDERED_ACCESS ) 
		result = reinterpret_cast<UnorderedAccessParameterDX11*>( pParam )->GetIndex( GetID() );

	return( result );
}
//--------------------------------------------------------------------------------
int ParameterManagerDX11::GetConstantBufferParameter( RenderParameterDX11* pParam )
{
	assert( pParam != 0 );

	int result = -1;

	// If the parameter is not found, create a new default one.  This goes 
	// into the bottom level manager.

	if ( pParam->GetParameterType() == CBUFFER ) 
		result = reinterpret_cast<ConstantBufferParameterDX11*>( pParam )->GetIndex( GetID() );

	return( result );
}
//--------------------------------------------------------------------------------
int ParameterManagerDX11::GetSamplerStateParameter( RenderParameterDX11* pParam )
{
	assert( pParam != 0 );

	int result = -1;

	// If the parameter is not found, create a new default one.  This goes 
	// into the bottom level manager.

	if ( pParam != 0 )
	{
		if ( pParam->GetParameterType() == SAMPLER ) 
			result = reinterpret_cast<SamplerParameterDX11*>( pParam )->GetIndex( GetID() );
	}

	return( result );	
}
//--------------------------------------------------------------------------------
VectorParameterDX11* ParameterManagerDX11::GetVectorParameterRef( const std::wstring& name )
{
	// Check for the existence of this parameter.  This search includes any
	// parent parameter managers if the parameter doesn't exist in this one.

	RenderParameterDX11* pParam = GetParameterRef( name );

	// If the parameter is not found, create a new default one.  This goes 
	// into the bottom level manager.

	if ( pParam == 0 )
	{
		pParam = new VectorParameterDX11();
		pParam->SetName( name );
		m_Parameters[name] = pParam;
	}

	return( reinterpret_cast<VectorParameterDX11*>( pParam ) );
}
//--------------------------------------------------------------------------------
MatrixParameterDX11* ParameterManagerDX11::GetMatrixParameterRef( const std::wstring& name )
{
	// Check for the existence of this parameter.  This search includes any
	// parent parameter managers if the parameter doesn't exist in this one.

	RenderParameterDX11* pParam = GetParameterRef( name );

	// If the parameter is not found, create a new default one.  This goes 
	// into the bottom level manager.

	if ( pParam == 0 )
	{
		pParam = new MatrixParameterDX11();
		pParam->SetName( name );
		m_Parameters[name] = pParam;
	}

	return( reinterpret_cast<MatrixParameterDX11*>( pParam ) );
}
//--------------------------------------------------------------------------------
MatrixArrayParameterDX11* ParameterManagerDX11::GetMatrixArrayParameterRef( const std::wstring& name, int count )
{
	RenderParameterDX11* pParam = GetParameterRef( name );

	// If the parameter is not found, create a new default one.  This goes 
	// into the bottom level manager.

	if ( pParam == 0 )
	{
		pParam = new MatrixArrayParameterDX11( count );
		pParam->SetName( name );
		m_Parameters[name] = pParam;
	}

	return( reinterpret_cast<MatrixArrayParameterDX11*>( pParam ) );
}
//--------------------------------------------------------------------------------
ShaderResourceParameterDX11* ParameterManagerDX11::GetShaderResourceParameterRef( const std::wstring& name )
{
	// Check for the existence of this parameter.  This search includes any
	// parent parameter managers if the parameter doesn't exist in this one.

	RenderParameterDX11* pParam = GetParameterRef( name );

	// If the parameter is not found, create a new default one.  This goes 
	// into the bottom level manager.

	if ( pParam == 0 )
	{
		pParam = new ShaderResourceParameterDX11();
		pParam->SetName( name );
		m_Parameters[name] = pParam;
	}

	return( reinterpret_cast<ShaderResourceParameterDX11*>( pParam ) );
}
//--------------------------------------------------------------------------------
UnorderedAccessParameterDX11* ParameterManagerDX11::GetUnorderedAccessParameterRef( const std::wstring& name )
{
	// Check for the existence of this parameter.  This search includes any
	// parent parameter managers if the parameter doesn't exist in this one.

	RenderParameterDX11* pParam = GetParameterRef( name );

	// If the parameter is not found, create a new default one.  This goes 
	// into the bottom level manager.

	if ( pParam == 0 )
	{
		pParam = new UnorderedAccessParameterDX11();
		pParam->SetName( name );
		m_Parameters[name] = pParam;
	}

	return( reinterpret_cast<UnorderedAccessParameterDX11*>( pParam ) );
}
//--------------------------------------------------------------------------------
ConstantBufferParameterDX11* ParameterManagerDX11::GetConstantBufferParameterRef( const std::wstring& name )
{
	// Check for the existence of this parameter.  This search includes any
	// parent parameter managers if the parameter doesn't exist in this one.

	RenderParameterDX11* pParam = GetParameterRef( name );

	// If the parameter is not found, create a new default one.  This goes 
	// into the bottom level manager.

	if ( pParam == 0 )
	{
		pParam = new ConstantBufferParameterDX11();
		pParam->SetName( name );
		m_Parameters[name] = pParam;
	}

	return( reinterpret_cast<ConstantBufferParameterDX11*>( pParam ) );
}
//--------------------------------------------------------------------------------
SamplerParameterDX11* ParameterManagerDX11::GetSamplerStateParameterRef( const std::wstring& name )
{
	// Check for the existence of this parameter.  This search includes any
	// parent parameter managers if the parameter doesn't exist in this one.

	RenderParameterDX11* pParam = GetParameterRef( name );

	// If the parameter is not found, create a new default one.  This goes 
	// into the bottom level manager.

	if ( pParam == 0 )
	{
		pParam = new SamplerParameterDX11();
		pParam->SetName( name );
		m_Parameters[name] = pParam;
	}

	return( reinterpret_cast<SamplerParameterDX11*>( pParam ) );	
}
//--------------------------------------------------------------------------------
void ParameterManagerDX11::SetWorldMatrixParameter( Matrix4f* pMatrix )
{
	SetMatrixParameter( m_pWorldMatrix, pMatrix );

	Matrix4f WorldMatrix = GetMatrixParameter( m_pWorldMatrix );
	Matrix4f ViewMatrix = GetMatrixParameter( m_pViewMatrix );
	Matrix4f ProjMatrix = GetMatrixParameter( m_pProjMatrix );

	Matrix4f WorldViewMatrix = WorldMatrix * ViewMatrix;
	Matrix4f ViewProjMatrix = ViewMatrix * ProjMatrix;
	Matrix4f WorldProjMatrix = WorldMatrix * ProjMatrix;
	Matrix4f WorldViewProjMatrix = WorldMatrix * ViewProjMatrix;

	SetMatrixParameter( m_pWorldViewMatrix, &WorldViewMatrix );
	SetMatrixParameter( m_pViewProjMatrix, &ViewProjMatrix );
	SetMatrixParameter( m_pWorldProjMatrix, &WorldProjMatrix );
	SetMatrixParameter( m_pWorldViewProjMatrix, &WorldViewProjMatrix );
}
//--------------------------------------------------------------------------------
void ParameterManagerDX11::SetViewMatrixParameter( Matrix4f* pMatrix )
{
	SetMatrixParameter( m_pViewMatrix, pMatrix );

	Matrix4f WorldMatrix = GetMatrixParameter( m_pWorldMatrix );
	Matrix4f ViewMatrix = GetMatrixParameter( m_pViewMatrix );
	Matrix4f ProjMatrix = GetMatrixParameter( m_pProjMatrix );

	Matrix4f WorldViewMatrix = WorldMatrix * ViewMatrix;
	Matrix4f ViewProjMatrix = ViewMatrix * ProjMatrix;
	Matrix4f WorldProjMatrix = WorldMatrix * ProjMatrix;
	Matrix4f WorldViewProjMatrix = WorldMatrix * ViewProjMatrix;

	SetMatrixParameter( m_pWorldViewMatrix, &WorldViewMatrix );
	SetMatrixParameter( m_pViewProjMatrix, &ViewProjMatrix );
	SetMatrixParameter( m_pWorldProjMatrix, &WorldProjMatrix );
	SetMatrixParameter( m_pWorldViewProjMatrix, &WorldViewProjMatrix );
}
//--------------------------------------------------------------------------------
void ParameterManagerDX11::SetProjMatrixParameter( Matrix4f* pMatrix )
{
	SetMatrixParameter( m_pProjMatrix, pMatrix );

	Matrix4f WorldMatrix = GetMatrixParameter( m_pWorldMatrix );
	Matrix4f ViewMatrix = GetMatrixParameter( m_pViewMatrix );
	Matrix4f ProjMatrix = GetMatrixParameter( m_pProjMatrix );

	Matrix4f WorldViewMatrix = WorldMatrix * ViewMatrix;
	Matrix4f ViewProjMatrix = ViewMatrix * ProjMatrix;
	Matrix4f WorldProjMatrix = WorldMatrix * ProjMatrix;
	Matrix4f WorldViewProjMatrix = WorldMatrix * ViewProjMatrix;

	SetMatrixParameter( m_pWorldViewMatrix, &WorldViewMatrix );
	SetMatrixParameter( m_pViewProjMatrix, &ViewProjMatrix );
	SetMatrixParameter( m_pWorldProjMatrix, &WorldProjMatrix );
	SetMatrixParameter( m_pWorldViewProjMatrix, &WorldViewProjMatrix );
}
//--------------------------------------------------------------------------------
void ParameterManagerDX11::AttachParent( IParameterManager* pParent )
{
	m_pParent = pParent;
}
//--------------------------------------------------------------------------------
void ParameterManagerDX11::DetachParent( )
{
	m_pParent = 0;
}
//--------------------------------------------------------------------------------
RenderParameterDX11* ParameterManagerDX11::GetParameterRef( const std::wstring& name )
{
	// First check this parameter manager
	RenderParameterDX11* pParam = m_Parameters[name];

	// Then check the parent manager
	if ( ( pParam == 0 ) && ( m_pParent ) )
		pParam = m_pParent->GetParameterRef( name );

	// Return the parameter
	return( pParam );
}
//--------------------------------------------------------------------------------
