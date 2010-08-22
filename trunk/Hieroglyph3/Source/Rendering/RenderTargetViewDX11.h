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
// RenderTargetViewDX11
//
//--------------------------------------------------------------------------------
#include "ResourceViewDX11.h"
#include "PipelineManagerDx11.h"
//--------------------------------------------------------------------------------
#ifndef RenderTargetViewDX11_h
#define RenderTargetViewDX11_h
//--------------------------------------------------------------------------------
namespace Glyph3
{
	class RenderTargetViewDX11 : public ResourceViewDX11
	{
	public:
		RenderTargetViewDX11( ID3D11RenderTargetView* pView );
		virtual ~RenderTargetViewDX11();

	protected:
		ID3D11RenderTargetView*			m_pRenderTargetView;
		
		friend PipelineManagerDX11;
	};
};
//--------------------------------------------------------------------------------
#endif // RenderTargetViewDX11_h
//--------------------------------------------------------------------------------

