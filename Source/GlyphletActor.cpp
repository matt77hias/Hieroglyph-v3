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
#include "GlyphletActor.h"
#include "Texture2dConfigDX11.h"
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
GlyphletActor::GlyphletActor()
{
	// Create the texture that we will be using to collect the results from the 
	// Glyphlet.

	Texture2dConfigDX11 config;
	config.SetFormat( DXGI_FORMAT_R8G8B8A8_UNORM );
	config.SetBindFlags( D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE );
	config.SetWidth( 640 );
	config.SetHeight( 480 );
	
	m_RenderTexture = RendererDX11::Get()->CreateTexture2D( &config, nullptr );

	UseTexturedMaterial( m_RenderTexture );
	DrawRect( Vector3f( 0.0f, 0.0f, 0.0f ), Vector3f( -1.0f, 0.0f, 0.0f ), Vector3f( 0.0f, 1.0f, 0.0f ), Vector2f( 3.2f, 2.4f ) );
}
//--------------------------------------------------------------------------------
void GlyphletActor::SetGlyphlet( std::shared_ptr<SingleWindowGlyphlet> pGlyphlet )
{
	m_pGlyphlet = pGlyphlet;
	m_pGlyphlet->Setup( m_RenderTexture );

	// Execute the glyphlet via its generic interface...

	m_pGlyphlet->Update( 0.0f );
}
//--------------------------------------------------------------------------------
GlyphletActor::~GlyphletActor()
{
	m_pGlyphlet->Shutdown();
	m_pGlyphlet = nullptr;
}
//--------------------------------------------------------------------------------
