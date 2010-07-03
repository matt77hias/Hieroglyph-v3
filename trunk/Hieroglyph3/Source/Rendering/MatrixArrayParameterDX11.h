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
// MatrixArrayParameterDX11
//
//--------------------------------------------------------------------------------
#include "RenderParameterDX11.h"
#include "Matrix4f.h"
//--------------------------------------------------------------------------------
#ifndef MatrixArrayParameterDX11_h
#define MatrixArrayParameterDX11_h
//--------------------------------------------------------------------------------
namespace Glyph3
{
	class MatrixArrayParameterDX11 : public RenderParameterDX11
	{
	public:
		MatrixArrayParameterDX11( int count );
		MatrixArrayParameterDX11( MatrixArrayParameterDX11& copy );
		MatrixArrayParameterDX11& operator=( MatrixArrayParameterDX11& parameter );
		virtual ~MatrixArrayParameterDX11();

		virtual void SetParameterData( void* pData );
		virtual ParameterType GetParameterType();
		int GetMatrixCount();
		Matrix4f* GetMatrices();

		void UpdateValue( RenderParameterDX11* pParameter );

	protected:
		int				m_iMatrixCount;
		Matrix4f*		m_pMatrices;
	};
};
//--------------------------------------------------------------------------------
#endif // MatrixArrayParameterDX11_h
//--------------------------------------------------------------------------------

