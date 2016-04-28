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
// SamplerParameterDX11
//
//--------------------------------------------------------------------------------
#ifndef SamplerParameterDX11_h
#define SamplerParameterDX11_h
//--------------------------------------------------------------------------------
#include "RenderParameterDX11.h"
//--------------------------------------------------------------------------------
namespace Glyph3
{
	class SamplerParameterDX11 : public RenderParameterDX11
	{
	public:
		SamplerParameterDX11();
		SamplerParameterDX11( SamplerParameterDX11& copy );
		virtual ~SamplerParameterDX11();

		virtual void SetParameterData( void* pData, unsigned int threadID = 0 );
		//virtual void ResetParameterData( void* pData, unsigned int threadID = 0 );

		virtual const ParameterType GetParameterType();
		int GetIndex( unsigned int threadID );

		//void UpdateValue( RenderParameterDX11* pParameter, unsigned int threadID = 0 );

	protected:
		int		m_iSampler[NUM_THREADS+1];
	};
};
//--------------------------------------------------------------------------------
#endif // SamplerParameterDX11_h
//--------------------------------------------------------------------------------

