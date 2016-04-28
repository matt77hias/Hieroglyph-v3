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
// MatrixArrayParameterDX11
//
//--------------------------------------------------------------------------------
#ifndef MatrixArrayParameterDX11_h
#define MatrixArrayParameterDX11_h
//--------------------------------------------------------------------------------
#include "RenderParameterDX11.h"
#include "Matrix4f.h"
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

		virtual void SetParameterData( void* pData, unsigned int threadID = 0 );
		//virtual void ResetParameterData( void* pData, unsigned int threadID = 0 );

		virtual const ParameterType GetParameterType();
		int GetMatrixCount();
		Matrix4f* GetMatrices( unsigned int threadID = 0 );

		//void UpdateValue( RenderParameterDX11* pParameter, unsigned int threadID = 0 );

	protected:
		int				m_iMatrixCount;
		Matrix4f*		m_pMatrices[NUM_THREADS+1];
	};
};
//--------------------------------------------------------------------------------
#endif // MatrixArrayParameterDX11_h
//--------------------------------------------------------------------------------

