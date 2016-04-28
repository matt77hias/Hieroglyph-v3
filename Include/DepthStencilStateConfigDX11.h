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
// DepthStencilStateConfigDX11
//
//--------------------------------------------------------------------------------
#ifndef DepthStencilStateConfigDX11_h
#define DepthStencilStateConfigDX11_h
//--------------------------------------------------------------------------------
#include "RendererDX11.h"
//--------------------------------------------------------------------------------
namespace Glyph3
{
	class DepthStencilStateConfigDX11 : public D3D11_DEPTH_STENCIL_DESC
	{
	public:
		DepthStencilStateConfigDX11();
		~DepthStencilStateConfigDX11();

		void SetDefaults();

	protected:

		friend RendererDX11;
	};
};
//--------------------------------------------------------------------------------
#endif // DepthStencilStateConfigDX11_h
//--------------------------------------------------------------------------------

