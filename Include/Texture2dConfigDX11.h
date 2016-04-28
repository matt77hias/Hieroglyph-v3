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
// Texture2dConfigDX11
//
//--------------------------------------------------------------------------------
#ifndef Texture2dConfigDX11_h
#define Texture2dConfigDX11_h
//--------------------------------------------------------------------------------
#include "RendererDX11.h"
//--------------------------------------------------------------------------------
namespace Glyph3
{
	class Texture2dConfigDX11
	{
	public:
		Texture2dConfigDX11();
		virtual ~Texture2dConfigDX11();

		void SetDefaults();
		void SetDepthBuffer( UINT width, UINT height );
		void SetColorBuffer( UINT width, UINT height );

		void SetWidth( UINT state );
		void SetHeight( UINT state );
		void SetMipLevels( UINT state );
		void SetArraySize( UINT state );
		void SetFormat( DXGI_FORMAT state );
		void SetSampleDesc( DXGI_SAMPLE_DESC state );
		void SetUsage( D3D11_USAGE state ); 
		void SetBindFlags( UINT state );
		void SetCPUAccessFlags( UINT state );
		void SetMiscFlags( UINT state );

		D3D11_TEXTURE2D_DESC GetTextureDesc();

	protected:
		D3D11_TEXTURE2D_DESC 		m_State;

		friend RendererDX11;
	};
};
//--------------------------------------------------------------------------------
#endif // Texture2dConfigDX11_h
//--------------------------------------------------------------------------------

