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
// ConstantBufferParameterDX11
//
//--------------------------------------------------------------------------------
#ifndef ConstantBufferParameterDX11_h
#define ConstantBufferParameterDX11_h
//--------------------------------------------------------------------------------
#include "RenderParameterDX11.h"
//--------------------------------------------------------------------------------
namespace Glyph3
{
	class ConstantBufferParameterDX11 : public RenderParameterDX11
	{
	public:
		ConstantBufferParameterDX11();
		ConstantBufferParameterDX11( ConstantBufferParameterDX11& copy );
		virtual ~ConstantBufferParameterDX11();

		virtual void SetParameterData( void* pData, unsigned int threadID = 0 );
		//virtual void ResetParameterData( void* pData, unsigned int threadID = 0 );

		virtual const ParameterType GetParameterType();

		//virtual void UpdateValue( RenderParameterDX11* pParameter, unsigned int threadID = 0 );

		int GetIndex( unsigned int threadID );

	protected:
		int		m_iCBuffer[NUM_THREADS+1];
	};
};
//--------------------------------------------------------------------------------
#endif // ConstantBufferParameterDX11_h
//--------------------------------------------------------------------------------

