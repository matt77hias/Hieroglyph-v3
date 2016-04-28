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
// Texture1dDX11
//
//--------------------------------------------------------------------------------
#ifndef Texture1dDX11_h
#define Texture1dDX11_h
//--------------------------------------------------------------------------------
#include "ResourceDX11.h"
//--------------------------------------------------------------------------------
namespace Glyph3
{
	class Texture1dDX11 : public ResourceDX11
	{
	public:
		explicit Texture1dDX11( Microsoft::WRL::ComPtr<ID3D11Texture1D> pTex );
		virtual ~Texture1dDX11();

		D3D11_TEXTURE1D_DESC		GetActualDescription();
		D3D11_TEXTURE1D_DESC		GetDesiredDescription();
		void						SetDesiredDescription( D3D11_TEXTURE1D_DESC description );

		virtual ResourceType				GetType();
		virtual ID3D11Resource*				GetResource();

		virtual UINT						GetEvictionPriority();
		virtual void						SetEvictionPriority( UINT EvictionPriority );

	protected:
		Microsoft::WRL::ComPtr<ID3D11Texture1D>			m_pTexture;
		D3D11_TEXTURE1D_DESC							m_DesiredDesc;
		D3D11_TEXTURE1D_DESC							m_ActualDesc;

		friend RendererDX11;
	};
};
//--------------------------------------------------------------------------------
#endif // Texture1dDX11_h
//--------------------------------------------------------------------------------

