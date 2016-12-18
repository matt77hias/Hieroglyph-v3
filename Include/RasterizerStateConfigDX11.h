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

	inline bool operator==(const RasterizerStateConfigDX11& lhs, const RasterizerStateConfigDX11& rhs)
	{
		return
			lhs.AntialiasedLineEnable == rhs.AntialiasedLineEnable &&
			lhs.CullMode == rhs.CullMode &&
			lhs.DepthBias == rhs.DepthBias &&
			lhs.DepthBiasClamp == rhs.DepthBiasClamp &&
			lhs.DepthClipEnable == rhs.DepthClipEnable &&
			lhs.FillMode == rhs.FillMode &&
			lhs.FrontCounterClockwise == rhs.FrontCounterClockwise &&
			lhs.MultisampleEnable == rhs.MultisampleEnable &&
			lhs.ScissorEnable == rhs.ScissorEnable &&
			lhs.SlopeScaledDepthBias == rhs.SlopeScaledDepthBias;
	}

	inline bool operator<(const RasterizerStateConfigDX11& lhs, const RasterizerStateConfigDX11& rhs)
	{
		return memcmp(&lhs, &rhs, sizeof(lhs) ) < 0;
		//return
		//	lhs.AntialiasedLineEnable < rhs.AntialiasedLineEnable ||
		//	lhs.CullMode < rhs.CullMode ||
		//	lhs.DepthBias < rhs.DepthBias ||
		//	lhs.DepthBiasClamp < rhs.DepthBiasClamp ||
		//	lhs.DepthClipEnable < rhs.DepthClipEnable ||
		//	lhs.FillMode < rhs.FillMode ||
		//	lhs.FrontCounterClockwise < rhs.FrontCounterClockwise ||
		//	lhs.MultisampleEnable < rhs.MultisampleEnable ||
		//	lhs.ScissorEnable < rhs.ScissorEnable ||
		//	lhs.SlopeScaledDepthBias < rhs.SlopeScaledDepthBias;
	}
};
//--------------------------------------------------------------------------------
#endif // RasterizerStateConfigDX11_h
//--------------------------------------------------------------------------------

