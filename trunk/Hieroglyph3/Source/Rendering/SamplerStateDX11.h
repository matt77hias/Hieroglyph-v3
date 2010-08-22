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
// SamplerStateDX11
//
//--------------------------------------------------------------------------------
#include "RendererDX11.h"
#include "PipelineManagerDX11.h"
//--------------------------------------------------------------------------------
#ifndef SamplerStateDX11_h
#define SamplerStateDX11_h
//--------------------------------------------------------------------------------
namespace Glyph3
{
	class SamplerStateDX11
	{
	public:
		SamplerStateDX11( ID3D11SamplerState* pState );
		virtual ~SamplerStateDX11();

	protected:
		ID3D11SamplerState*			m_pState;

		friend PipelineManagerDX11;
	};
};
//--------------------------------------------------------------------------------
#endif // SamplerStateDX11_h
//--------------------------------------------------------------------------------

