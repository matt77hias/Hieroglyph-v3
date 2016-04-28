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
// DXGIAdapter
//
//--------------------------------------------------------------------------------
#include "PCH.h"
//--------------------------------------------------------------------------------
#ifndef DXGIAdapter_h
#define DXGIAdapter_h
//--------------------------------------------------------------------------------
namespace Glyph3
{
	class DXGIOutput;

	class DXGIAdapter
	{
	public:
		DXGIAdapter( Microsoft::WRL::ComPtr<IDXGIAdapter1> pAdapter );
		virtual ~DXGIAdapter();

	protected:
		Microsoft::WRL::ComPtr<IDXGIAdapter1>		m_pAdapter;
		std::vector<DXGIOutput>						m_vOutputs;

		friend class RendererDX11;
	};
};
//--------------------------------------------------------------------------------
#endif // DXGIAdapter_h
//--------------------------------------------------------------------------------

