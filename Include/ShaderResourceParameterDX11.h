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
// ShaderResourceParameterDX11
//
//--------------------------------------------------------------------------------
#ifndef ShaderResourceParameterDX11_h
#define ShaderResourceParameterDX11_h
//--------------------------------------------------------------------------------
#include "RenderParameterDX11.h"
//--------------------------------------------------------------------------------
namespace Glyph3
{
	class ShaderResourceParameterDX11 : public RenderParameterDX11
	{
	public:
		ShaderResourceParameterDX11();
		ShaderResourceParameterDX11( ShaderResourceParameterDX11& copy );
		virtual ~ShaderResourceParameterDX11();

		virtual void SetParameterData( void* pData, unsigned int threadID = 0 );
		//virtual void ResetParameterData( void* pData, unsigned int threadID = 0 );

		virtual const ParameterType GetParameterType();
		int GetIndex( unsigned int threadID );

		//void UpdateValue( RenderParameterDX11* pParameter, unsigned int threadID = 0 );

	protected:
		int		m_iShaderResourceView[NUM_THREADS+1];
	};
};
//--------------------------------------------------------------------------------
#endif // ShaderResourceParameterDX11_h
//--------------------------------------------------------------------------------

