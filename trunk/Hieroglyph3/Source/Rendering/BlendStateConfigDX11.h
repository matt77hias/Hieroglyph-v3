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
// BlendStateConfigDX11
//
//--------------------------------------------------------------------------------
#include "RendererDX11.h"
//--------------------------------------------------------------------------------
#ifndef BlendStateConfigDX11_h
#define BlendStateConfigDX11_h
//--------------------------------------------------------------------------------
namespace Glyph3
{
	class BlendStateConfigDX11 : public D3D11_BLEND_DESC
	{
	public:
		BlendStateConfigDX11();
		~BlendStateConfigDX11();

		void SetDefaults();

	protected:

		friend RendererDX11;
	};
};
//--------------------------------------------------------------------------------
#endif // BlendStateConfigDX11_h
//--------------------------------------------------------------------------------

