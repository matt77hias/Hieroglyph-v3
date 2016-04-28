//--------------------------------------------------------------------------------
// This file is a portion of the Hieroglyph 3 Rendering Engine.  It is distributed
// under the MIT License, available in the root of this distribution and 
// at the following URL:
//
// http://www.opensource.org/licenses/mit-license.php
//
// Copyright (c) Jason Zink 
//--------------------------------------------------------------------------------
#pragma warning( disable : 4244 )
//--------------------------------------------------------------------------------
#include "PCH.h"
#include "ShaderReflectionFactoryDX11.h"
#include "Log.h"
#include "IParameterManager.h"
#include "RendererDX11.h"
//--------------------------------------------------------------------------------
using namespace Glyph3;
using Microsoft::WRL::ComPtr;
//--------------------------------------------------------------------------------
ShaderReflectionFactoryDX11::ShaderReflectionFactoryDX11( )
{
}
//--------------------------------------------------------------------------------
ShaderReflectionFactoryDX11::~ShaderReflectionFactoryDX11()
{
}
//--------------------------------------------------------------------------------
ShaderReflectionDX11* ShaderReflectionFactoryDX11::GenerateReflection( ShaderDX11& shader )
{
	// TODO: Fix this hack - I'm getting the parameter manager reference from the
	//       renderer here, but it shouldn't be needed at all here.  The 
	//       references should be resolved in an outside class or container.

	IParameterManager* pParamMgr = RendererDX11::Get()->m_pParamMgr;

	// Create the reflection interface to query information about the shader.  Note that
	// this reflection interface does not depend on the shader type!

	ShaderReflectionDX11* pReflection = new ShaderReflectionDX11();

	ComPtr<ID3D11ShaderReflection> pReflector;
	HRESULT hr = D3DReflect( shader.m_pCompiledShader->GetBufferPointer(), shader.m_pCompiledShader->GetBufferSize(),
		IID_ID3D11ShaderReflection, reinterpret_cast<void**>( pReflector.GetAddressOf() ));

	if ( FAILED( hr ) )
	{
		Log::Get().Write( L"Failed to create shader reflection interface!" );
		return( nullptr );
	}


	// Get the top level shader information, including the number of constant buffers,
	// as well as the number bound resources (constant buffers + other objects), the
	// number of input elements, and the number of output elements for the shader.

	D3D11_SHADER_DESC desc;
	pReflector->GetDesc( &desc );
	pReflection->ShaderDescription = desc;


	// Get the input and output signature description arrays.  These can be used later
	// for verification of linking shader stages together.
	for ( UINT i = 0; i < desc.InputParameters; i++ )
	{
		D3D11_SIGNATURE_PARAMETER_DESC input_desc;
		pReflector->GetInputParameterDesc( i, &input_desc );
		pReflection->InputSignatureParameters.push_back( input_desc );
	}
	for ( UINT i = 0; i < desc.OutputParameters; i++ )
	{
		D3D11_SIGNATURE_PARAMETER_DESC output_desc;
		pReflector->GetOutputParameterDesc( i, &output_desc );
		pReflection->OutputSignatureParameters.push_back( output_desc );
	}


	// Get the constant buffer information, which will be used for setting parameters
	// for use by this shader at rendering time.

	for ( UINT i = 0; i < desc.ConstantBuffers; i++ )
	{
		ID3D11ShaderReflectionConstantBuffer* pConstBuffer = pReflector->GetConstantBufferByIndex( i );
		
		D3D11_SHADER_BUFFER_DESC bufferDesc;
		pConstBuffer->GetDesc( &bufferDesc );
		
		if ( bufferDesc.Type == D3D_CT_CBUFFER || bufferDesc.Type == D3D_CT_TBUFFER )
		{
			ConstantBufferLayout BufferLayout;
			BufferLayout.Description = bufferDesc;
			BufferLayout.pParamRef = pParamMgr->GetConstantBufferParameterRef( 
				GlyphString::ToUnicode( std::string( BufferLayout.Description.Name ) ) );

			// Load the description of each variable for use later on when binding a buffer
			for ( UINT j = 0; j < BufferLayout.Description.Variables; j++ )
			{
				// Get the variable description and store it
				ID3D11ShaderReflectionVariable* pVariable = pConstBuffer->GetVariableByIndex( j );
				D3D11_SHADER_VARIABLE_DESC var_desc;
				pVariable->GetDesc( &var_desc );

				BufferLayout.Variables.push_back( var_desc );

				// Get the variable type description and store it
				ID3D11ShaderReflectionType* pType = pVariable->GetType();
				D3D11_SHADER_TYPE_DESC type_desc;
				pType->GetDesc( &type_desc );

				BufferLayout.Types.push_back( type_desc );

				// Get references to the parameters for binding to these variables.
				RenderParameterDX11* pParam = 0;
				if ( type_desc.Class == D3D_SVC_VECTOR )
				{
					pParam = pParamMgr->GetVectorParameterRef( GlyphString::ToUnicode( std::string( var_desc.Name ) ) );
				}
				else if ( ( type_desc.Class == D3D_SVC_MATRIX_ROWS ) ||
							( type_desc.Class == D3D_SVC_MATRIX_COLUMNS ) )
				{
					// Check if it is an array of matrices first...
					unsigned int count = type_desc.Elements;
					if ( count == 0 ) 
					{
						pParam = pParamMgr->GetMatrixParameterRef( GlyphString::ToUnicode( std::string( var_desc.Name ) ) );
					}
					else
					{
						pParam = pParamMgr->GetMatrixArrayParameterRef( GlyphString::ToUnicode( std::string( var_desc.Name ) ), count );
					}
				}

				BufferLayout.Parameters.push_back( pParam );
			}

			pReflection->ConstantBuffers.push_back( BufferLayout );
		}
	}


	// Get the overall resource binding information for this shader.  This includes
	// the constant buffers, plus all of the other objects that can be bound to the
	// pipeline with resource views (shader resource views and unordered access views).

	for ( UINT i = 0; i < desc.BoundResources; i++ )
	{
		D3D11_SHADER_INPUT_BIND_DESC resource_desc;
		pReflector->GetResourceBindingDesc( i, &resource_desc );
		ShaderInputBindDesc binddesc( resource_desc );

		if ( resource_desc.Type == D3D_SIT_CBUFFER || resource_desc.Type == D3D_SIT_TBUFFER )
		{
			binddesc.pParamRef = pParamMgr->GetConstantBufferParameterRef( binddesc.Name );
		}
		else if ( resource_desc.Type == D3D_SIT_TEXTURE || resource_desc.Type == D3D_SIT_STRUCTURED )
		{
			binddesc.pParamRef = pParamMgr->GetShaderResourceParameterRef( binddesc.Name );
		}
		else if ( resource_desc.Type == D3D_SIT_SAMPLER )
		{
			binddesc.pParamRef = pParamMgr->GetSamplerStateParameterRef( binddesc.Name );
		}
		else if ( resource_desc.Type == D3D_SIT_UAV_RWTYPED || resource_desc.Type == D3D_SIT_UAV_RWSTRUCTURED
			|| resource_desc.Type == D3D_SIT_BYTEADDRESS || resource_desc.Type == D3D_SIT_UAV_RWBYTEADDRESS
			|| resource_desc.Type == D3D_SIT_UAV_APPEND_STRUCTURED || resource_desc.Type == D3D_SIT_UAV_CONSUME_STRUCTURED
			|| resource_desc.Type == D3D_SIT_UAV_RWSTRUCTURED_WITH_COUNTER )
		{
			binddesc.pParamRef = pParamMgr->GetUnorderedAccessParameterRef( binddesc.Name );
		}


		pReflection->ResourceBindings.push_back( binddesc );
	}

	return( pReflection );
}
//--------------------------------------------------------------------------------
