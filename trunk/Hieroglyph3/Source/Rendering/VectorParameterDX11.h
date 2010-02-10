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
// VectorParameterDX11
//
//--------------------------------------------------------------------------------
#include "RenderParameterDX11.h"
#include "Vector4f.h"
//--------------------------------------------------------------------------------
#ifndef VectorParameterDX11_h
#define VectorParameterDX11_h
//--------------------------------------------------------------------------------
namespace Glyph3
{
	class VectorParameterDX11 : public RenderParameterDX11
	{
	public:
		VectorParameterDX11();
		virtual ~VectorParameterDX11();

		virtual void SetParameterData( void* pData );
		virtual ParameterType GetParameterType();
		Vector4f GetVector();

	protected:
		Vector4f	m_Vector;
	};
};
//--------------------------------------------------------------------------------
#endif // VectorParameterDX11_h
//--------------------------------------------------------------------------------

