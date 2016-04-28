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
#include "ShaderReflectionDX11.h"
#include "BufferConfigDX11.h"
#include "Log.h"
#include "GlyphString.h"
#include "IParameterManager.h"
#include "PipelineManagerDX11.h"
#include "ConstantBufferDX11.h"
#include "D3DEnumConversion.h"
#include "ShaderReflectionFactoryDX11.h"
#include "ConstantBufferDX11.h"
#include "EvtErrorMessage.h"
#include "EventManager.h"
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
ShaderReflectionDX11::ShaderReflectionDX11( )
{
}
//--------------------------------------------------------------------------------
ShaderReflectionDX11::~ShaderReflectionDX11()
{
}
//--------------------------------------------------------------------------------
void ShaderReflectionDX11::SetName( const std::wstring& name )
{
	Name = name;
}
//--------------------------------------------------------------------------------
std::wstring ShaderReflectionDX11::GetName( )
{
	return( Name );
}
//--------------------------------------------------------------------------------
void ShaderReflectionDX11::InitializeConstantBuffers( IParameterManager* pParamManager )
{
	for ( unsigned int i = 0; i < ConstantBuffers.size(); i++ )
	{
		if ( ConstantBuffers[i].Description.Type == D3D11_CT_CBUFFER )
		{
			// Get the index of the constant buffer parameter currently set with 
			// this name.

			int index = pParamManager->GetConstantBufferParameter( ConstantBuffers[i].pParamRef );

			// If the constant buffer does not exist yet, create a one with 
			// standard options - writeable by the CPU and only bound as a 
			// constant buffer.  By automatically creating the constant buffer
			// we reduce the amount of code to do common tasks, but still allow
			// the user to create and use a special buffer if they want.

			if ( index == -1 )
			{
				// Here we create one constant buffer for each thread that could potentially
				// be rendering, and then set each one accordingly within the parameter 
				// reference that we have for this buffer.

				for ( int thread = 0; thread < NUM_THREADS + 1; thread++ )
				{
					// Configure the buffer for the needed size and dynamic updating.
					BufferConfigDX11 cbuffer;
					cbuffer.SetDefaultConstantBuffer( ConstantBuffers[i].Description.Size, true );

					// Create the buffer and set it as a constant buffer parameter.  This
					// creates a parameter object to be used in the future.
					ResourcePtr resource = RendererDX11::Get()->CreateConstantBuffer( &cbuffer, 0 );
					index = resource->m_iResource;

					//ConstantBuffers[i].pParamRef->InitializeParameterData( &index );
					ConstantBuffers[i].pParamRef->SetParameterData( &index, thread );

					// Now we load the variable information into each of the constant
					// buffers.  This allows the buffer to know how to update itself
					// with the parameter system.

					for ( unsigned int j = 0; j < ConstantBuffers[i].Variables.size(); j++ )
					{
						ConstantBufferMapping mapping;

						mapping.pParameter = ConstantBuffers[i].Parameters[j];
						mapping.offset = ConstantBuffers[i].Variables[j].StartOffset;
						mapping.size = ConstantBuffers[i].Variables[j].Size;
						mapping.elements = ConstantBuffers[i].Types[j].Elements;
						mapping.varclass = ConstantBuffers[i].Types[j].Class;
						mapping.valueID = -1;

						ConstantBufferDX11* constBuffer = RendererDX11::Get()->GetConstantBufferByIndex( resource->m_iResource );
						constBuffer->AddMapping( mapping );
					}
				}
			} else {
				
				// If we get here, that means we are trying to initialize a constant buffer
				// that has already been set as a parameter before.  We need to compare if this
				// constant buffer has the same variables in it as the reflection data that we
				// are currently working with.  This can be done by getting the buffer, then
				// examining its mapping data and comparing that with the current reflection
				// provided mapping data.
				//
				// It is sufficient to check only one of the parameter's constant buffers, since
				// all of the buffers for multiple threads should be identical (as created above).

				ConstantBufferDX11* pConstBuffer = RendererDX11::Get()->GetConstantBufferByIndex( index );

				if ( pConstBuffer->GetAutoUpdate() ) {

					// Only check this buffer if we are auto-updating.  If not, then we are 
					// expecting something else to manage the contents of this buffer, and there
					// is no need for the mapping information.

					for ( unsigned int j = 0; j < ConstantBuffers[i].Variables.size(); j++ )
					{
						ConstantBufferMapping mapping;

						mapping.pParameter = ConstantBuffers[i].Parameters[j];
						mapping.offset = ConstantBuffers[i].Variables[j].StartOffset;
						mapping.size = ConstantBuffers[i].Variables[j].Size;
						mapping.elements = ConstantBuffers[i].Types[j].Elements;
						mapping.varclass = ConstantBuffers[i].Types[j].Class;
						mapping.valueID = -1;

						if ( !pConstBuffer->ContainsMapping( j, mapping ) ) {

							std::wstringstream message;
							message << L"Constant buffer by this name is already mapped, but with a different mapping!" << std::endl;
							message << L"See constant buffer: " << ConstantBuffers[i].pParamRef->GetName() << std::endl;
							message << L" - parameter: " << mapping.pParameter->GetName() << std::endl;
							message << L" - offset: "  << mapping.offset << std::endl;
							message << L" - size: " << mapping.size << std::endl;
							message << L" - elements: " << mapping.elements << std::endl;
							message << L" - class: " << TO_STRING_D3D_SHADER_VARIABLE_CLASS(mapping.varclass) << std::endl << std::endl;

							Log::Get().Write( message.str() );
							EventManager::Get()->ProcessEvent( EvtErrorMessagePtr( new EvtErrorMessage( message.str() ) ) );

						}
					}
				}
			}
		}
	}
}
//--------------------------------------------------------------------------------
void ShaderReflectionDX11::UpdateParameters( PipelineManagerDX11* pPipeline, IParameterManager* pParamManager )
{
	// Renderer will call this function when binding the shader to the pipeline.
	// This function will then call renderer functions to update each constant
	// buffer needed for the shader.

	for ( unsigned int i = 0; i < ConstantBuffers.size(); i++ )
	{
		if ( ConstantBuffers[i].Description.Type == D3D11_CT_CBUFFER )
		{
			// Get the index of the constant buffer parameter currently set with 
			// this name.

			int index = pParamManager->GetConstantBufferParameter( ConstantBuffers[i].pParamRef );

			// If the constant buffer does not exist yet, create a one with 
			// standard options - writeable by the CPU and only bound as a 
			// constant buffer.  By automatically creating the constant buffer
			// we reduce the amount of code to do common tasks, but still allow
			// the user to create and use a special buffer if they want.

			if ( index == -1 )
			{
				// This section of the code should never be reached anymore - all CBs should
				// be initially created when a shader is compiled if it doesn't already exist.
				// If we do end up here, send a message about it!
				Log::Get().Write( L"Uh oh - creating a constant buffer in the ShaderDX11::UpdateParameters functions!!!!" );

				// Configure the buffer for the needed size and dynamic updating.
				BufferConfigDX11 cbuffer;
				cbuffer.SetDefaultConstantBuffer( ConstantBuffers[i].Description.Size, true );

				// Create the buffer and set it as a constant buffer parameter.  This
				// creates a parameter object to be used in the future.
				ResourcePtr resource = RendererDX11::Get()->CreateConstantBuffer( &cbuffer, 0 );
				index = resource->m_iResource;

				pParamManager->SetConstantBufferParameter( ConstantBuffers[i].pParamRef, resource );
			}


			// Check if the resource is a constant buffer before accessing it!

			ConstantBufferDX11* pBuffer = RendererDX11::Get()->GetConstantBufferByIndex( index );

			// Test the index to ensure that it is a constant buffer.  If the method above returns
			// a non-null result, then this is a constant buffer.

			if ( pBuffer ) {

				// Here all of the individual variables from the reflection data are used to
				// update the data held by the constant buffer.

				if ( pBuffer->GetAutoUpdate() ) {
					pBuffer->EvaluateMappings( pPipeline, pParamManager );
				}

			} else {
				Log::Get().Write( L"Trying to update a constant buffer that isn't a constant buffer!" );
			}
		}
	}
}
//--------------------------------------------------------------------------------
void ShaderReflectionDX11::BindParameters( ShaderType type, PipelineManagerDX11* pPipeline, IParameterManager* pParamManager )
{
	// Here the shader will attempt to bind each parameter needed by the pipeline.
	// The renderer supplies methods for binding a parameter based on the name
	// and type of parameter, in addition to the pipeline stage to bind the 
	// parameter to.

	for ( unsigned int i = 0; i < ResourceBindings.size(); i++ )
	{
		UINT slot = ResourceBindings[i].BindPoint;

		switch ( ResourceBindings[i].Type )
		{
		case D3D_SIT_CBUFFER:
		case D3D_SIT_TBUFFER:
			pPipeline->BindConstantBufferParameter( type, ResourceBindings[i].pParamRef, slot, pParamManager );
			break;
		case D3D_SIT_SAMPLER:
			pPipeline->BindSamplerStateParameter( type, ResourceBindings[i].pParamRef, slot, pParamManager );
			break;
		case D3D_SIT_TEXTURE:
		case D3D_SIT_STRUCTURED:
		case D3D_SIT_BYTEADDRESS:
			pPipeline->BindShaderResourceParameter( type, ResourceBindings[i].pParamRef, slot, pParamManager );
			break;
		case D3D_SIT_UAV_RWSTRUCTURED:
		case D3D_SIT_UAV_RWTYPED:
		case D3D_SIT_UAV_RWBYTEADDRESS:
		case D3D_SIT_UAV_APPEND_STRUCTURED:
		case D3D_SIT_UAV_CONSUME_STRUCTURED:
		case D3D_SIT_UAV_RWSTRUCTURED_WITH_COUNTER:
			pPipeline->BindUnorderedAccessParameter( type, ResourceBindings[i].pParamRef, slot, pParamManager );
			break;
		}
	}
}
//--------------------------------------------------------------------------------
void ShaderReflectionDX11::PrintShaderDetails()
{
	std::wstringstream s;

	s << L"----------------------------------------------------------------------" << std::endl;
	s << L"Shader details printout for: " << Name << std::endl;
	s << L"----------------------------------------------------------------------" << std::endl;

	s << L"Shader Description: " << std::endl;
	s << TO_STRING_D3D11_SHADER_DESC( this->ShaderDescription );
	
	s << std::endl;

	s << L"Number of Input Signature Elements: " << InputSignatureParameters.size() << std::endl;
	for ( unsigned int i = 0; i < InputSignatureParameters.size(); i++ )
	{
		s << L"  Semantic Name: " << InputSignatureParameters[i].SemanticName;
		s << L", Semantic Index: " << InputSignatureParameters[i].SemanticIndex;
		s << L", Register: " << InputSignatureParameters[i].Register;
		s << L", System Value Type: " << TO_STRING_D3D_NAME( InputSignatureParameters[i].SystemValueType ); 
		s << L", Component Type: " << TO_STRING_D3D_REGISTER_COMPONENT_TYPE( InputSignatureParameters[i].ComponentType );
		s << L", Mask: " << (unsigned int)InputSignatureParameters[i].Mask;
		s << L", Read/Write Mask: " << (unsigned int)InputSignatureParameters[i].ReadWriteMask;
		s << std::endl;
	}
	s << std::endl;

	s << L"Number of Output Signature Elements: " << OutputSignatureParameters.size() << std::endl;
	for ( unsigned int i = 0; i < OutputSignatureParameters.size(); i++ )
	{
		s << L"  Semantic Name: " << OutputSignatureParameters[i].SemanticName;
		s << L", Semantic Index: " << OutputSignatureParameters[i].SemanticIndex;
		s << L", Register: " << OutputSignatureParameters[i].Register;
		s << L", System Value Type: " << TO_STRING_D3D_NAME( OutputSignatureParameters[i].SystemValueType );  
		s << L", Component Type: " << TO_STRING_D3D_REGISTER_COMPONENT_TYPE( OutputSignatureParameters[i].ComponentType );
		s << L", Mask: " << (unsigned int)OutputSignatureParameters[i].Mask;
		s << L", Read/Write Mask: " << (unsigned int)OutputSignatureParameters[i].ReadWriteMask;
		s << std::endl;
	}
	s << std::endl;

	s << L"Number of Constant Buffer Descriptions: " << ConstantBuffers.size() << std::endl;
	for ( unsigned int i = 0; i < ConstantBuffers.size(); i++ )
	{
		s << L"  Buffer Name: " << ConstantBuffers[i].Description.Name;
		s << L", Buffer Type: " << TO_STRING_D3D_CBUFFER_TYPE( ConstantBuffers[i].Description.Type );
		s << L", Variables: " << ConstantBuffers[i].Description.Variables;
		s << L", Size: " << ConstantBuffers[i].Description.Size;
		s << L", Flags: " << ConstantBuffers[i].Description.uFlags;
		
		for ( unsigned int j = 0; j < ConstantBuffers[i].Variables.size(); j++ )
		{
			s << std::endl << L"    ";
			s << L"Variable Name: " << ConstantBuffers[i].Variables[j].Name;
			s << L", Start Offset: " << ConstantBuffers[i].Variables[j].StartOffset;
			s << L", Size: " << ConstantBuffers[i].Variables[j].Size;
			s << L", Flags: " << ConstantBuffers[i].Variables[j].uFlags;
			s << L", Start Texture: " << (int)ConstantBuffers[i].Variables[j].StartTexture;
			s << L", Texture Size: " << ConstantBuffers[i].Variables[j].TextureSize;
			s << L", Start Sampler: " << (int)ConstantBuffers[i].Variables[j].StartSampler;
			s << L", Sampler Size: " << ConstantBuffers[i].Variables[j].SamplerSize;
		}


		for ( unsigned int j = 0; j < ConstantBuffers[i].Types.size(); j++ )
		{
			s << std::endl << L"    ";
			s << L"Variable Type Name: " << ConstantBuffers[i].Types[j].Name;
			s << L", Class: " << TO_STRING_D3D_SHADER_VARIABLE_CLASS( ConstantBuffers[i].Types[j].Class );
			s << L", Type: " << TO_STRING_D3D_SHADER_VARIABLE_TYPE( ConstantBuffers[i].Types[j].Type );			
			s << L", Rows: " << ConstantBuffers[i].Types[j].Rows;
			s << L", Columns: " << ConstantBuffers[i].Types[j].Columns;
			s << L", Elements: " << ConstantBuffers[i].Types[j].Elements;
			s << L", Members: " << ConstantBuffers[i].Types[j].Members;
			s << L", Offset: " << ConstantBuffers[i].Types[j].Offset;
		}
		
		s << std::endl;
	}
	s << std::endl;

	s << L"Number of Resource Binding Descriptions: " << ResourceBindings.size() << std::endl;
	for ( unsigned int i = 0; i < ResourceBindings.size(); i++ )
	{
		s << L"  Name: " << ResourceBindings[i].Name;
		s << L", Type: " << TO_STRING_D3D_SHADER_INPUT_TYPE( ResourceBindings[i].Type );
		s << L", Bind Point: " << ResourceBindings[i].BindPoint;
		s << L", Bind Count: " << ResourceBindings[i].BindCount;
		s << L", Flags: " << ResourceBindings[i].uFlags;
		s << L", Resource Return Type: " << TO_STRING_D3D_RESOURCE_RETURN_TYPE( ResourceBindings[i].ReturnType );
		s << L", Dimension: " << ResourceBindings[i].Dimension;
		s << L", Number of Samples: " << ResourceBindings[i].NumSamples;
		s << std::endl;
	}

	s << std::endl;

	//LPVOID pBlobBuffer = pCompiledShader->GetBufferPointer();
	//const char* pMessage = (const char*)pBlobBuffer;
	
	//s << pMessage << std::endl;

	s << L"----------------------------------------------------------------------" << std::endl;

	Log::Get().Write( s.str() );

}
//--------------------------------------------------------------------------------
