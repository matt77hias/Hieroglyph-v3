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
// DXGIOutput
//
//--------------------------------------------------------------------------------
#include "PCH.h"
//--------------------------------------------------------------------------------
#ifndef DXGIOutput_h
#define DXGIOutput_h
//--------------------------------------------------------------------------------
namespace Glyph3
{
	class DXGIOutput
	{
	public:
		DXGIOutput( IDXGIOutput* pOutput );
		virtual ~DXGIOutput();

	protected:
		IDXGIOutput*	m_pOutput;

		friend class RendererDX11;
	};
};
//--------------------------------------------------------------------------------
#endif // DXGIOutput_h
//--------------------------------------------------------------------------------

