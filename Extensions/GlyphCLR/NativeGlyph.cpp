//--------------------------------------------------------------------------------
// This file is a portion of the Hieroglyph 3 Rendering Engine.  It is distributed
// under the MIT License, available in the root of this distribution and 
// at the following URL:
//
// http://www.opensource.org/licenses/mit-license.php
//
// Copyright (c) Jason Zink 
//--------------------------------------------------------------------------------
#include "NativeGlyph.h"
#include "EventManager.h"
#include "RendererDX11.h"
#include "ParticleStormGlyphlet.h"
#include "Texture2dDX11.h"
#include "Texture2dConfigDX11.h"
#include "FileSystem.h"
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
NativeGlyph::NativeGlyph()
{
	// Create the framework stuff to be used by the glyphlets...
	Log::Get().Open();
	m_pEventManager = new EventManager();
	m_pRenderer = new RendererDX11();
}
//--------------------------------------------------------------------------------
void NativeGlyph::Initialize()
{
	if ( !m_pRenderer->Initialize( D3D_DRIVER_TYPE_HARDWARE, D3D_FEATURE_LEVEL_11_0 ) )
	{
		//Log::Get().Write( L"Could not create hardware device, trying to create the reference device..." );

		if ( !m_pRenderer->Initialize( D3D_DRIVER_TYPE_REFERENCE, D3D_FEATURE_LEVEL_11_0 ) )
		{
			//MessageBox( 0, L"Could not create a hardware or software Direct3D 11 device - the program will now abort!", L"Hieroglyph 3 Rendering", MB_ICONEXCLAMATION | MB_SYSTEMMODAL );
			//RequestTermination();			
			//return( false );
		}

		// If using the reference device, utilize a fixed time step for any animations.
		//m_pTimer->SetFixedTimeStep( 1.0f / 10.0f );
	}

	
	// Create the resource to be used for rendering into and for being 
	// shared with D3D9.  This has a number of requirements including that 
	// it must have the render target and shader resource bind flags, it
	// must set the shared resource miscellaneous flag, and the format is
	// restricted to an 8-bit UNORM format.  If one or more of these
	// requirements aren't met, the eventual call to IDXGIResource::GetSharedHandle
	// will fail.

	Texture2dConfigDX11 cfg;
	cfg.SetWidth( 800 );
	cfg.SetHeight( 600 );
	cfg.SetMipLevels( 1 );
	cfg.SetArraySize( 1 );
	cfg.SetFormat( DXGI_FORMAT_B8G8R8A8_UNORM ); 
	cfg.SetUsage( D3D11_USAGE_DEFAULT );
	cfg.SetBindFlags( D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE );
	cfg.SetCPUAccessFlags( 0 );
	cfg.SetMiscFlags( D3D11_RESOURCE_MISC_SHARED );
	
	ResourcePtr resource = m_pRenderer->CreateTexture2D( &cfg, nullptr );
	m_pTexture = reinterpret_cast<ID3D11Texture2D*>( m_pRenderer->GetTexture2DByIndex( resource->m_iResource )->GetResource() );

	// Create the Glyphlet to display in the window.  The texture is set
	// in the glyphlet, which links the native D3D11 code to the shared
	// resource.  The connection to D3D9 is done in the D3DImageEx class.

	m_pGlyphlet = new ParticleStormGlyphlet();
	m_pGlyphlet->Initialize();
	m_pGlyphlet->Setup( resource );
}
//--------------------------------------------------------------------------------
ID3D11Texture2D* NativeGlyph::GetRenderTarget()
{
	return( m_pTexture );
}
//--------------------------------------------------------------------------------
void NativeGlyph::Update( float time )
{
	m_pGlyphlet->Update( time );
	m_pRenderer->pImmPipeline->m_pContext->Flush();
}
//--------------------------------------------------------------------------------
void NativeGlyph::Shutdown()
{
	m_pGlyphlet->Shutdown();
}
//--------------------------------------------------------------------------------