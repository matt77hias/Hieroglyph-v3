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
// DepthStencilViewDX11
//
//--------------------------------------------------------------------------------
#include "ResourceViewDX11.h"
#include "PipelineManagerDx11.h"
//--------------------------------------------------------------------------------
#ifndef DepthStencilViewDX11_h
#define DepthStencilViewDX11_h
//--------------------------------------------------------------------------------
namespace Glyph3
{
	class DepthStencilViewDX11 : public ResourceViewDX11
	{
	public:
		DepthStencilViewDX11( ID3D11DepthStencilView* pView );
		virtual ~DepthStencilViewDX11();

	protected:
		ID3D11DepthStencilView*			m_pDepthStencilView;
		
		friend PipelineManagerDX11;
	};
};
//--------------------------------------------------------------------------------
#endif // DepthStencilViewDX11_h
//--------------------------------------------------------------------------------

