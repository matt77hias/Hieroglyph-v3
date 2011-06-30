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
// DepthStencilStateDX11
//
//--------------------------------------------------------------------------------
#ifndef DepthStencilStateDX11_h
#define DepthStencilStateDX11_h
//--------------------------------------------------------------------------------
#include "RendererDX11.h"
#include "PipelineManagerDX11.h"
//--------------------------------------------------------------------------------
namespace Glyph3
{
	class DepthStencilStateDX11
	{
	public:
		DepthStencilStateDX11( ID3D11DepthStencilState* pState );
		virtual ~DepthStencilStateDX11();

	protected:
		ID3D11DepthStencilState*			m_pState;

		friend PipelineManagerDX11;
	};
};
//--------------------------------------------------------------------------------
#endif // DepthStencilStateDX11_h
//--------------------------------------------------------------------------------

