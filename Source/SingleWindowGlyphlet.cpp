//--------------------------------------------------------------------------------
// This file is a portion of the Hieroglyph 3 Rendering Engine.  It is distributed
// under the MIT License, available in the root of this distribution and 
// at the following URL:
//
// http://www.opensource.org/licenses/mit-license.php
//
// Copyright (c) Jason Zink 
//--------------------------------------------------------------------------------
#include "PCH.h"
#include "SingleWindowGlyphlet.h"
#include "ViewPerspective.h"
#include "FirstPersonCamera.h"
#include "Texture2dDX11.h"
#include "GeometryActor.h"
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
SingleWindowGlyphlet::SingleWindowGlyphlet()
{
	SetEventManager( &EvtManager );

	RequestEvent( SYSTEM_KEYBOARD_KEYDOWN );


	RendererDX11* pRenderer = RendererDX11::Get();

	m_pScene = new Scene();

	ViewPerspective* pPerspectiveView = new ViewPerspective( *pRenderer, m_RenderTarget );
	pPerspectiveView->SetBackColor( Vector4f( 0.6f, 0.6f, 0.9f, 1.0f ) );
	m_pRenderView = pPerspectiveView;

	m_pCamera = new FirstPersonCamera();
	m_pCamera->SetEventManager( &EvtManager );
	
	m_pCamera->Spatial().SetRotation( Vector3f( 0.0f, 0.0f, 0.0f ) );
	m_pCamera->Spatial().SetTranslation( Vector3f( 0.0f, 10.0f, -20.0f ) );
	m_pCamera->SetCameraView( m_pRenderView );
	m_pCamera->SetProjectionParams( 0.1f, 1000.0f, 1.0f / 1.0f, static_cast<float>( GLYPH_PI ) / 4.0f );

	m_pScene->AddCamera( m_pCamera );
}
//--------------------------------------------------------------------------------
SingleWindowGlyphlet::~SingleWindowGlyphlet()
{
	SAFE_DELETE( m_pScene );
}
//--------------------------------------------------------------------------------
void SingleWindowGlyphlet::Setup( ResourcePtr target )
{
	RendererDX11* pRenderer = RendererDX11::Get();

	m_RenderTarget = target;

	ResourceDX11* pResource = pRenderer->GetResourceByIndex( m_RenderTarget->m_iResource );

	if ( pResource->GetType() == RT_TEXTURE2D )
	{
		Texture2dDX11* pTexture = (Texture2dDX11*)pResource;
		D3D11_TEXTURE2D_DESC desc = pTexture->GetActualDescription();

		m_iWidth = desc.Width;
		m_iHeight = desc.Height;

		m_pCamera->SetProjectionParams( 0.1f, 1000.0f, static_cast<float>(m_iWidth) / static_cast<float>(m_iHeight), static_cast<float>( GLYPH_PI ) / 4.0f );
	} else {
		// TODO: add error handling here.
	}

	dynamic_cast<ViewPerspective*>( m_pRenderView )->SetRenderTargets( m_RenderTarget );
}
//--------------------------------------------------------------------------------
void SingleWindowGlyphlet::Initialize()
{
}
//--------------------------------------------------------------------------------
void SingleWindowGlyphlet::Update( float dt )
{
	// Update and render your scene here...

	m_pScene->Update( dt );
	m_pScene->Render( RendererDX11::Get() );
}
//--------------------------------------------------------------------------------
void SingleWindowGlyphlet::Shutdown()
{
}
//--------------------------------------------------------------------------------
bool SingleWindowGlyphlet::HandleEvent( EventPtr pEvent )
{

	return( false );
}
//--------------------------------------------------------------------------------