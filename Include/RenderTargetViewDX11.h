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
// RenderTargetViewDX11
//
//--------------------------------------------------------------------------------
#ifndef RenderTargetViewDX11_h
#define RenderTargetViewDX11_h
//--------------------------------------------------------------------------------
#include "PipelineManagerDx11.h"
//--------------------------------------------------------------------------------
namespace Glyph3
{
	class RenderTargetViewDX11
	{
	public:
		RenderTargetViewDX11( RenderTargetViewComPtr pView );
		~RenderTargetViewDX11();

		ID3D11RenderTargetView* GetRTV();

	protected:
		RenderTargetViewComPtr			m_pRenderTargetView;
		
		friend PipelineManagerDX11;
		friend OutputMergerStageDX11;
		friend RendererDX11;
	};
};
//--------------------------------------------------------------------------------
#endif // RenderTargetViewDX11_h
//--------------------------------------------------------------------------------

