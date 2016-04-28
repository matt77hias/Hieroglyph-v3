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
// ShaderReflectionDX11
//
//--------------------------------------------------------------------------------
#ifndef ShaderReflectionDX11_h
#define ShaderReflectionDX11_h
//--------------------------------------------------------------------------------
#include "PCH.h"
#include "GlyphString.h"
#include "RenderParameterDX11.h"
//--------------------------------------------------------------------------------
namespace Glyph3
{
	class PipelineManagerDX11;
	class IParameterManager;

	// This enumeration is used to access an array shader stages/programs.  Do not
	// modify the value of each of the enumerations!

	enum ShaderType
	{
		VERTEX_SHADER = 0,
		HULL_SHADER = 1,
		DOMAIN_SHADER = 2,
		GEOMETRY_SHADER = 3,
		PIXEL_SHADER = 4,
		COMPUTE_SHADER = 5
	};


	enum ShaderMask
	{
		VERTEX_SHADER_MSK = 0x0001,
		HULL_SHADER_MSK = 0x0002,
		DOMAIN_SHADER_MSK = 0x0004,
		GEOMETRY_SHADER_MSK = 0x0008,
		PIXEL_SHADER_MSK = 0x0010,
		COMPUTE_SHADER_MSK = 0x0020
	};


	struct ShaderInputBindDesc
	{
		ShaderInputBindDesc( D3D11_SHADER_INPUT_BIND_DESC desc )
		{
			Name = std::wstring( GlyphString::ToUnicode( std::string( desc.Name ) ) );
			Type = desc.Type;
			BindPoint = desc.BindPoint;
			BindCount = desc.BindCount;
			uFlags = desc.uFlags;
			ReturnType = desc.ReturnType;
			Dimension = desc.Dimension;
			NumSamples = desc.NumSamples;
			pParamRef = 0;
		};
		
		ShaderInputBindDesc()
		{
			Name = std::wstring( L"" );
			Type = D3D_SIT_CBUFFER;
			BindPoint = 0;
			BindCount = 0;
			uFlags = 0;
			ReturnType = D3D11_RETURN_TYPE_UNORM;
			Dimension = D3D_SRV_DIMENSION_UNKNOWN;
			NumSamples = 0;
			pParamRef = 0;
		};

		std::wstring Name;
		D3D_SHADER_INPUT_TYPE Type;
		UINT BindPoint;
		UINT BindCount;
		UINT uFlags;
		D3D11_RESOURCE_RETURN_TYPE ReturnType;
		D3D_SRV_DIMENSION Dimension;
		UINT NumSamples;
		RenderParameterDX11* pParamRef;
	};


	struct ConstantBufferLayout
	{
		D3D11_SHADER_BUFFER_DESC				Description;
		std::vector<D3D11_SHADER_VARIABLE_DESC>	Variables;
		std::vector<D3D11_SHADER_TYPE_DESC>		Types;
		std::vector<RenderParameterDX11*>		Parameters;
		RenderParameterDX11*					pParamRef;
	};

	class ShaderReflectionDX11
	{
	public:
		ShaderReflectionDX11();
		~ShaderReflectionDX11();

		void InitializeConstantBuffers( IParameterManager* pParamManager );
		void UpdateParameters( PipelineManagerDX11* pPipeline, IParameterManager* pParamManager );
		void BindParameters( ShaderType type, PipelineManagerDX11* pPipeline, IParameterManager* pParamManager );

		void PrintShaderDetails();

		void SetName( const std::wstring& name );
		std::wstring GetName( );

	public:
		std::wstring									Name;
		D3D11_SHADER_DESC								ShaderDescription;
		std::vector<D3D11_SIGNATURE_PARAMETER_DESC>		InputSignatureParameters;
		std::vector<D3D11_SIGNATURE_PARAMETER_DESC>		OutputSignatureParameters;
		std::vector<ConstantBufferLayout>				ConstantBuffers;
		std::vector<ShaderInputBindDesc>				ResourceBindings;
	};

};
#endif // ShaderReflectionDX11_h


