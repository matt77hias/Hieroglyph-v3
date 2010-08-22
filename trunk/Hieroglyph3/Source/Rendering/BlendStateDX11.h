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
// BlendStateDX11
//
//--------------------------------------------------------------------------------
#include "RendererDX11.h"
#include "PipelineManagerDX11.h"
//--------------------------------------------------------------------------------
#ifndef BlendStateDX11_h
#define BlendStateDX11_h
//--------------------------------------------------------------------------------
namespace Glyph3
{
	class BlendStateDX11
	{
	public:
		BlendStateDX11( ID3D11BlendState* pState );
		virtual ~BlendStateDX11();

	protected:
		ID3D11BlendState*			m_pState;

		friend PipelineManagerDX11;
	};
};
//--------------------------------------------------------------------------------
#endif // BlendStateDX11_h
//--------------------------------------------------------------------------------

