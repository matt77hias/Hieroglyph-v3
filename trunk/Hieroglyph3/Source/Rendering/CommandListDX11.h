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
// CommandListDX11
//
//--------------------------------------------------------------------------------
#include "RendererDX11.h"
#include "PipelineManagerDX11.h"
//--------------------------------------------------------------------------------
#ifndef CommandListDX11_h
#define CommandListDX11_h
//--------------------------------------------------------------------------------
namespace Glyph3
{
	class CommandListDX11
	{
	public:
		CommandListDX11( );
		virtual ~CommandListDX11();

		bool ListAvailable();
		void ReleaseList();

	protected:
		ID3D11CommandList*			m_pList;

		friend PipelineManagerDX11;
		friend RendererDX11;
	};
};
//--------------------------------------------------------------------------------
#endif // CommandListDX11_h
//--------------------------------------------------------------------------------

