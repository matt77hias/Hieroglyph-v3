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
// RasterizerStateConfigDX11
//
//--------------------------------------------------------------------------------
#ifndef RasterizerStateConfigDX11_h
#define RasterizerStateConfigDX11_h
//--------------------------------------------------------------------------------
#include "RendererDX11.h"
//--------------------------------------------------------------------------------
namespace Glyph3
{
	class RasterizerStateConfigDX11 : public D3D11_RASTERIZER_DESC
	{
	public:
		RasterizerStateConfigDX11();
		~RasterizerStateConfigDX11();

		void SetDefaults();

	protected:

		friend RendererDX11;
	};
};
//--------------------------------------------------------------------------------
#endif // RasterizerStateConfigDX11_h
//--------------------------------------------------------------------------------

