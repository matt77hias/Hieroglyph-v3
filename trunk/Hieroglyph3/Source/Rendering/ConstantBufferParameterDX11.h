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
// ConstantBufferParameterDX11
//
//--------------------------------------------------------------------------------
#include "RenderParameterDX11.h"
#include "Vector4f.h"
//--------------------------------------------------------------------------------
#ifndef ConstantBufferParameterDX11_h
#define ConstantBufferParameterDX11_h
//--------------------------------------------------------------------------------
namespace Glyph3
{
	class ConstantBufferParameterDX11 : public RenderParameterDX11
	{
	public:
		ConstantBufferParameterDX11();
		ConstantBufferParameterDX11( ConstantBufferParameterDX11& copy );
		virtual ~ConstantBufferParameterDX11();

		virtual void SetParameterData( void* pData );
		virtual ParameterType GetParameterType();

		void UpdateValue( RenderParameterDX11* pParameter );

		int GetIndex();

	protected:
		int		m_iCBuffer;
	};
};
//--------------------------------------------------------------------------------
#endif // ConstantBufferParameterDX11_h
//--------------------------------------------------------------------------------

