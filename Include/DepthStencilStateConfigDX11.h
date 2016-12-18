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

	// Global comparison operators
	inline bool operator==( const D3D11_DEPTH_STENCILOP_DESC& lhs, const D3D11_DEPTH_STENCILOP_DESC& rhs )
	{
		return 
			lhs.StencilDepthFailOp == rhs.StencilDepthFailOp &&
			lhs.StencilFailOp == rhs.StencilFailOp &&
			lhs.StencilFunc == rhs.StencilFunc &&
			lhs.StencilPassOp == rhs.StencilPassOp;
	}

	inline bool operator==(const DepthStencilStateConfigDX11& lhs, const DepthStencilStateConfigDX11& rhs)
	{
		return
			lhs.BackFace == rhs.BackFace &&
			lhs.DepthEnable == rhs.DepthEnable &&
			lhs.DepthFunc == rhs.DepthFunc &&
			lhs.DepthWriteMask == rhs.DepthWriteMask &&
			lhs.FrontFace == rhs.FrontFace &&
			lhs.StencilEnable == rhs.StencilEnable &&
			lhs.StencilReadMask == rhs.StencilReadMask &&
			lhs.StencilWriteMask == rhs.StencilWriteMask;
	}

	inline bool operator<( const D3D11_DEPTH_STENCILOP_DESC& lhs, const D3D11_DEPTH_STENCILOP_DESC& rhs )
	{
		return memcmp(&lhs, &rhs, sizeof(lhs)) < 0;
		//return 
		//	lhs.StencilDepthFailOp < rhs.StencilDepthFailOp ||
		//	lhs.StencilFailOp < rhs.StencilFailOp ||
		//	lhs.StencilFunc < rhs.StencilFunc ||
		//	lhs.StencilPassOp < rhs.StencilPassOp;
	}

	inline bool operator<(const DepthStencilStateConfigDX11& lhs, const DepthStencilStateConfigDX11& rhs)
	{
		return memcmp(&lhs, &rhs, sizeof(lhs)) < 0;
		//return
		//	lhs.BackFace < rhs.BackFace ||
		//	lhs.DepthEnable < rhs.DepthEnable ||
		//	lhs.DepthFunc < rhs.DepthFunc ||
		//	lhs.DepthWriteMask < rhs.DepthWriteMask ||
		//	lhs.FrontFace < rhs.FrontFace ||
		//	lhs.StencilEnable < rhs.StencilEnable ||
		//	lhs.StencilReadMask < rhs.StencilReadMask ||
		//	lhs.StencilWriteMask < rhs.StencilWriteMask;
	}

};
//--------------------------------------------------------------------------------
#endif // DepthStencilStateConfigDX11_h
//--------------------------------------------------------------------------------

