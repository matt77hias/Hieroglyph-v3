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
#include "ViewGBuffer.h"
#include "Entity3D.h"
#include "Node3D.h"
#include "Texture2dConfigDX11.h"
#include "Log.h"
#include "ParameterManagerDX11.h"
#include "PipelineManagerDX11.h"
#include "Texture2dDX11.h"
#include "DepthStencilStateConfigDX11.h"
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
ViewGBuffer::ViewGBuffer( RendererDX11& Renderer, ResourcePtr DepthTarget )
    : m_Renderer( Renderer ) , m_DepthTarget( DepthTarget )
{
	m_sParams.iViewType = VT_GBUFFER;

	ViewMatrix.MakeIdentity();
	ProjMatrix.MakeIdentity();		

	// Create the depth stencil view. We'll enable stencil writes and depth writes
	DepthStencilStateConfigDX11 dsConfig;
	dsConfig.DepthEnable = true;
	dsConfig.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	dsConfig.DepthFunc = D3D11_COMPARISON_LESS;
	dsConfig.StencilEnable = true;
	dsConfig.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
	dsConfig.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;
	dsConfig.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	dsConfig.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	dsConfig.FrontFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE;
	dsConfig.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	dsConfig.BackFace = dsConfig.FrontFace;

	m_iDepthStencilState = Renderer.CreateDepthStencilState( &dsConfig );

	if ( m_iDepthStencilState == -1 )	
		Log::Get().Write( L"Failed to create G-Buffer depth stencil state" );			
}
//--------------------------------------------------------------------------------
ViewGBuffer::~ViewGBuffer()
{
}
//--------------------------------------------------------------------------------
void ViewGBuffer::Update( float fTime )
{
}
//--------------------------------------------------------------------------------
void ViewGBuffer::PreDraw( RendererDX11* pRenderer )
{
	if ( m_pEntity != NULL )
	{
		Matrix4f view = m_pEntity->GetView();
		SetViewMatrix( view );
	}

	// Queue this view into the renderer for processing.
	pRenderer->QueueRenderView( this );

	if ( m_pRoot )
	{
		// Run through the graph and pre-render the entities
		m_pRoot->PreRender( pRenderer, GetType() );
	}
}
//--------------------------------------------------------------------------------
void ViewGBuffer::Draw( PipelineManagerDX11* pPipelineManager, ParameterManagerDX11* pParamManager )
{
	if ( m_pRoot )
	{
		// Set the parameters for rendering this view
		pPipelineManager->ClearRenderTargets();
		for ( int i = 0; i < m_GBufferTargets.count(); ++i)
			pPipelineManager->BindRenderTargets( i, m_GBufferTargets[i] );
		pPipelineManager->BindDepthTarget( m_DepthTarget );
		pPipelineManager->ApplyRenderTargets();

		pPipelineManager->SetViewPort( m_iViewport );

		// Set default states for these stages
		pPipelineManager->SetRasterizerState( 0 );		
		pPipelineManager->SetBlendState( 0 );

		// Set our depth stencil state
		pPipelineManager->SetDepthStencilState( m_iDepthStencilState, StencilRef );

		// Clear the G-Buffer targets
		Vector4f color(0.0f, 0.0f, 0.0f, 0.0f);
		pPipelineManager->ClearBuffers( color, 1.0f, 0 );

		// Set this view's render parameters
		SetRenderParams( pParamManager );

		// Run through the graph and render each of the entities
		m_pRoot->Render( pPipelineManager, pParamManager, GetType() );
	}
}
//--------------------------------------------------------------------------------
void ViewGBuffer::SetRenderParams( ParameterManagerDX11* pParamManager )
{
	pParamManager->SetViewMatrixParameter( &ViewMatrix );
	pParamManager->SetProjMatrixParameter( &ProjMatrix );
}
//--------------------------------------------------------------------------------
void ViewGBuffer::SetUsageParams( ParameterManagerDX11* pParamManager )
{

}
//--------------------------------------------------------------------------------
void ViewGBuffer::SetGBufferTargets( TArray<ResourcePtr>& GBufferTargets )
{
    m_GBufferTargets = GBufferTargets;    

    // Create the viewport based on the first render target
    ResourceDX11* pResource = m_Renderer.GetResource( m_GBufferTargets[0]->m_iResource & 0x0000ffff );

    if ( pResource->GetType() == D3D11_RESOURCE_DIMENSION_TEXTURE2D )
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

        m_iViewport = m_Renderer.CreateViewPort( viewport );
    }
}