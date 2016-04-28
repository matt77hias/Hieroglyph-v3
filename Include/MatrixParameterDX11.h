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
// MatrixParameterDX11
//
//--------------------------------------------------------------------------------
#ifndef MatrixParameterDX11_h
#define MatrixParameterDX11_h
//--------------------------------------------------------------------------------
#include "RenderParameterDX11.h"
#include "Matrix4f.h"
//--------------------------------------------------------------------------------
namespace Glyph3
{
	class MatrixParameterDX11 : public RenderParameterDX11
	{
	public:
		MatrixParameterDX11();
		MatrixParameterDX11( MatrixParameterDX11& copy );
		virtual ~MatrixParameterDX11();

		virtual void SetParameterData( void* pData, unsigned int threadID = 0 );
		//virtual void ResetParameterData( void* pData, unsigned int threadID = 0 );

		virtual const ParameterType GetParameterType();
		Matrix4f GetMatrix( unsigned int threadID = 0);

		//void UpdateValue( RenderParameterDX11* pParameter, unsigned int threadID = 0 );

	protected:
		Matrix4f	m_Matrix[NUM_THREADS+1];
	};
};
//--------------------------------------------------------------------------------
#endif // MatrixParameterDX11_h
//--------------------------------------------------------------------------------

