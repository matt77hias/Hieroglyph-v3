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
// InputLayoutDX11
//
//--------------------------------------------------------------------------------
#ifndef InputLayoutDX11_h
#define InputLayoutDX11_h
//--------------------------------------------------------------------------------
#include "PCH.h"
//--------------------------------------------------------------------------------
namespace Glyph3
{
	class PipelineManagerDX11;
	class InputAssemblerStageDX11;

	class InputLayoutDX11
	{
	public:
		InputLayoutDX11( ID3D11InputLayout* pLayout );
		virtual ~InputLayoutDX11();

	protected:
		ID3D11InputLayout*			m_pInputLayout;

		friend PipelineManagerDX11;
		friend InputAssemblerStageDX11;
	};
};
//--------------------------------------------------------------------------------
#endif // InputLayoutDX11_h
//--------------------------------------------------------------------------------

