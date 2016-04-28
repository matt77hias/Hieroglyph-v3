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
// Texture1dConfigDX11
//
//
// Copyright (C) 2003-2009 Jason Zink
//--------------------------------------------------------------------------------
#ifndef Texture1dConfigDX11_h
#define Texture1dConfigDX11_h
//--------------------------------------------------------------------------------
#include "RendererDX11.h"
//--------------------------------------------------------------------------------
namespace Glyph3
{
	class Texture1dConfigDX11
	{
	public:
		Texture1dConfigDX11();
		virtual ~Texture1dConfigDX11();

		void SetDefaults();

		void SetWidth( UINT state );
		void SetMipLevels( UINT state );
		void SetArraySize( UINT state );
		void SetFormat( DXGI_FORMAT state );
		void SetUsage( D3D11_USAGE state ); 
		void SetBindFlags( UINT state );
		void SetCPUAccessFlags( UINT state );
		void SetMiscFlags( UINT state );

		D3D11_TEXTURE1D_DESC GetTextureDesc();

	protected:
		D3D11_TEXTURE1D_DESC 		m_State;

		friend RendererDX11;
	};
};
//--------------------------------------------------------------------------------
#endif // Texture1dConfigDX11_h
//--------------------------------------------------------------------------------

