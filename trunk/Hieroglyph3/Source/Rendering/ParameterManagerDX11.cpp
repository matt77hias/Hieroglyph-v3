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
ParameterManagerDX11::ParameterManagerDX11()
{
	m_pParent = 0;
}
//--------------------------------------------------------------------------------
ParameterManagerDX11::~ParameterManagerDX11()
{
	
}
//--------------------------------------------------------------------------------
void ParameterManagerDX11::SetVectorParameter( std::wstring name, Vector4f* pVector )
{
	RenderParameterDX11* pParameter = m_Parameters[name];

	// Only create the new parameter if it hasn't already been registered
	if ( pParameter == 0 )
	{
		pParameter = new VectorParameterDX11();
		pParameter->SetName( name );
		m_Parameters[name] = reinterpret_cast<RenderParameterDX11*>( pParameter );
	}

	if ( pParameter->GetParameterType() == VECTOR )
		pParameter->SetParameterData( reinterpret_cast<void*>( pVector ) );
	else
		Log::Get().Write( L"Vector parameter name collision!" );
}
//--------------------------------------------------------------------------------
void ParameterManagerDX11::SetMatrixParameter( std::wstring name, Matrix4f* pMatrix )
{
	RenderParameterDX11* pParameter = m_Parameters[name];

	// Only create the new parameter if it hasn't already been registered
	if ( pParameter == 0 )
	{
		pParameter = new MatrixParameterDX11();
		pParameter->SetName( name );
		m_Parameters[name] = reinterpret_cast<RenderParameterDX11*>( pParameter );
		
	}
	
	if ( pParameter->GetParameterType() == MATRIX )
		pParameter->SetParameterData( reinterpret_cast<void*>( pMatrix ) );
	else
		Log::Get().Write( L"Matrix parameter name collision!" );
}
//--------------------------------------------------------------------------------
void ParameterManagerDX11::SetMatrixArrayParameter( std::wstring name, int count, Matrix4f* pMatrix )
{
	RenderParameterDX11* pParameter = m_Parameters[name];

	// Only create the new parameter if it hasn't already been registered
	if ( pParameter == 0 )
	{
		pParameter = new MatrixArrayParameterDX11( count );
		pParameter->SetName( name );
		m_Parameters[name] = reinterpret_cast<RenderParameterDX11*>( pParameter );
	}
	
	if ( pParameter->GetParameterType() == MATRIX_ARRAY )
		pParameter->SetParameterData( reinterpret_cast<void*>( pMatrix ) );
	else
		Log::Get().Write( L"Matrix Array parameter name collision!" );
}
//--------------------------------------------------------------------------------
void ParameterManagerDX11::SetParameter( RenderParameterDX11* pParameter )
{
	std::wstring name = pParameter->GetName();
	RenderParameterDX11* pCurrent = m_Parameters[name];

	if ( pParameter )
	{
		if ( pParameter->GetParameterType() == VECTOR )
		{
			// Only create the new parameter if it hasn't already been registered
			if ( pCurrent == 0 )
			{
				pCurrent = new VectorParameterDX11();
				pCurrent->SetName( name );
				m_Parameters[name] = reinterpret_cast<RenderParameterDX11*>( pCurrent );
			}

			*reinterpret_cast<VectorParameterDX11*>( pCurrent )
				= *reinterpret_cast<VectorParameterDX11*>( pParameter );
		}
		if ( pParameter->GetParameterType() == MATRIX )
		{
			// Only create the new parameter if it hasn't already been registered
			if ( pCurrent == 0 )
			{
				pCurrent = new MatrixParameterDX11();
				pCurrent->SetName( name );
				m_Parameters[name] = reinterpret_cast<RenderParameterDX11*>( pCurrent );
			}

			*reinterpret_cast<MatrixParameterDX11*>( pCurrent )
				= *reinterpret_cast<MatrixParameterDX11*>( pParameter );
		}
		if ( pParameter->GetParameterType() == MATRIX_ARRAY )
		{
			// Only create the new parameter if it hasn't already been registered
			if ( pCurrent == 0 )
			{
				int count = reinterpret_cast<MatrixArrayParameterDX11*>( pParameter )->GetMatrixCount();
				pCurrent = new MatrixArrayParameterDX11( count );
				m_Parameters[name] = reinterpret_cast<RenderParameterDX11*>( pCurrent );
			}

			// This assignment performs a deep copy of the matrix array.
			*reinterpret_cast<MatrixArrayParameterDX11*>( pCurrent )
				= *reinterpret_cast<MatrixArrayParameterDX11*>( pParameter );
		}
		if ( pParameter->GetParameterType() == SHADER_RESOURCE )
		{
			// Only create the new parameter if it hasn't already been registered
			if ( pCurrent == 0 )
			{
				pCurrent = new ShaderResourceParameterDX11();
				pCurrent->SetName( name );
				m_Parameters[name] = reinterpret_cast<RenderParameterDX11*>( pCurrent );
			}

			*reinterpret_cast<ShaderResourceParameterDX11*>( pCurrent )
				= *reinterpret_cast<ShaderResourceParameterDX11*>( pParameter );
		}
		if ( pParameter->GetParameterType() == UNORDERED_ACCESS )
		{
			// Only create the new parameter if it hasn't already been registered
			if ( pCurrent == 0 )
			{
				pCurrent = new UnorderedAccessParameterDX11();
				pCurrent->SetName( name );
				m_Parameters[name] = reinterpret_cast<RenderParameterDX11*>( pCurrent );
			}

			*reinterpret_cast<UnorderedAccessParameterDX11*>( pCurrent )
				= *reinterpret_cast<UnorderedAccessParameterDX11*>( pParameter );
		}
		if ( pParameter->GetParameterType() == CBUFFER )
		{
			// Only create the new parameter if it hasn't already been registered
			if ( pCurrent == 0 )
			{
				pCurrent = new ConstantBufferParameterDX11();
				pCurrent->SetName( name );
				m_Parameters[name] = reinterpret_cast<RenderParameterDX11*>( pCurrent );
			}

			*reinterpret_cast<ConstantBufferParameterDX11*>( pCurrent )
				= *reinterpret_cast<ConstantBufferParameterDX11*>( pParameter );
		}
		if ( pParameter->GetParameterType() == SAMPLER )
		{
			// Only create the new parameter if it hasn't already been registered
			if ( pCurrent == 0 )
			{
				pCurrent = new SamplerParameterDX11();
				pCurrent->SetName( name );
				m_Parameters[name] = reinterpret_cast<RenderParameterDX11*>( pCurrent );
			}

			*reinterpret_cast<SamplerParameterDX11*>( pCurrent )
				= *reinterpret_cast<SamplerParameterDX11*>( pParameter );
		}
	}
}
//--------------------------------------------------------------------------------
void ParameterManagerDX11::SetShaderResourceParameter( std::wstring name, ResourcePtr resource )
{
	RenderParameterDX11* pParameter = m_Parameters[name];

	// Only create the new parameter if it hasn't already been registered
	if ( pParameter == 0 )
	{
		pParameter = new ShaderResourceParameterDX11();
		pParameter->SetName( name );
		m_Parameters[name] = reinterpret_cast<RenderParameterDX11*>( pParameter );
	}

	if ( pParameter->GetParameterType() == SHADER_RESOURCE )
		pParameter->SetParameterData( reinterpret_cast<void*>( &resource->m_iResourceSRV ) );
	else
		Log::Get().Write( L"Shader resource view parameter name collision!" );
}
//--------------------------------------------------------------------------------
void ParameterManagerDX11::SetUnorderedAccessParameter( std::wstring name, ResourcePtr resource )
{
	RenderParameterDX11* pParameter = m_Parameters[name];

	// Only create the new parameter if it hasn't already been registered
	if ( pParameter == 0 )
	{
		pParameter = new UnorderedAccessParameterDX11();
		pParameter->SetName( name );
		m_Parameters[name] = reinterpret_cast<RenderParameterDX11*>( pParameter );
	}

	if ( pParameter->GetParameterType() == UNORDERED_ACCESS )
		pParameter->SetParameterData( reinterpret_cast<void*>( &resource->m_iResourceUAV ) );
	else
		Log::Get().Write( L"Unordered access view parameter name collision!" );
}
//--------------------------------------------------------------------------------
void ParameterManagerDX11::SetConstantBufferParameter( std::wstring name, ResourcePtr resource )
{
	RenderParameterDX11* pParameter = m_Parameters[name];

	// Only create the new parameter if it hasn't already been registered
	if ( pParameter == 0 )
	{
		pParameter = new ConstantBufferParameterDX11();
		pParameter->SetName( name );
		m_Parameters[name] = reinterpret_cast<RenderParameterDX11*>( pParameter );
	}

	if ( pParameter->GetParameterType() == CBUFFER )
		pParameter->SetParameterData( reinterpret_cast<void*>( &resource->m_iResource ) );
	else
		Log::Get().Write( L"Constant buffer parameter name collision!" );
}
//--------------------------------------------------------------------------------
void ParameterManagerDX11::SetSamplerParameter( std::wstring name, int* pID )
{
	RenderParameterDX11* pParameter = m_Parameters[name];

	// Only create the new parameter if it hasn't already been registered
	if ( pParameter == 0 )
	{
		pParameter = new SamplerParameterDX11();
		pParameter->SetName( name );
		m_Parameters[name] = reinterpret_cast<RenderParameterDX11*>( pParameter );
	}

	if ( pParameter->GetParameterType() == SAMPLER )
		pParameter->SetParameterData( reinterpret_cast<void*>( pID ) );
	else
		Log::Get().Write( L"Sampler parameter name collision!" );
}
//--------------------------------------------------------------------------------
Vector4f ParameterManagerDX11::GetVectorParameter( std::wstring name )
{
	Vector4f result;
	result.MakeZero();

	// Check for the existence of this parameter.  This search includes any
	// parent parameter managers if the parameter doesn't exist in this one.

	RenderParameterDX11* pParam = GetParameter( name );

	// If the parameter is not found, create a new default one.  This goes 
	// into the bottom level manager.

	if ( pParam != 0 )
	{
		if ( pParam->GetParameterType() == VECTOR ) 
			result = reinterpret_cast<VectorParameterDX11*>( pParam )->GetVector();
	}
	else
	{
		pParam = new VectorParameterDX11();
		m_Parameters[name] = pParam;
	}

	return( result );
}
//--------------------------------------------------------------------------------
Matrix4f ParameterManagerDX11::GetMatrixParameter( std::wstring name )
{
	Matrix4f result;
	result.MakeZero();

	// Check for the existence of this parameter.  This search includes any
	// parent parameter managers if the parameter doesn't exist in this one.

	RenderParameterDX11* pParam = GetParameter( name );

	// If the parameter is not found, create a new default one.  This goes 
	// into the bottom level manager.

	if ( pParam != 0 )
	{
		if ( pParam->GetParameterType() == MATRIX ) 
			result = reinterpret_cast<MatrixParameterDX11*>( pParam )->GetMatrix();
	}
	else
	{
		pParam = new MatrixParameterDX11();
		m_Parameters[name] = pParam;
	}

	return( result );
}
//--------------------------------------------------------------------------------
Matrix4f* ParameterManagerDX11::GetMatrixArrayParameter( std::wstring name, int count )
{
	Matrix4f* pResult = 0;

	// Check for the existence of this parameter.  This search includes any
	// parent parameter managers if the parameter doesn't exist in this one.

	RenderParameterDX11* pParam = GetParameter( name );

	// If the parameter is not found, create a new default one.  This goes 
	// into the bottom level manager.

	if ( pParam != 0 )
	{
		if ( pParam->GetParameterType() == MATRIX_ARRAY ) 
			if ( reinterpret_cast<MatrixArrayParameterDX11*>( pParam )->GetMatrixCount() == count )
				pResult = reinterpret_cast<MatrixArrayParameterDX11*>( pParam )->GetMatrices();
	}
	else
	{
		pParam = new MatrixArrayParameterDX11( count );
		m_Parameters[name] = pParam;
		pResult = reinterpret_cast<MatrixArrayParameterDX11*>( pParam )->GetMatrices();
	}

	return( pResult );
}
//--------------------------------------------------------------------------------
int ParameterManagerDX11::GetShaderResourceParameter( std::wstring name )
{
	int result;
	result = -1;

	// Check for the existence of this parameter.  This search includes any
	// parent parameter managers if the parameter doesn't exist in this one.

	RenderParameterDX11* pParam = GetParameter( name );

	// If the parameter is not found, create a new default one.  This goes 
	// into the bottom level manager.

	if ( pParam != 0 )
	{
		if ( pParam->GetParameterType() == SHADER_RESOURCE ) 
			result = reinterpret_cast<ShaderResourceParameterDX11*>( pParam )->GetIndex();
	}
	else
	{
		pParam = new ShaderResourceParameterDX11();
		m_Parameters[name] = pParam;
	}

	return( result );
}
//--------------------------------------------------------------------------------
int ParameterManagerDX11::GetUnorderedAccessParameter( std::wstring name )
{
	int result;
	result = -1;

	// Check for the existence of this parameter.  This search includes any
	// parent parameter managers if the parameter doesn't exist in this one.

	RenderParameterDX11* pParam = GetParameter( name );

	// If the parameter is not found, create a new default one.  This goes 
	// into the bottom level manager.

	if ( pParam != 0 )
	{
		if ( pParam->GetParameterType() == UNORDERED_ACCESS ) 
			result = reinterpret_cast<UnorderedAccessParameterDX11*>( pParam )->GetIndex();
	}
	else
	{
		pParam = new UnorderedAccessParameterDX11();
		m_Parameters[name] = pParam;
	}

	return( result );
}
//--------------------------------------------------------------------------------
int ParameterManagerDX11::GetConstantBufferParameter( std::wstring name )
{
	int result;
	result = -1;

	// Check for the existence of this parameter.  This search includes any
	// parent parameter managers if the parameter doesn't exist in this one.

	RenderParameterDX11* pParam = GetParameter( name );

	// If the parameter is not found, create a new default one.  This goes 
	// into the bottom level manager.

	if ( pParam != 0 )
	{
		if ( pParam->GetParameterType() == CBUFFER ) 
			result = reinterpret_cast<ConstantBufferParameterDX11*>( pParam )->GetIndex();
	}
	else
	{
		pParam = new ConstantBufferParameterDX11();
		m_Parameters[name] = pParam;
	}

	return( result );
}
//--------------------------------------------------------------------------------
int ParameterManagerDX11::GetSamplerStateParameter( std::wstring name )
{
	int result;
	result = -1;

	// Check for the existence of this parameter.  This search includes any
	// parent parameter managers if the parameter doesn't exist in this one.

	RenderParameterDX11* pParam = GetParameter( name );

	// If the parameter is not found, create a new default one.  This goes 
	// into the bottom level manager.

	if ( pParam != 0 )
	{
		if ( pParam->GetParameterType() == SAMPLER ) 
			result = reinterpret_cast<SamplerParameterDX11*>( pParam )->GetIndex();
	}
	else
	{
		pParam = new SamplerParameterDX11();
		m_Parameters[name] = pParam;
	}

	return( result );	
}
//--------------------------------------------------------------------------------
void ParameterManagerDX11::SetWorldMatrixParameter( Matrix4f* pMatrix )
{
	SetMatrixParameter( std::wstring( L"WorldMatrix" ), pMatrix );

	Matrix4f WorldMatrix = GetMatrixParameter( std::wstring( L"WorldMatrix" ) );
	Matrix4f ViewMatrix = GetMatrixParameter( std::wstring( L"ViewMatrix" ) );
	Matrix4f ProjMatrix = GetMatrixParameter( std::wstring( L"ProjMatrix" ) );

	Matrix4f WorldViewMatrix = WorldMatrix * ViewMatrix;
	Matrix4f ViewProjMatrix = ViewMatrix * ProjMatrix;
	Matrix4f WorldViewProjMatrix = WorldMatrix * ViewProjMatrix;

	SetMatrixParameter( std::wstring( L"WorldViewMatrix" ), &WorldViewMatrix );
	SetMatrixParameter( std::wstring( L"ViewProjMatrix" ), &ViewProjMatrix );
	SetMatrixParameter( std::wstring( L"WorldViewProjMatrix" ), &WorldViewProjMatrix );
}
//--------------------------------------------------------------------------------
void ParameterManagerDX11::SetViewMatrixParameter( Matrix4f* pMatrix )
{
	SetMatrixParameter( std::wstring( L"ViewMatrix" ), pMatrix );

	Matrix4f WorldMatrix = GetMatrixParameter( std::wstring( L"WorldMatrix" ) );
	Matrix4f ViewMatrix = GetMatrixParameter( std::wstring( L"ViewMatrix" ) );
	Matrix4f ProjMatrix = GetMatrixParameter( std::wstring( L"ProjMatrix" ) );

	Matrix4f WorldViewMatrix = WorldMatrix * ViewMatrix;
	Matrix4f ViewProjMatrix = ViewMatrix * ProjMatrix;
	Matrix4f WorldViewProjMatrix = WorldMatrix * ViewProjMatrix;

	SetMatrixParameter( std::wstring( L"WorldViewMatrix" ), &WorldViewMatrix );
	SetMatrixParameter( std::wstring( L"ViewProjMatrix" ), &ViewProjMatrix );
	SetMatrixParameter( std::wstring( L"WorldViewProjMatrix" ), &WorldViewProjMatrix );
}
//--------------------------------------------------------------------------------
void ParameterManagerDX11::SetProjMatrixParameter( Matrix4f* pMatrix )
{
	SetMatrixParameter( std::wstring( L"ProjMatrix" ), pMatrix );

	Matrix4f WorldMatrix = GetMatrixParameter( std::wstring( L"WorldMatrix" ) );
	Matrix4f ViewMatrix = GetMatrixParameter( std::wstring( L"ViewMatrix" ) );
	Matrix4f ProjMatrix = GetMatrixParameter( std::wstring( L"ProjMatrix" ) );

	Matrix4f WorldViewMatrix = WorldMatrix * ViewMatrix;
	Matrix4f ViewProjMatrix = ViewMatrix * ProjMatrix;
	Matrix4f WorldViewProjMatrix = WorldMatrix * ViewProjMatrix;

	SetMatrixParameter( std::wstring( L"WorldViewMatrix" ), &WorldViewMatrix );
	SetMatrixParameter( std::wstring( L"ViewProjMatrix" ), &ViewProjMatrix );
	SetMatrixParameter( std::wstring( L"WorldViewProjMatrix" ), &WorldViewProjMatrix );
}
//--------------------------------------------------------------------------------
void ParameterManagerDX11::AttachParent( ParameterManagerDX11* pParent )
{
	m_pParent = pParent;
}
//--------------------------------------------------------------------------------
void ParameterManagerDX11::DetachParent( )
{
	m_pParent = 0;
}
//--------------------------------------------------------------------------------
RenderParameterDX11* ParameterManagerDX11::GetParameter( std::wstring name )
{
	// First check this parameter manager
	RenderParameterDX11* pParam = m_Parameters[name];

	// Then check the parent manager
	if ( ( pParam == 0 ) && ( m_pParent ) )
		pParam = m_pParent->GetParameter( name );

	// Return the parameter
	return( pParam );
}
//--------------------------------------------------------------------------------
