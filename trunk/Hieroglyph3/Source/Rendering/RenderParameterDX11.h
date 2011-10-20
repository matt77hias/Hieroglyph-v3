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
#ifndef RenderParameterDX11_h
#define RenderParameterDX11_h
//--------------------------------------------------------------------------------
#include "PCH.h"
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

		void InitializeParameterData( void* pData );

		virtual void SetParameterData( void* pData, unsigned int threadID = 0 ) = 0;
		virtual const ParameterType GetParameterType() = 0;

		void SetName( const std::wstring& name );
		std::wstring& GetName();

		virtual void UpdateValue( RenderParameterDX11* pParameter, unsigned int threadID = 0 ) = 0;

	protected:
		std::wstring	m_sParameterName;
		ParameterType	m_iParameterType;
	};
};
//--------------------------------------------------------------------------------
#endif // RenderParameterDX11_h
//--------------------------------------------------------------------------------

