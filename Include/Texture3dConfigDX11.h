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
// Texture3dConfigDX11
//
//--------------------------------------------------------------------------------
#ifndef Texture3dConfigDX11_h
#define Texture3dConfigDX11_h
//--------------------------------------------------------------------------------
#include "RendererDX11.h"
//--------------------------------------------------------------------------------
namespace Glyph3
{
	class Texture3dConfigDX11
	{
	public:
		Texture3dConfigDX11();
		virtual ~Texture3dConfigDX11();

		void SetDefaults();

		void SetWidth( UINT state );
		void SetHeight( UINT state );
		void SetDepth( UINT state );
		void SetMipLevels( UINT state );
		void SetFormat( DXGI_FORMAT state );
		void SetUsage( D3D11_USAGE state ); 
		void SetBindFlags( UINT state );
		void SetCPUAccessFlags( UINT state );
		void SetMiscFlags( UINT state );

		D3D11_TEXTURE3D_DESC GetTextureDesc();

	protected:
		D3D11_TEXTURE3D_DESC 		m_State;

		friend RendererDX11;
	};
};
//--------------------------------------------------------------------------------
#endif // Texture3dConfigDX11_h
//--------------------------------------------------------------------------------

