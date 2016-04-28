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
#include "PCH.h"
#include "DXGIAdapter.h"
#include "DXGIOutput.h"
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
DXGIAdapter::DXGIAdapter( Microsoft::WRL::ComPtr<IDXGIAdapter1> pAdapter )
{
	m_pAdapter = pAdapter;

	Microsoft::WRL::ComPtr<IDXGIOutput> pOutput;

	while ( pAdapter->EnumOutputs( m_vOutputs.size(), pOutput.ReleaseAndGetAddressOf() ) != DXGI_ERROR_NOT_FOUND )
	{
		m_vOutputs.push_back( pOutput );
	}
}
//--------------------------------------------------------------------------------
DXGIAdapter::~DXGIAdapter()
{
}
//--------------------------------------------------------------------------------
