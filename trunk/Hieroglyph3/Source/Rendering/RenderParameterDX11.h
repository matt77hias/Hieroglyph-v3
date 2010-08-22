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
// RenderParameterDX11
//
//--------------------------------------------------------------------------------
#include "RendererDX11.h"
//--------------------------------------------------------------------------------
#ifndef RenderParameterDX11_h
#define RenderParameterDX11_h
//--------------------------------------------------------------------------------
namespace Glyph3
{
	enum ParameterType
	{
		VECTOR,
		MATRIX,
		MATRIX_ARRAY,
		SHADER_RESOURCE,
		UNORDERED_ACCESS,
		CBUFFER,
		SAMPLER,
		ENTITY
	};

	class RenderParameterDX11
	{
	public:
		RenderParameterDX11();
		RenderParameterDX11( RenderParameterDX11& copy );
		virtual ~RenderParameterDX11();

		RenderParameterDX11* CreateCopy();

		virtual void SetParameterData( void* pData ) = 0;
		virtual ParameterType GetParameterType() = 0;

		void SetName( std::wstring& name );
		std::wstring& GetName();

		virtual void UpdateValue( RenderParameterDX11* pParameter ) = 0;

	protected:
		std::wstring	m_sParameterName;
		ParameterType	m_iParameterType;
	};
};
//--------------------------------------------------------------------------------
#endif // RenderParameterDX11_h
//--------------------------------------------------------------------------------

