//--------------------------------------------------------------------------------
// This file is a portion of the Hieroglyph 3 Rendering Engine.  It is distributed
// under the MIT License, available in the root of this distribution and 
// at the following URL:
//
// http://www.opensource.org/licenses/mit-license.php
//
// Copyright (c) Jason Zink 
//--------------------------------------------------------------------------------
#pragma once
#include <d3d11.h>
//--------------------------------------------------------------------------------
namespace Glyph3 {

	class EventManager;
	class RendererDX11;
	class ParticleStormGlyphlet;

	class NativeGlyph
	{
	public:
		NativeGlyph();

		void Initialize();
		ID3D11Texture2D* GetRenderTarget();
		void Update( float time );
		void Shutdown();

	private:
		EventManager* m_pEventManager;
		RendererDX11* m_pRenderer;
		ID3D11Texture2D* m_pTexture;
		ParticleStormGlyphlet* m_pGlyphlet;
	};
}
//--------------------------------------------------------------------------------