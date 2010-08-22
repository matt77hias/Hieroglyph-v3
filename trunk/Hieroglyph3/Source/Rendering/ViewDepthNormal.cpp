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
#include "ViewDepthNormal.h"
#include "Entity3D.h"
#include "Node3D.h"
#include "Texture2dConfigDX11.h"
#include "Log.h"
#include <sstream>
#include "ParameterManagerDX11.h"
#include "PipelineManagerDX11.h"
#include "Texture2dDX11.h"
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
ViewDepthNormal::ViewDepthNormal( RendererDX11& Renderer, ResourcePtr RenderTarget, ResourcePtr DepthTarget )
: ViewPerspective( Renderer, RenderTarget, DepthTarget )
{
	// This view is the same as a perspective view, with only a different 
	// rendering type as indicated here.

	m_sParams.iViewType = VT_LINEAR_DEPTH_NORMAL;

	DepthNormalBuffer = RenderTarget;
	m_RenderTarget = DepthNormalBuffer; // This is to set the buffer as the render target.
}
//--------------------------------------------------------------------------------
ViewDepthNormal::~ViewDepthNormal()
{
}
//--------------------------------------------------------------------------------
//void ViewDepthNormal::SetBackColor( Vector4f color )
//{
//	m_vColor = color;
//}
////--------------------------------------------------------------------------------
//void ViewDepthNormal::Update( float fTime )
//{
//}
////--------------------------------------------------------------------------------
//void ViewDepthNormal::PreDraw( RendererDX11* pRenderer )
//{
//	if ( m_pRoot )
//	{
//		// Queue this view into the renderer for processing.
//		pRenderer->QueueRenderView( this );
//
//		// Run through the graph and pre-render the entities
//		m_pRoot->PreRender( pRenderer, GetType() );
//	}
//}
////--------------------------------------------------------------------------------
//void ViewDepthNormal::Draw( PipelineManagerDX11* pPipelineManager, ParameterManagerDX11* pParamManager )
//{
//	if ( m_pEntity != NULL )
//		ViewMatrix = m_pEntity->GetView();
//
//
//	if ( m_pRoot )
//	{
//		// Run through the graph and pre-render the entities
//		//m_pRoot->PreRender( pPipelineManager, pParamManager, GetType() );
//
//		// Set the parameters for rendering this view
//		pPipelineManager->ClearRenderTargets();
//		pPipelineManager->BindRenderTargets( 0, m_RenderTarget );
//		pPipelineManager->BindDepthTarget( m_DepthTarget );
//		pPipelineManager->ApplyRenderTargets();
//
//		pPipelineManager->SetViewPort( m_iViewport );
//
//
//		pPipelineManager->ClearBuffers( m_vColor, 1.0f );
//
//		// Set this view's render parameters
//		SetRenderParams( pParamManager );
//
//		// Run through the graph and render each of the entities
//		m_pRoot->Render( pPipelineManager, pParamManager, GetType() );
//	}
//}
////--------------------------------------------------------------------------------
//void ViewDepthNormal::SetViewPort( DWORD x, DWORD y, DWORD w, DWORD h, float MinZ, float MaxZ )
//{
//	//m_viewport.X = x;
//	//m_viewport.Y = y;
//	//m_viewport.Width = w;
//	//m_viewport.Height = h;
//	//m_viewport.MinZ = MinZ;
//	//m_viewport.MaxZ = MaxZ;
//}
////--------------------------------------------------------------------------------
void ViewDepthNormal::SetRenderParams( ParameterManagerDX11* pParamManager )
{
	pParamManager->SetViewMatrixParameter( &ViewMatrix );
	pParamManager->SetProjMatrixParameter( &ProjMatrix );
}

//--------------------------------------------------------------------------------
void ViewDepthNormal::SetUsageParams( ParameterManagerDX11* pParamManager )
{
	pParamManager->SetShaderResourceParameter( L"DepthNormalBuffer", DepthNormalBuffer );
}
//--------------------------------------------------------------------------------
