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
// RasterizerStateDX11
//
//--------------------------------------------------------------------------------
#ifndef RasterizerStateDX11_h
#define RasterizerStateDX11_h
//--------------------------------------------------------------------------------
#include "RasterizerStageDX11.h"
//--------------------------------------------------------------------------------
namespace Glyph3
{
	class RasterizerStateDX11
	{
	public:
		RasterizerStateDX11( ID3D11RasterizerState* pState );
		virtual ~RasterizerStateDX11();

	protected:
		ID3D11RasterizerState*			m_pState;

		friend RasterizerStageDX11;
	};
	typedef std::shared_ptr<RasterizerStateDX11> RasterizerStatePtr;
};
//--------------------------------------------------------------------------------
#endif // RasterizerStateDX11_h
//--------------------------------------------------------------------------------

