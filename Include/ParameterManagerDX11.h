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
// ParameterManagerDX11
// 
//--------------------------------------------------------------------------------
#ifndef ParameterManagerDX11_h
#define ParameterManagerDX11_h
//--------------------------------------------------------------------------------
#include "IParameterManager.h"
//--------------------------------------------------------------------------------
namespace Glyph3
{

	class ParameterManagerDX11 : public IParameterManager
	{
	public:
		ParameterManagerDX11( unsigned int ID );
		virtual ~ParameterManagerDX11();

		// Here we allow rendering parameters to be registered and set by name.  These 
		// parameters are then available for setting their value and subsequent use 
		// during rendering.  Setting the value for a non-existing parameter will create
		// a new one and initialize all of the available slots to the provided data.

		//void SetParameter( RenderParameterDX11* pParameter );
		void SetVectorParameter( const std::wstring& name, Vector4f* pVector );
		void SetMatrixParameter( const std::wstring& name, Matrix4f* pMatrix );
		void SetSamplerParameter( const std::wstring& name, int* pID );
		void SetShaderResourceParameter( const std::wstring& name, ResourcePtr resource );
		void SetUnorderedAccessParameter( const std::wstring& name, ResourcePtr resource, unsigned int initial = -1 );
		void SetConstantBufferParameter( const std::wstring& name, ResourcePtr resource );
		void SetMatrixArrayParameter( const std::wstring& name, int count, Matrix4f* pMatrices );

		// Additionally, it is possible to set a value and provide the reference to the 
		// parameter itself.  This eliminates the need to do a lookup of the parameter
		// completely.

		virtual void SetVectorParameter( RenderParameterDX11* pParameter, Vector4f* pVector );
		virtual void SetMatrixParameter( RenderParameterDX11* pParameter, Matrix4f* pMatrix );
		virtual void SetSamplerParameter( RenderParameterDX11* pParameter, int* pID );
		virtual void SetShaderResourceParameter( RenderParameterDX11* pParameter, ResourcePtr resource );
		virtual void SetUnorderedAccessParameter( RenderParameterDX11* pParameter, ResourcePtr resource, unsigned int initial = -1 );
		virtual void SetConstantBufferParameter( RenderParameterDX11* pParameter, ResourcePtr resource );
		virtual void SetMatrixArrayParameter( RenderParameterDX11* pParameter, int count, Matrix4f* pMatrices );

		// References to parameters are acquired by simply querying for them by name.

		virtual RenderParameterDX11* GetParameterRef( const std::wstring& name );
		virtual VectorParameterDX11* GetVectorParameterRef( const std::wstring& name );
		virtual MatrixParameterDX11* GetMatrixParameterRef( const std::wstring& name );
		virtual ShaderResourceParameterDX11* GetShaderResourceParameterRef( const std::wstring& name );
		virtual UnorderedAccessParameterDX11* GetUnorderedAccessParameterRef( const std::wstring& name );
		virtual ConstantBufferParameterDX11* GetConstantBufferParameterRef( const std::wstring& name );
		virtual SamplerParameterDX11* GetSamplerStateParameterRef( const std::wstring& name );
		virtual MatrixArrayParameterDX11* GetMatrixArrayParameterRef( const std::wstring& name, int count );

		// Each of the parameter types can also be accessed to inspect their current
		// value prior to setting them.  Calling the get methods will create an empty 
		// parameter if it doesn't already exist.  This can also be performed with the
		// generic render parameter pointer as well (which is used internally to the 
		// engine).

		Vector4f GetVectorParameter( const std::wstring& name );
		Matrix4f GetMatrixParameter( const std::wstring& name );
		int GetShaderResourceParameter( const std::wstring& name );
		int GetUnorderedAccessParameter( const std::wstring& name );
		int GetConstantBufferParameter( const std::wstring& name );
		int GetSamplerStateParameter( const std::wstring& name );
		Matrix4f* GetMatrixArrayParameter( const std::wstring& name, int count );

		Vector4f GetVectorParameter( RenderParameterDX11* pParameter );
		Matrix4f GetMatrixParameter( RenderParameterDX11* pParameter );
		int GetShaderResourceParameter( RenderParameterDX11* pParameter );
		int GetUnorderedAccessParameter( RenderParameterDX11* pParameter );
		int GetConstantBufferParameter( RenderParameterDX11* pParameter );
		int GetSamplerStateParameter( RenderParameterDX11* pParameter );
		Matrix4f* GetMatrixArrayParameter( RenderParameterDX11* pParameter );


		// The following matrix parameters are set with specialized functions to allow
		// the renderer to calculate the concatenated matrices.
		
		void SetWorldMatrixParameter( Matrix4f* pMatrix );
		void SetViewMatrixParameter( Matrix4f* pMatrix );
		void SetProjMatrixParameter( Matrix4f* pMatrix );

		// All rendering parameters are stored in a map and are accessed by name.  This may
		// be modified to allow faster random access in the future, possibly using a hash 
		// table to reference an array.

		static std::map< std::wstring, RenderParameterDX11* >	m_Parameters;

		void AttachParent( IParameterManager* pParent );
		void DetachParent( );

		// Each parameter manager is provided with an ID to indicate which thread
		// it is working on.  After creation this ID is read only.

		unsigned int GetID();

	protected:

		IParameterManager*	m_pParent;
		unsigned int m_ID;

		MatrixParameterDX11* m_pWorldMatrix;
		MatrixParameterDX11* m_pViewMatrix;
		MatrixParameterDX11* m_pProjMatrix;
		MatrixParameterDX11* m_pWorldViewMatrix;
		MatrixParameterDX11* m_pWorldProjMatrix;
		MatrixParameterDX11* m_pViewProjMatrix;
		MatrixParameterDX11* m_pWorldViewProjMatrix;
	};
};

#endif // ParameterManagerDX11_h
