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
// ParameterContainer
//
//--------------------------------------------------------------------------------
#ifndef ParameterContainer_h
#define ParameterContainer_h
//--------------------------------------------------------------------------------
#include "TArray.h"
#include "ConstantBufferParameterWriterDX11.h"
#include "MatrixArrayParameterWriterDX11.h"
#include "MatrixParameterWriterDX11.h"
#include "SamplerParameterWriterDX11.h"
#include "ShaderResourceParameterWriterDX11.h"
#include "UnorderedAccessParameterWriterDX11.h"
#include "VectorParameterWriterDX11.h"
//--------------------------------------------------------------------------------
namespace Glyph3
{

	class ParameterContainer
	{
	public:
		ParameterContainer( );
		virtual ~ParameterContainer( );

		// Adding a generic parameter writer to this container.
		void AddRenderParameter( ParameterWriter* pWriter );

		// Retrieve a parameter writer that was added to this container earlier.
		ParameterWriter* GetRenderParameter( const std::wstring& name );
		ConstantBufferParameterWriterDX11* GetConstantBufferParameterWriter( const std::wstring& name );
		MatrixArrayParameterWriterDX11* GetMatrixArrayParameterWriter( const std::wstring& name );
		MatrixParameterWriterDX11* GetMatrixParameterWriter( const std::wstring& name );
		SamplerParameterWriterDX11* GetSamplerParameterWriter( const std::wstring& name );
		ShaderResourceParameterWriterDX11* GetShaderResourceParameterWriter( const std::wstring& name );
		UnorderedAccessParameterWriterDX11* GetUnorderedAccessParameterWriter( const std::wstring& name );
		VectorParameterWriterDX11* GetVectorParameterWriter( const std::wstring& name );

		// Setting values will add a parameter writer of the appropriate type to 
		// this container.  This allows for easy setting of values without having to
		// manually add, get, and set each value.
		void SetConstantBufferParameter( const std::wstring& name, ResourcePtr value );
		void SetMatrixArrayParameter( const std::wstring& name, Matrix4f* values, int count );
		void SetMatrixParameter( const std::wstring& name, Matrix4f& value );
		void SetSamplerParameter( const std::wstring& name, int value );
		void SetShaderResourceParameter( const std::wstring& name, ResourcePtr value );
		void SetUnorderedAccessParameter( const std::wstring& name, ResourcePtr value );
		void SetVectorParameter( const std::wstring& name, Vector4f& value );

		// Apply the parameters in this container to a parameter manager.
		void SetRenderParams( IParameterManager* pParamManager );

	protected:


		TArray< ParameterWriter* > m_RenderParameters;
	};
};
//--------------------------------------------------------------------------------
#endif // ParameterContainer_h
