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
// DepthStencilViewDX11
//
//--------------------------------------------------------------------------------
#ifndef DepthStencilViewDX11_h
#define DepthStencilViewDX11_h
//--------------------------------------------------------------------------------
#include "PipelineManagerDx11.h"
//--------------------------------------------------------------------------------
namespace Glyph3
{
	class DepthStencilViewDX11
	{
	public:
		DepthStencilViewDX11( DepthStencilViewComPtr pView );
		~DepthStencilViewDX11();

	protected:
		DepthStencilViewComPtr			m_pDepthStencilView;
		
		friend PipelineManagerDX11;
		friend OutputMergerStageDX11;
		friend RendererDX11;
	};
};
//--------------------------------------------------------------------------------
#endif // DepthStencilViewDX11_h
//--------------------------------------------------------------------------------

