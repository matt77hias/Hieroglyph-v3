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
#include "DXGIAdapter.h"
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
DXGIAdapter::DXGIAdapter( IDXGIAdapter1* pAdapter )
{
	m_pAdapter = pAdapter;

	IDXGIOutput* pOutput = 0;

	while ( pAdapter->EnumOutputs( m_vOutputs.count(), &pOutput ) != DXGI_ERROR_NOT_FOUND )
	{
		m_vOutputs.add( new DXGIOutput( pOutput ) );
	}
}
//--------------------------------------------------------------------------------
DXGIAdapter::~DXGIAdapter()
{
	for ( int i = 0; i < m_vOutputs.count(); i++ )
		delete m_vOutputs[i];

	SAFE_RELEASE( m_pAdapter );
}
//--------------------------------------------------------------------------------
