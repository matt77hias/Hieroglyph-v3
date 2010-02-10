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
		SHADER_RESOURCE,
		UNORDERED_ACCESS,
		CBUFFER,
		SAMPLER
	};

	class RenderParameterDX11
	{
	public:
		RenderParameterDX11();
		virtual ~RenderParameterDX11();

		virtual void SetParameterData( void* pData ) = 0;
		virtual ParameterType GetParameterType() = 0;

	protected:
		std::wstring	m_sParameterName;
		ParameterType	m_iParameterType;
	};
};
//--------------------------------------------------------------------------------
#endif // RenderParameterDX11_h
//--------------------------------------------------------------------------------

