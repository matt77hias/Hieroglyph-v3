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
// IParameterManager
// 
//--------------------------------------------------------------------------------
#ifndef IParameterManager_h
#define IParameterManager_h
//--------------------------------------------------------------------------------
#include "RendererDX11.h"
#include "ConstantBufferParameterDX11.h"
#include "MatrixParameterDX11.h"
#include "MatrixArrayParameterDX11.h"
#include "SamplerParameterDX11.h"
#include "ShaderResourceParameterDX11.h"
#include "UnorderedAccessParameterDX11.h"
#include "VectorParameterDX11.h"
//--------------------------------------------------------------------------------
namespace Glyph3
{

	class IParameterManager
	{
	public:
		IParameterManager() {};
		virtual ~IParameterManager() {};

		virtual unsigned int GetID( ) = 0;

		// Here we allow rendering parameters to be registered and set by name.  These 
		// parameters are then available for setting their value and subsequent use 
		// during rendering.

		//virtual void SetParameter( RenderParameterDX11* pParameter ) = 0;
		virtual void SetVectorParameter( const std::wstring& name, Vector4f* pVector ) = 0;
		virtual void SetMatrixParameter( const std::wstring& name, Matrix4f* pMatrix ) = 0;
		virtual void SetSamplerParameter( const std::wstring& name, int* pID ) = 0;
		virtual void SetShaderResourceParameter( const std::wstring& name, ResourcePtr resource ) = 0;
		virtual void SetUnorderedAccessParameter( const std::wstring& name, ResourcePtr resource, unsigned int initial = -1 ) = 0;
		virtual void SetConstantBufferParameter( const std::wstring& name, ResourcePtr resource ) = 0;
		virtual void SetMatrixArrayParameter( const std::wstring& name, int count, Matrix4f* pMatrices ) = 0;
		
		virtual void SetVectorParameter( RenderParameterDX11* pParameter, Vector4f* pVector ) = 0;
		virtual void SetMatrixParameter( RenderParameterDX11* pParameter, Matrix4f* pMatrix ) = 0;
		virtual void SetSamplerParameter( RenderParameterDX11* pParameter, int* pID ) = 0;
		virtual void SetShaderResourceParameter( RenderParameterDX11* pParameter, ResourcePtr resource ) = 0;
		virtual void SetUnorderedAccessParameter( RenderParameterDX11* pParameter, ResourcePtr resource, unsigned int initial = -1 ) = 0;
		virtual void SetConstantBufferParameter( RenderParameterDX11* pParameter, ResourcePtr resource ) = 0;
		virtual void SetMatrixArrayParameter( RenderParameterDX11* pParameter, int count, Matrix4f* pMatrices ) = 0;

		virtual RenderParameterDX11* GetParameterRef( const std::wstring& name ) = 0;
		virtual VectorParameterDX11* GetVectorParameterRef( const std::wstring& name ) = 0;
		virtual MatrixParameterDX11* GetMatrixParameterRef( const std::wstring& name ) = 0;
		virtual ShaderResourceParameterDX11* GetShaderResourceParameterRef( const std::wstring& name ) = 0;
		virtual UnorderedAccessParameterDX11* GetUnorderedAccessParameterRef( const std::wstring& name ) = 0;
		virtual ConstantBufferParameterDX11* GetConstantBufferParameterRef( const std::wstring& name ) = 0;
		virtual SamplerParameterDX11* GetSamplerStateParameterRef( const std::wstring& name ) = 0;
		virtual MatrixArrayParameterDX11* GetMatrixArrayParameterRef( const std::wstring& name, int count ) = 0;

		// Each of the parameter types can also be accessed to inspect their current
		// value prior to setting them.

		virtual Vector4f GetVectorParameter( RenderParameterDX11* pParameter ) = 0;
		virtual Matrix4f GetMatrixParameter( RenderParameterDX11* pParameter ) = 0;
		virtual int GetShaderResourceParameter( RenderParameterDX11* pParameter ) = 0;
		virtual int GetUnorderedAccessParameter( RenderParameterDX11* pParameter ) = 0;
		virtual int GetConstantBufferParameter( RenderParameterDX11* pParameter ) = 0;
		virtual int GetSamplerStateParameter( RenderParameterDX11* pParameter ) = 0;
		virtual Matrix4f* GetMatrixArrayParameter( RenderParameterDX11* pParameter ) = 0;

		virtual Vector4f GetVectorParameter( const std::wstring& name ) = 0;
		virtual Matrix4f GetMatrixParameter( const std::wstring& name ) = 0;
		virtual int GetShaderResourceParameter( const std::wstring& name ) = 0;
		virtual int GetUnorderedAccessParameter( const std::wstring& name ) = 0;
		virtual int GetConstantBufferParameter( const std::wstring& name ) = 0;
		virtual int GetSamplerStateParameter( const std::wstring& name ) = 0;
		virtual Matrix4f* GetMatrixArrayParameter( const std::wstring& name, int count ) = 0;

		// The following matrix parameters are set with specialized functions to allow
		// the renderer to calculate the concatenated matrices.
		
		virtual void SetWorldMatrixParameter( Matrix4f* pMatrix ) = 0;
		virtual void SetViewMatrixParameter( Matrix4f* pMatrix ) = 0;
		virtual void SetProjMatrixParameter( Matrix4f* pMatrix ) = 0;

		// Allow the connection of multiple parameter managers for subsets of the 
		// complete parameter space to be isolated from the other regions if needed.
		// This can be helpful in distributed processing as seen in multithreaded
		// rendering.

		virtual void AttachParent( IParameterManager* pParent ) = 0;
		virtual void DetachParent( ) = 0;

	};
};

#endif // IParameterManager_h
