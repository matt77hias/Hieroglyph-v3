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
#include "DXGIOutput.h"
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
DXGIOutput::DXGIOutput( IDXGIOutput* pOutput )
{
	m_pOutput = pOutput;
}
//--------------------------------------------------------------------------------
DXGIOutput::~DXGIOutput()
{
	SAFE_RELEASE( m_pOutput );
}
//--------------------------------------------------------------------------------
