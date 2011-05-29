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
#include "ShaderDX11.h"
#include "BufferConfigDX11.h"
#include "Log.h"
#include "GlyphString.h"
#include "IParameterManager.h"
#include "PipelineManagerDX11.h"
#include "ConstantBufferDX11.h"
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
ShaderDX11::ShaderDX11()
{
}
//--------------------------------------------------------------------------------
ShaderDX11::~ShaderDX11()
{

}
//--------------------------------------------------------------------------------
void ShaderDX11::UpdateParameters( PipelineManagerDX11* pPipeline, IParameterManager* pParamManager )
{
	// Renderer will call this function when binding the shader to the pipeline.
	// This function will then call renderer functions to update each constant
	// buffer needed for the shader.

	for ( int i = 0; i < ConstantBuffers.count(); i++ )
	{
		if ( ConstantBuffers[i].Description.Type == D3D11_CT_CBUFFER )
		{
			// Get the index of the constant buffer parameter currently set with 
			// this name.

			int index = pParamManager->GetConstantBufferParameter( ConstantBuffers[i].pParamRef );
//			int index = pParamManager->GetConstantBufferParameter( ConstantBuffers[i].Description.Name );

			// If the constant buffer does not exist yet, create a one with 
			// standard options - writeable by the CPU and only bound as a 
			// constant buffer.  By automatically creating the constant buffer
			// we reduce the amount of code to do common tasks, but still allow
			// the user to create and use a special buffer if they want.

			if ( index == -1 )
			{
				// Configure the buffer for the needed size and dynamic updating.
				BufferConfigDX11 cbuffer;
				cbuffer.SetDefaultConstantBuffer( ConstantBuffers[i].Description.Size, true );

				// Create the buffer and set it as a constant buffer parameter.  This
				// creates a parameter object to be used in the future.
				ResourcePtr resource = RendererDX11::Get()->CreateConstantBuffer( &cbuffer, 0 );
				index = resource->m_iResource;

				pParamManager->SetConstantBufferParameter( ConstantBuffers[i].pParamRef, resource );
				//pParamManager->SetConstantBufferParameter( ConstantBuffers[i].Description.Name, resource );
			}

			// Test the index to ensure that it is a constant buffer
			if ( (index & 0x00FF0000) == RT_CONSTANTBUFFER ) 
			{
				// TODO: Check if the resource is a constant buffer before accessing it!
				ConstantBufferDX11* pBuffer = (ConstantBufferDX11*)RendererDX11::Get()->GetResource( index & 0x0000FFFF );
				
				if ( pBuffer->GetAutoUpdate() )
				{
					// Map the constant buffer into system memory.  We map the buffer 
					// with the discard write flag since we don't care what was in the
					// buffer already.

					D3D11_MAPPED_SUBRESOURCE resource = 
						pPipeline->MapResource( index, 0, D3D11_MAP_WRITE_DISCARD, 0 );

					// Update each variable in the constant buffer.  These variables are identified
					// by their type, and are currently allowed to be Vector4f, Matrix4f, or Matrix4f
					// arrays.  Additional types will be added as they are needed...

					for ( int j = 0; j < ConstantBuffers[i].Variables.count(); j++ )
					{
						RenderParameterDX11* pParam = ConstantBuffers[i].Parameters[j];
						//const std::wstring& name = ConstantBuffers[i].Variables[j].Name;
						int offset = ConstantBuffers[i].Variables[j].StartOffset;
						UINT size = ConstantBuffers[i].Variables[j].Size;
						
						if ( ConstantBuffers[i].Types[j].Class == D3D10_SVC_VECTOR )
						{
							Vector4f vector = pParamManager->GetVectorParameter( pParam );
							Vector4f* pBuf = (Vector4f*)((char*)resource.pData + offset);
							*pBuf = vector;
						}
						else if ( ( ConstantBuffers[i].Types[j].Class == D3D10_SVC_MATRIX_ROWS ) ||
							( ConstantBuffers[i].Types[j].Class == D3D10_SVC_MATRIX_COLUMNS ) )
						{
							// Check if it is an array of matrices first...
							unsigned int count = ConstantBuffers[i].Types[j].Elements;
							if ( count == 0 ) 
							{
								Matrix4f matrix = pParamManager->GetMatrixParameter( pParam );
								Matrix4f* pBuf = (Matrix4f*)((char*)resource.pData + offset);
								*pBuf = matrix;
							}
							else 
							{
								// If a matrix array, then use the corresponding parameter type.
								// TODO: If the shader expects a different number of matrices than are present
								//       from the matrix array parameter, then this causes an exception!
								Matrix4f* pMatrices = pParamManager->GetMatrixArrayParameter( pParam );
								memcpy( ((char*)resource.pData + offset), (char*)pMatrices, ConstantBuffers[i].Variables[j].Size );
							}
						}
						else
						{
							Log::Get().Write( L"Non vector or matrix parameter specified in a constant buffer!  This will not be updated!" );
						}
					}

					pPipeline->UnMapResource( index, 0 );
				}
			}
			else
			{
				Log::Get().Write( L"Trying to update a constant buffer that isn't a constant buffer!" );
			}
		}
	}
}
//--------------------------------------------------------------------------------
void ShaderDX11::BindParameters( PipelineManagerDX11* pPipeline, IParameterManager* pParamManager )
{
	// Here the shader will attempt to bind each parameter needed by the pipeline.
	// The renderer supplies methods for binding a parameter based on the name
	// and type of parameter, in addition to the pipeline stage to bind the 
	// parameter to.

	for ( int i = 0; i < ResourceBindings.count(); i++ )
	{
		//const std::wstring& name = ResourceBindings[i].Name;
		UINT slot = ResourceBindings[i].BindPoint;

		switch ( ResourceBindings[i].Type )
		{
		case D3D10_SIT_CBUFFER:
			pPipeline->BindConstantBufferParameter( GetType(), ResourceBindings[i].pParamRef, slot, pParamManager );
			break;
		case D3D10_SIT_TBUFFER:
			pPipeline->BindConstantBufferParameter( GetType(), ResourceBindings[i].pParamRef, slot, pParamManager );
			break;
		case D3D10_SIT_TEXTURE:
			pPipeline->BindShaderResourceParameter( GetType(), ResourceBindings[i].pParamRef, slot, pParamManager );
			break;
		case D3D10_SIT_SAMPLER:
			pPipeline->BindSamplerStateParameter( GetType(), ResourceBindings[i].pParamRef, slot, pParamManager );
			break;
		case D3D11_SIT_UAV_RWTYPED:
			pPipeline->BindUnorderedAccessParameter( GetType(), ResourceBindings[i].pParamRef, slot, pParamManager );
			break;
		case D3D11_SIT_STRUCTURED:
			pPipeline->BindShaderResourceParameter( GetType(), ResourceBindings[i].pParamRef, slot, pParamManager );
			break;
		case D3D11_SIT_UAV_RWSTRUCTURED:
			pPipeline->BindUnorderedAccessParameter( GetType(), ResourceBindings[i].pParamRef, slot, pParamManager );
			break;
		case D3D11_SIT_BYTEADDRESS:
			pPipeline->BindShaderResourceParameter( GetType(), ResourceBindings[i].pParamRef, slot, pParamManager );
			break;
		case D3D11_SIT_UAV_RWBYTEADDRESS:
			pPipeline->BindUnorderedAccessParameter( GetType(), ResourceBindings[i].pParamRef, slot, pParamManager );
			break;
		case D3D11_SIT_UAV_APPEND_STRUCTURED:
			pPipeline->BindUnorderedAccessParameter( GetType(), ResourceBindings[i].pParamRef, slot, pParamManager );
			break;
		case D3D11_SIT_UAV_CONSUME_STRUCTURED:
			pPipeline->BindUnorderedAccessParameter( GetType(), ResourceBindings[i].pParamRef, slot, pParamManager );
			break;
		case D3D11_SIT_UAV_RWSTRUCTURED_WITH_COUNTER:
			pPipeline->BindUnorderedAccessParameter( GetType(), ResourceBindings[i].pParamRef, slot, pParamManager );
			break;
		}
	}
}
//--------------------------------------------------------------------------------
void ShaderDX11::PrintShaderDetails()
{
	std::wstringstream s;

	s << L"----------------------------------------------------------------------" << std::endl;
	s << L"Shader details printout for: " << FileName << std::endl;
	s << L"----------------------------------------------------------------------" << std::endl;

	s << L"Shader Description: " << std::endl;
	s << RendererDX11::Print_D3D11_SHADER_DESC( this->ShaderDescription );
	
	s << std::endl;

	s << L"Number of Input Signature Elements: " << InputSignatureParameters.count() << std::endl;
	for ( int i = 0; i < InputSignatureParameters.count(); i++ )
	{
		s << L"  Semantic Name: " << InputSignatureParameters[i].SemanticName;
		s << L", Semantic Index: " << InputSignatureParameters[i].SemanticIndex;
		s << L", Register: " << InputSignatureParameters[i].Register;
		s << L", System Value Type: " << RendererDX11::TranslateSystemVariableName( InputSignatureParameters[i].SystemValueType ); 
		s << L", Component Type: " << RendererDX11::TranslateRegisterComponentType( InputSignatureParameters[i].ComponentType );
		s << L", Mask: " << (unsigned int)InputSignatureParameters[i].Mask;
		s << L", Read/Write Mask: " << (unsigned int)InputSignatureParameters[i].ReadWriteMask;
		s << std::endl;
	}
	s << std::endl;

	s << L"Number of Output Signature Elements: " << OutputSignatureParameters.count() << std::endl;
	for ( int i = 0; i < OutputSignatureParameters.count(); i++ )
	{
		s << L"  Semantic Name: " << OutputSignatureParameters[i].SemanticName;
		s << L", Semantic Index: " << OutputSignatureParameters[i].SemanticIndex;
		s << L", Register: " << OutputSignatureParameters[i].Register;
		s << L", System Value Type: " << RendererDX11::TranslateSystemVariableName( OutputSignatureParameters[i].SystemValueType );  
		s << L", Component Type: " << RendererDX11::TranslateRegisterComponentType( OutputSignatureParameters[i].ComponentType );
		s << L", Mask: " << (unsigned int)OutputSignatureParameters[i].Mask;
		s << L", Read/Write Mask: " << (unsigned int)OutputSignatureParameters[i].ReadWriteMask;
		s << std::endl;
	}
	s << std::endl;

	s << L"Number of Constant Buffer Descriptions: " << ConstantBuffers.count() << std::endl;
	for ( int i = 0; i < ConstantBuffers.count(); i++ )
	{
		s << L"  Buffer Name: " << ConstantBuffers[i].Description.Name;
		s << L", Buffer Type: " << RendererDX11::TranslateCBufferType( ConstantBuffers[i].Description.Type );
		s << L", Variables: " << ConstantBuffers[i].Description.Variables;
		s << L", Size: " << ConstantBuffers[i].Description.Size;
		s << L", Flags: " << ConstantBuffers[i].Description.uFlags;
		
		for ( int j = 0; j < ConstantBuffers[i].Variables.count(); j++ )
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


		for ( int j = 0; j < ConstantBuffers[i].Types.count(); j++ )
		{
			s << std::endl << L"    ";
			s << L"Variable Type Name: " << ConstantBuffers[i].Types[j].Name;
			s << L", Class: " << RendererDX11::TranslateShaderVariableClass( ConstantBuffers[i].Types[j].Class );
			s << L", Type: " << RendererDX11::TranslateShaderVariableType( ConstantBuffers[i].Types[j].Type );			
			s << L", Rows: " << ConstantBuffers[i].Types[j].Rows;
			s << L", Columns: " << ConstantBuffers[i].Types[j].Columns;
			s << L", Elements: " << ConstantBuffers[i].Types[j].Elements;
			s << L", Members: " << ConstantBuffers[i].Types[j].Members;
			s << L", Offset: " << ConstantBuffers[i].Types[j].Offset;
		}
		
		s << std::endl;
	}
	s << std::endl;

	s << L"Number of Resource Binding Descriptions: " << ResourceBindings.count() << std::endl;
	for ( int i = 0; i < ResourceBindings.count(); i++ )
	{
		s << L"  Name: " << ResourceBindings[i].Name;
		s << L", Type: " << RendererDX11::TranslateShaderInputType( ResourceBindings[i].Type );
		s << L", Bind Point: " << ResourceBindings[i].BindPoint;
		s << L", Bind Count: " << ResourceBindings[i].BindCount;
		s << L", Flags: " << ResourceBindings[i].uFlags;
		s << L", Resource Return Type: " << RendererDX11::TranslateResourceReturnType( ResourceBindings[i].ReturnType );
		s << L", Dimension: " << ResourceBindings[i].Dimension;
		s << L", Number of Samples: " << ResourceBindings[i].NumSamples;
		s << std::endl;
	}

	s << std::endl;

	LPVOID pBlobBuffer = pCompiledShader->GetBufferPointer();
	const char* pMessage = (const char*)pBlobBuffer;
	
	s << pMessage << std::endl;

	s << L"----------------------------------------------------------------------" << std::endl;

	Log::Get().Write( s.str() );

}
//--------------------------------------------------------------------------------
