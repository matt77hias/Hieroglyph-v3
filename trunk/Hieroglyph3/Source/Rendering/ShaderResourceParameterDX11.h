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
// ShaderResourceParameterDX11
//
//--------------------------------------------------------------------------------
#include "RenderParameterDX11.h"
#include "Vector4f.h"
//--------------------------------------------------------------------------------
#ifndef ShaderResourceParameterDX11_h
#define ShaderResourceParameterDX11_h
//--------------------------------------------------------------------------------
namespace Glyph3
{
	class ShaderResourceParameterDX11 : public RenderParameterDX11
	{
	public:
		ShaderResourceParameterDX11();
		virtual ~ShaderResourceParameterDX11();

		virtual void SetParameterData( void* pData );
		virtual ParameterType GetParameterType();
		int GetIndex();
	protected:
		int		m_iShaderResourceView;
	};
};
//--------------------------------------------------------------------------------
#endif // ShaderResourceParameterDX11_h
//--------------------------------------------------------------------------------

