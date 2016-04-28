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
// SamplerStateConfigDX11
//
//--------------------------------------------------------------------------------
#ifndef SamplerStateConfigDX11_h
#define SamplerStateConfigDX11_h
//--------------------------------------------------------------------------------
#include "RendererDX11.h"
//--------------------------------------------------------------------------------
namespace Glyph3
{
	class SamplerStateConfigDX11 : public D3D11_SAMPLER_DESC
	{
	public:
		SamplerStateConfigDX11();
		~SamplerStateConfigDX11();

		void SetDefaults();

	protected:

		friend RendererDX11;
	};
};
//--------------------------------------------------------------------------------
#endif // SamplerStateConfigDX11_h
//--------------------------------------------------------------------------------

