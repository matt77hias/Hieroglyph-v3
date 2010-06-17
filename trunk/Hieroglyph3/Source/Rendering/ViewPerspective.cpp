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
#include "ViewPerspective.h"
#include "Entity3D.h"
#include "Node3D.h"
#include "Texture2dConfigDX11.h"
#include "Log.h"
#include <sstream>
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
ViewPerspective::ViewPerspective( RendererDX11& Renderer, ResourcePtr RenderTarget, ResourcePtr DepthTarget )
{
	m_sParams.iViewType = VT_PERSPECTIVE;

	m_RenderTarget = RenderTarget;
	m_DepthTarget = DepthTarget;

	ViewMatrix.MakeIdentity();
	ProjMatrix.MakeIdentity();

	m_pEntity = 0;
	m_vColor.MakeZero();
}
//--------------------------------------------------------------------------------
ViewPerspective::~ViewPerspective()
{
}
//--------------------------------------------------------------------------------
void ViewPerspective::SetBackColor( Vector4f color )
{
	m_vColor = color;
}
//--------------------------------------------------------------------------------
void ViewPerspective::Update( float fTime )
{
}
//--------------------------------------------------------------------------------
void ViewPerspective::Draw( RendererDX11& Renderer )
{
	if ( m_pEntity != NULL )
		ViewMatrix = m_pEntity->GetView();


	if ( m_pRoot )
	{
		// Run through the graph and pre-render the entities
		m_pRoot->PreRender( Renderer, GetType() );

		// Set the parameters for rendering this view
		Renderer.ClearRenderTargets();
		Renderer.BindRenderTargets( 0, m_RenderTarget );
		Renderer.BindDepthTarget( m_DepthTarget );
		Renderer.ApplyRenderTargets();

		Renderer.ClearBuffers( m_vColor, 1.0f );

		// Set this view's render parameters
		SetRenderParams( Renderer );

		// Run through the graph and render each of the entities
		m_pRoot->Render( Renderer, GetType() );
	}
}
//--------------------------------------------------------------------------------
void ViewPerspective::SetViewPort( DWORD x, DWORD y, DWORD w, DWORD h, float MinZ, float MaxZ )
{
	//m_viewport.X = x;
	//m_viewport.Y = y;
	//m_viewport.Width = w;
	//m_viewport.Height = h;
	//m_viewport.MinZ = MinZ;
	//m_viewport.MaxZ = MaxZ;
}
//--------------------------------------------------------------------------------
void ViewPerspective::SetRenderParams( RendererDX11& Renderer )
{
	Renderer.SetViewMatrixParameter( &ViewMatrix );
	Renderer.SetProjMatrixParameter( &ProjMatrix );
}
//--------------------------------------------------------------------------------
void ViewPerspective::SetUsageParams( RendererDX11& Renderer )
{

}
//--------------------------------------------------------------------------------
