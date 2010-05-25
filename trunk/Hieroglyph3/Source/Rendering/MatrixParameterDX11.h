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
// MatrixParameterDX11
//
//--------------------------------------------------------------------------------
#include "RenderParameterDX11.h"
#include "Matrix4f.h"
//--------------------------------------------------------------------------------
#ifndef MatrixParameterDX11_h
#define MatrixParameterDX11_h
//--------------------------------------------------------------------------------
namespace Glyph3
{
	class MatrixParameterDX11 : public RenderParameterDX11
	{
	public:
		MatrixParameterDX11();
		MatrixParameterDX11( MatrixParameterDX11& copy );
		virtual ~MatrixParameterDX11();

		virtual void SetParameterData( void* pData );
		virtual ParameterType GetParameterType();
		Matrix4f GetMatrix();

	protected:
		Matrix4f	m_Matrix;
	};
};
//--------------------------------------------------------------------------------
#endif // MatrixParameterDX11_h
//--------------------------------------------------------------------------------

