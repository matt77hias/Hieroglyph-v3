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
// ParameterContainer
//
//--------------------------------------------------------------------------------
#ifndef ParameterContainer_h
#define ParameterContainer_h
//--------------------------------------------------------------------------------
#include "PCH.h"
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
		// TODO: These methods will return nullptr when the writer hasn't already been
		//       set at least once before.  This should probably change to creating a 
		//       new writer instead of returning nullptr, as there is no alternative - 
		//       you have no choice but to create a new writer.
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
		ConstantBufferParameterWriterDX11* SetConstantBufferParameter( const std::wstring& name, const ResourcePtr& value );
		MatrixArrayParameterWriterDX11* SetMatrixArrayParameter( const std::wstring& name, Matrix4f* values, int count );
		MatrixParameterWriterDX11* SetMatrixParameter( const std::wstring& name, const Matrix4f& value );
		SamplerParameterWriterDX11* SetSamplerParameter( const std::wstring& name, int value );
		ShaderResourceParameterWriterDX11* SetShaderResourceParameter( const std::wstring& name, const ResourcePtr& value );
		UnorderedAccessParameterWriterDX11* SetUnorderedAccessParameter( const std::wstring& name, const ResourcePtr& value );
		VectorParameterWriterDX11* SetVectorParameter( const std::wstring& name, const Vector4f& value );

		// Apply the parameters in this container to a parameter manager.
		void SetRenderParams( IParameterManager* pParamManager );
		void InitRenderParams( );

	protected:

		std::vector< ParameterWriter* > m_RenderParameters;
	};
};
//--------------------------------------------------------------------------------
#endif // ParameterContainer_h
