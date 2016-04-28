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
#include "ParameterContainer.h"
#include "Log.h"
#include "RenderParameterDX11.h"
#include "IParameterManager.h"
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
ParameterContainer::ParameterContainer()
{
}
//--------------------------------------------------------------------------------
ParameterContainer::~ParameterContainer()
{
	for ( auto pParameter : m_RenderParameters )
		delete pParameter;
}
//--------------------------------------------------------------------------------
void ParameterContainer::AddRenderParameter( ParameterWriter* pWriter )
{
	// Add render parameter will take the pointer passed in and add it to the
	// entity's internal list.  Therefore, this must not be from the stack!

	if ( pWriter )
	{
		// Search the list to see if this parameter is already there
		ParameterWriter* pCurr = 0;

		for ( unsigned int i = 0; i < m_RenderParameters.size(); i++ )
		{
			if ( pWriter->GetRenderParameterRef()->GetName() == m_RenderParameters[i]->GetRenderParameterRef()->GetName() )
			{
				pCurr = m_RenderParameters[i];
				break;
			}
		}

		if ( !pCurr )
		{
			m_RenderParameters.push_back( pWriter );
		}
		else
		{
			Log::Get().Write( L"Tried to add a parameter to an entity that was already there!" );
		}
	}
}
//--------------------------------------------------------------------------------
ParameterWriter* ParameterContainer::GetRenderParameter( const std::wstring& name )
{
	ParameterWriter* pResult = nullptr;

	for ( auto pParameterWriter : m_RenderParameters )
	{
		RenderParameterDX11* pParameter = pParameterWriter->GetRenderParameterRef();
		if ( pParameter != nullptr ) {
			if ( name.compare( pParameter->GetName() ) == 0 ) {
				pResult = pParameterWriter;
				break;
			}
		}
	}

	return( pResult );
}
//--------------------------------------------------------------------------------
ConstantBufferParameterWriterDX11* ParameterContainer::GetConstantBufferParameterWriter( const std::wstring& name )
{
	ParameterWriter* pWriter = nullptr;
	ConstantBufferParameterWriterDX11* pConstantBufferWriter = nullptr;

	// Check if the parameter already exists in this container.
	pWriter = GetRenderParameter( name );

	if ( nullptr != pWriter )
	{
		// The parameter is there, so now check its parameter type...
		RenderParameterDX11* pParameter = pWriter->GetRenderParameterRef();

		if ( nullptr != pParameter )
		{
			// If the type is correct, then set the value.
			if ( pParameter->GetParameterType() == CBUFFER ) {
				pConstantBufferWriter = dynamic_cast<ConstantBufferParameterWriterDX11*>( pWriter );
			} else {
				Log::Get().Write( L"ERROR: Trying to access a constant buffer in a non-CB parameter writer!!!" );
			}
		} else {
			// Parameter was there, but didn't have a reference set so you can't tell what type it is.
			// This shouldn't happen, so log an error if it does...
			Log::Get().Write( L"ERROR: Trying to access a parameter writer without any reference set!!!" );
		}
	}

	return( pConstantBufferWriter );
}
//--------------------------------------------------------------------------------
MatrixArrayParameterWriterDX11* ParameterContainer::GetMatrixArrayParameterWriter( const std::wstring& name )
{
	ParameterWriter* pWriter = nullptr;
	MatrixArrayParameterWriterDX11* pMatrixArrayWriter = nullptr;

	// Check if the parameter already exists in this container.
	pWriter = GetRenderParameter( name );

	if ( nullptr != pWriter )
	{
		// The parameter is there, so now check its parameter type...
		RenderParameterDX11* pParameter = pWriter->GetRenderParameterRef();

		if ( nullptr != pParameter )
		{
			// If the type is correct, then set the value.
			if ( pParameter->GetParameterType() == MATRIX_ARRAY ) {
				pMatrixArrayWriter = dynamic_cast<MatrixArrayParameterWriterDX11*>( pWriter );
			} else {
				Log::Get().Write( L"ERROR: Trying to access a matrix array in a non-matrix array parameter writer!!!" );
			}
		} else {
			// Parameter was there, but didn't have a reference set so you can't tell what type it is.
			// This shouldn't happen, so log an error if it does...
			Log::Get().Write( L"ERROR: Trying to access a parameter writer without any reference set!!!" );
		}
	}

	return( pMatrixArrayWriter );
}
//--------------------------------------------------------------------------------
MatrixParameterWriterDX11* ParameterContainer::GetMatrixParameterWriter( const std::wstring& name )
{
	ParameterWriter* pWriter = nullptr;
	MatrixParameterWriterDX11* pMatrixWriter = nullptr;

	// Check if the parameter already exists in this container.
	pWriter = GetRenderParameter( name );

	if ( nullptr != pWriter )
	{
		// The parameter is there, so now check its parameter type...
		RenderParameterDX11* pParameter = pWriter->GetRenderParameterRef();

		if ( nullptr != pParameter )
		{
			// If the type is correct, then set the value.
			if ( pParameter->GetParameterType() == MATRIX ) {
				pMatrixWriter = dynamic_cast<MatrixParameterWriterDX11*>( pWriter );
			} else {
				Log::Get().Write( L"ERROR: Trying to access a matrix in a non-matrix parameter writer!!!" );
			}
		} else {
			// Parameter was there, but didn't have a reference set so you can't tell what type it is.
			// This shouldn't happen, so log an error if it does...
			Log::Get().Write( L"ERROR: Trying to access a parameter writer without any reference set!!!" );
		}
	}

	return( pMatrixWriter );
}
//--------------------------------------------------------------------------------
SamplerParameterWriterDX11* ParameterContainer::GetSamplerParameterWriter( const std::wstring& name )
{
	ParameterWriter* pWriter = nullptr;
	SamplerParameterWriterDX11* pSamplerWriter = nullptr;

	// Check if the parameter already exists in this container.
	pWriter = GetRenderParameter( name );

	if ( nullptr != pWriter )
	{
		// The parameter is there, so now check its parameter type...
		RenderParameterDX11* pParameter = pWriter->GetRenderParameterRef();

		if ( nullptr != pParameter )
		{
			// If the type is correct, then set the value.
			if ( pParameter->GetParameterType() == SAMPLER ) {
				pSamplerWriter = dynamic_cast<SamplerParameterWriterDX11*>( pWriter );
			} else {
				Log::Get().Write( L"ERROR: Trying to access a sampler in a non-sampler parameter writer!!!" );
			}
		} else {
			// Parameter was there, but didn't have a reference set so you can't tell what type it is.
			// This shouldn't happen, so log an error if it does...
			Log::Get().Write( L"ERROR: Trying to access a parameter writer without any reference set!!!" );
		}
	}

	return( pSamplerWriter );
}
//--------------------------------------------------------------------------------
ShaderResourceParameterWriterDX11* ParameterContainer::GetShaderResourceParameterWriter( const std::wstring& name )
{
	ParameterWriter* pWriter = nullptr;
	ShaderResourceParameterWriterDX11* pShaderResourceWriter = nullptr;

	// Check if the parameter already exists in this container.
	pWriter = GetRenderParameter( name );

	if ( nullptr != pWriter )
	{
		// The parameter is there, so now check its parameter type...
		RenderParameterDX11* pParameter = pWriter->GetRenderParameterRef();

		if ( nullptr != pParameter )
		{
			// If the type is correct, then set the value.
			if ( pParameter->GetParameterType() == SHADER_RESOURCE ) {
				pShaderResourceWriter = dynamic_cast<ShaderResourceParameterWriterDX11*>( pWriter );
			} else {
				Log::Get().Write( L"ERROR: Trying to access a shader resource in a non-shader resource parameter writer!!!" );
			}
		} else {
			// Parameter was there, but didn't have a reference set so you can't tell what type it is.
			// This shouldn't happen, so log an error if it does...
			Log::Get().Write( L"ERROR: Trying to access a parameter writer without any reference set!!!" );
		}
	}

	return( pShaderResourceWriter );
}
//--------------------------------------------------------------------------------
UnorderedAccessParameterWriterDX11* ParameterContainer::GetUnorderedAccessParameterWriter( const std::wstring& name )
{
	ParameterWriter* pWriter = nullptr;
	UnorderedAccessParameterWriterDX11* pUnorderedAccessWriter = nullptr;

	// Check if the parameter already exists in this container.
	pWriter = GetRenderParameter( name );

	if ( nullptr != pWriter )
	{
		// The parameter is there, so now check its parameter type...
		RenderParameterDX11* pParameter = pWriter->GetRenderParameterRef();

		if ( nullptr != pParameter )
		{
			// If the type is correct, then set the value.
			if ( pParameter->GetParameterType() == UNORDERED_ACCESS ) {
				pUnorderedAccessWriter = dynamic_cast<UnorderedAccessParameterWriterDX11*>( pWriter );
			} else {
				Log::Get().Write( L"ERROR: Trying to access a unordered access view in a non-UAV parameter writer!!!" );
			}
		} else {
			// Parameter was there, but didn't have a reference set so you can't tell what type it is.
			// This shouldn't happen, so log an error if it does...
			Log::Get().Write( L"ERROR: Trying to access a parameter writer without any reference set!!!" );
		}
	}

	return( pUnorderedAccessWriter );
}
//--------------------------------------------------------------------------------
VectorParameterWriterDX11* ParameterContainer::GetVectorParameterWriter( const std::wstring& name )
{
	ParameterWriter* pWriter = nullptr;
	VectorParameterWriterDX11* pVectorWriter = nullptr;

	// Check if the parameter already exists in this container.
	pWriter = GetRenderParameter( name );

	if ( nullptr != pWriter )
	{
		// The parameter is there, so now check its parameter type...
		RenderParameterDX11* pParameter = pWriter->GetRenderParameterRef();

		if ( nullptr != pParameter )
		{
			// If the type is correct, then set the value.
			if ( pParameter->GetParameterType() == VECTOR) {
				pVectorWriter = dynamic_cast<VectorParameterWriterDX11*>( pWriter );
			} else {
				Log::Get().Write( L"ERROR: Trying to access a vector in a non-vector parameter writer!!!" );
			}
		} else {
			// Parameter was there, but didn't have a reference set so you can't tell what type it is.
			// This shouldn't happen, so log an error if it does...
			Log::Get().Write( L"ERROR: Trying to access a parameter writer without any reference set!!!" );
		}
	}

	return( pVectorWriter );
}
//--------------------------------------------------------------------------------
void ParameterContainer::SetRenderParams( IParameterManager* pParamManager )
{
	// Scroll through each parameter and set it in the provided parameter manager.
	for ( auto pParamWriter : m_RenderParameters )
		pParamWriter->WriteParameter( pParamManager );
}
//--------------------------------------------------------------------------------
void ParameterContainer::InitRenderParams( )
{
	// Scroll through each parameter and initialize its value.
	for ( auto pParamWriter : m_RenderParameters )
		pParamWriter->InitializeParameter( );
}
//--------------------------------------------------------------------------------
ConstantBufferParameterWriterDX11* ParameterContainer::SetConstantBufferParameter( const std::wstring& name, const ResourcePtr& value )
{
	// Check if the parameter already exists in this container
	ConstantBufferParameterWriterDX11* pConstantBufferWriter = GetConstantBufferParameterWriter( name );

	// If not, then create one...
	if ( nullptr == pConstantBufferWriter ) {
		pConstantBufferWriter = new ConstantBufferParameterWriterDX11();
		pConstantBufferWriter->SetRenderParameterRef( RendererDX11::Get()->m_pParamMgr->GetConstantBufferParameterRef( name ) );
		AddRenderParameter( pConstantBufferWriter );
	}

	// ... and set the value.
	pConstantBufferWriter->SetValue( value );

	return( pConstantBufferWriter );
}
//--------------------------------------------------------------------------------
MatrixArrayParameterWriterDX11* ParameterContainer::SetMatrixArrayParameter( const std::wstring& name, Matrix4f* values, int count )
{
	// Check if the parameter already exists in this container
	MatrixArrayParameterWriterDX11* pMatrixArrayWriter = GetMatrixArrayParameterWriter( name );

	// If not, then create one...
	if ( nullptr == pMatrixArrayWriter ) {
		pMatrixArrayWriter = new MatrixArrayParameterWriterDX11();
		pMatrixArrayWriter->SetRenderParameterRef( RendererDX11::Get()->m_pParamMgr->GetMatrixArrayParameterRef( name, count ) );
		AddRenderParameter( pMatrixArrayWriter );
	}

	// ... and set the value.
	pMatrixArrayWriter->SetValue( values );

	return( pMatrixArrayWriter );
}
//--------------------------------------------------------------------------------
MatrixParameterWriterDX11* ParameterContainer::SetMatrixParameter( const std::wstring& name, const Matrix4f& value )
{
	// Check if the parameter already exists in this container
	MatrixParameterWriterDX11* pMatrixWriter = GetMatrixParameterWriter( name );

	// If not, then create one...
	if ( nullptr == pMatrixWriter ) {
		pMatrixWriter = new MatrixParameterWriterDX11();
		pMatrixWriter->SetRenderParameterRef( RendererDX11::Get()->m_pParamMgr->GetMatrixParameterRef( name ) );
		AddRenderParameter( pMatrixWriter );
	}

	// ... and set the value.
	pMatrixWriter->SetValue( value );

	return( pMatrixWriter );
}
//--------------------------------------------------------------------------------
SamplerParameterWriterDX11* ParameterContainer::SetSamplerParameter( const std::wstring& name, int value )
{
	// Check if the parameter already exists in this container
	SamplerParameterWriterDX11* pSamplerWriter = GetSamplerParameterWriter( name );

	// If not, then create one...
	if ( nullptr == pSamplerWriter ) {
		pSamplerWriter = new SamplerParameterWriterDX11();
		pSamplerWriter->SetRenderParameterRef( RendererDX11::Get()->m_pParamMgr->GetSamplerStateParameterRef( name ) );
		AddRenderParameter( pSamplerWriter );
	}

	// ... and set the value.
	pSamplerWriter->SetValue( value );

	return( pSamplerWriter );
}
//--------------------------------------------------------------------------------
ShaderResourceParameterWriterDX11* ParameterContainer::SetShaderResourceParameter( const std::wstring& name, const ResourcePtr& value )
{
	// Check if the parameter already exists in this container
	ShaderResourceParameterWriterDX11* pShaderResourceWriter = GetShaderResourceParameterWriter( name );

	// If not, then create one...
	if ( nullptr == pShaderResourceWriter ) {
		pShaderResourceWriter = new ShaderResourceParameterWriterDX11();
		pShaderResourceWriter->SetRenderParameterRef( RendererDX11::Get()->m_pParamMgr->GetShaderResourceParameterRef( name ) );
		AddRenderParameter( pShaderResourceWriter );
	}

	// ... and set the value.
	pShaderResourceWriter->SetValue( value );

	return( pShaderResourceWriter );
}
//--------------------------------------------------------------------------------
UnorderedAccessParameterWriterDX11* ParameterContainer::SetUnorderedAccessParameter( const std::wstring& name, const ResourcePtr& value )
{
	// Check if the parameter already exists in this container
	UnorderedAccessParameterWriterDX11* pUnorderedAccessWriter = GetUnorderedAccessParameterWriter( name );

	// If not, then create one...
	if ( nullptr == pUnorderedAccessWriter ) {
		pUnorderedAccessWriter = new UnorderedAccessParameterWriterDX11();
		pUnorderedAccessWriter->SetRenderParameterRef( RendererDX11::Get()->m_pParamMgr->GetUnorderedAccessParameterRef( name ) );
		AddRenderParameter( pUnorderedAccessWriter );
	}

	// ... and set the value.
	pUnorderedAccessWriter->SetValue( value );

	return( pUnorderedAccessWriter );
}
//--------------------------------------------------------------------------------
VectorParameterWriterDX11* ParameterContainer::SetVectorParameter( const std::wstring& name, const Vector4f& value )
{
	// Check if the parameter already exists in this container
	VectorParameterWriterDX11* pVectorWriter = GetVectorParameterWriter( name );

	// If not, then create one...
	if ( nullptr == pVectorWriter ) {
		pVectorWriter = new VectorParameterWriterDX11();
		pVectorWriter->SetRenderParameterRef( RendererDX11::Get()->m_pParamMgr->GetVectorParameterRef( name ) );
		AddRenderParameter( pVectorWriter );
	}

	// ... and set the value.
	pVectorWriter->SetValue( value );

	return( pVectorWriter );
}
//--------------------------------------------------------------------------------
