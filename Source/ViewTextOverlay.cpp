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
#include "ViewTextOverlay.h"
#include "Entity3D.h"
#include "Node3D.h"
#include "Texture2dConfigDX11.h"
#include "Log.h"
#include "IParameterManager.h"
#include "PipelineManagerDX11.h"
#include "Texture2dDX11.h"
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
ViewTextOverlay::ViewTextOverlay( RendererDX11& Renderer, ResourcePtr RenderTarget )
{
	//m_sParams.iViewType = VT_GUI_SKIN;

	m_RenderTarget = RenderTarget;

	//ViewMatrix.MakeIdentity();
	//ProjMatrix.MakeIdentity();

	//m_pEntity = 0;
	m_vColor.MakeZero();

	ResourceDX11* pResource = Renderer.GetResourceByIndex( m_RenderTarget->m_iResource );

	if ( pResource->GetType() == RT_TEXTURE2D )
	{
		Texture2dDX11* pTexture = (Texture2dDX11*)pResource;
		D3D11_TEXTURE2D_DESC desc = pTexture->GetActualDescription();

		// Create a view port to use on the scene.  This basically selects the 
		// entire floating point area of the render target.
		D3D11_VIEWPORT viewport;
		viewport.Width = static_cast< float >( desc.Width );
		viewport.Height = static_cast< float >( desc.Height );
		viewport.MinDepth = 0.0f;
		viewport.MaxDepth = 1.0f;
		viewport.TopLeftX = 0;
		viewport.TopLeftY = 0;

		m_iViewport = Renderer.CreateViewPort( viewport );
	}

	// Create the text rendering classes.
	m_pSpriteFont = SpriteFontLoaderDX11::LoadFont( std::wstring( L"Consolas" ), 12.0f, 0, false );
	
	m_pSpriteRenderer = new SpriteRendererDX11();
	m_pSpriteRenderer->Initialize();
}
//--------------------------------------------------------------------------------
ViewTextOverlay::~ViewTextOverlay()
{
	SAFE_DELETE( m_pSpriteRenderer );
}
//--------------------------------------------------------------------------------
void ViewTextOverlay::Update( float fTime )
{
}
//--------------------------------------------------------------------------------
void ViewTextOverlay::QueuePreTasks( RendererDX11* pRenderer )
{
	// Queue this view into the renderer for processing.
	pRenderer->QueueTask( this );
}
//--------------------------------------------------------------------------------
void ViewTextOverlay::ExecuteTask( PipelineManagerDX11* pPipelineManager, IParameterManager* pParamManager )
{
	if ( m_TextEntries.size() > 0 ) {
		// Set the parameters for rendering this view
		pPipelineManager->ClearRenderTargets();
		pPipelineManager->OutputMergerStage.DesiredState.RenderTargetViews.SetState( 0, m_RenderTarget->m_iResourceRTV );
		pPipelineManager->ApplyRenderTargets();

		pPipelineManager->RasterizerStage.DesiredState.ViewportCount.SetState( 1 );
		pPipelineManager->RasterizerStage.DesiredState.Viewports.SetState( 0, m_iViewport );
		pPipelineManager->RasterizerStage.DesiredState.RasterizerState.SetState( 0 );

		// Set default states for these stages
		pPipelineManager->OutputMergerStage.DesiredState.DepthStencilState.SetState( 0 );
		pPipelineManager->OutputMergerStage.DesiredState.BlendState.SetState( 0 );

		for ( auto entry : m_TextEntries )
		{
			m_pSpriteRenderer->RenderText( pPipelineManager, pParamManager, m_pSpriteFont, entry.text.c_str(), entry.xform, entry.color );
		}

		m_TextEntries.clear();
	}
}
//--------------------------------------------------------------------------------
void ViewTextOverlay::Resize( UINT width, UINT height )
{
	RendererDX11::Get()->ResizeViewport( m_iViewport, width, height );
}
//--------------------------------------------------------------------------------
void ViewTextOverlay::SetRenderParams( IParameterManager* pParamManager )
{
//	pParamManager->SetViewMatrixParameter( &ViewMatrix );
//	pParamManager->SetProjMatrixParameter( &ProjMatrix );
}
//--------------------------------------------------------------------------------
void ViewTextOverlay::SetUsageParams( IParameterManager* pParamManager )
{

}
//--------------------------------------------------------------------------------
void ViewTextOverlay::WriteText( std::wstring& text, Matrix4f& xform, Vector4f& color )
{
	m_TextEntries.push_back( TextEntry( text, xform, color ) );
}
//--------------------------------------------------------------------------------
std::wstring ViewTextOverlay::GetName()
{
	return( L"ViewTextOverlay" );
}
//--------------------------------------------------------------------------------
