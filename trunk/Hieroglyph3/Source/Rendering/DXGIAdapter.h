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
// DXGIAdapter
//
//--------------------------------------------------------------------------------
#include "RendererDX11.h"
#include "DXGIOutput.h"
//--------------------------------------------------------------------------------
#ifndef DXGIAdapter_h
#define DXGIAdapter_h
//--------------------------------------------------------------------------------
namespace Glyph3
{
	class DXGIAdapter
	{
	public:
		DXGIAdapter( IDXGIAdapter1* pAdapter );
		virtual ~DXGIAdapter();

	protected:
		IDXGIAdapter1*			m_pAdapter;
		TArray<DXGIOutput*>		m_vOutputs;

		friend RendererDX11;
	};
};
//--------------------------------------------------------------------------------
#endif // DXGIAdapter_h
//--------------------------------------------------------------------------------

