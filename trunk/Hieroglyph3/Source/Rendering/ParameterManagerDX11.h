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
// ParameterManagerDX11
// 
//--------------------------------------------------------------------------------
#ifndef ParameterManagerDX11_h
#define ParameterManagerDX11_h
//--------------------------------------------------------------------------------
#include "RendererDX11.h"
//--------------------------------------------------------------------------------
namespace Glyph3
{

	class ParameterManagerDX11
	{
	public:
		ParameterManagerDX11();
		~ParameterManagerDX11();

		// Here we allow rendering parameters to be registered and set by name.  These 
		// parameters are then available for setting their value and subsequent use 
		// during rendering.

		void SetParameter( RenderParameterDX11* pParameter );
		void SetVectorParameter( const std::wstring& name, Vector4f* pVector );
		void SetMatrixParameter( const std::wstring& name, Matrix4f* pMatrix );
		void SetSamplerParameter( const std::wstring& name, int* pID );
		void SetShaderResourceParameter( const std::wstring& name, ResourcePtr resource );
		void SetUnorderedAccessParameter( const std::wstring& name, ResourcePtr resource );
		void SetConstantBufferParameter( const std::wstring& name, ResourcePtr resource );
		void SetMatrixArrayParameter( const std::wstring& name, int count, Matrix4f* pMatrices );
		
		// Each of the parameter types can also be accessed to inspect their current
		// value prior to setting them.

		RenderParameterDX11* GetParameter( const std::wstring& name );
		Vector4f GetVectorParameter( const std::wstring& name );
		Matrix4f GetMatrixParameter( const std::wstring& name );
		int GetShaderResourceParameter( const std::wstring& name );
		int GetUnorderedAccessParameter( const std::wstring& name );
		int GetConstantBufferParameter( const std::wstring& name );
		int GetSamplerStateParameter( const std::wstring& name );
		Matrix4f* GetMatrixArrayParameter( const std::wstring& name, int count );

		// The following matrix parameters are set with specialized functions to allow
		// the renderer to calculate the concatenated matrices.
		
		void SetWorldMatrixParameter( Matrix4f* pMatrix );
		void SetViewMatrixParameter( Matrix4f* pMatrix );
		void SetProjMatrixParameter( Matrix4f* pMatrix );

			// All rendering parameters are stored in a map and are accessed by name.  This may
			// be modified to allow faster random access in the future, possibly using a hash 
			// table to reference an array.

		std::map< std::wstring, RenderParameterDX11* >	m_Parameters;

		void AttachParent( ParameterManagerDX11* pParent );
		void DetachParent( );

	protected:

		ParameterManagerDX11*	m_pParent;
	};
};

#endif // ParameterManagerDX11_h
