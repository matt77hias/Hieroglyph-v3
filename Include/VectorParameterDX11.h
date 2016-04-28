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
// VectorParameterDX11
//
//--------------------------------------------------------------------------------
#ifndef VectorParameterDX11_h
#define VectorParameterDX11_h
//--------------------------------------------------------------------------------
#include "RenderParameterDX11.h"
#include "Vector4f.h"
//--------------------------------------------------------------------------------
namespace Glyph3
{
	class VectorParameterDX11 : public RenderParameterDX11
	{
	public:
		VectorParameterDX11();
		VectorParameterDX11( VectorParameterDX11& copy );
		virtual ~VectorParameterDX11();

		virtual void SetParameterData( void* pData, unsigned int threadID = 0 );
		//virtual void ResetParameterData( void* pData, unsigned int threadID = 0 );
		virtual const ParameterType GetParameterType();
		
		Vector4f GetVector( unsigned int threadID = 0 );
		void SetVector( Vector4f v, unsigned int threadID = 0 );

		//void UpdateValue( RenderParameterDX11* pParameter, unsigned int threadID = 0 );

	protected:
		Vector4f	m_Vector[NUM_THREADS+1];
	};
};
//--------------------------------------------------------------------------------
#endif // VectorParameterDX11_h
//--------------------------------------------------------------------------------

