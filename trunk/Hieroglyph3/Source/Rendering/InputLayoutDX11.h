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
// InputLayoutDX11
//
//--------------------------------------------------------------------------------
#include "RendererDX11.h"
//--------------------------------------------------------------------------------
#ifndef InputLayoutDX11_h
#define InputLayoutDX11_h
//--------------------------------------------------------------------------------
namespace Glyph3
{
	class InputLayoutDX11
	{
	public:
		InputLayoutDX11( ID3D11InputLayout* pLayout );
		virtual ~InputLayoutDX11();

	protected:
		ID3D11InputLayout*			m_pInputLayout;

		friend RendererDX11;
	};
};
//--------------------------------------------------------------------------------
#endif // InputLayoutDX11_h
//--------------------------------------------------------------------------------

