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
#include "ViewLinearDepthNormals.h"
#include "Entity3D.h"
#include "Node3D.h"
#include "Texture2dConfigDX11.h"
#include "Log.h"
#include <sstream>
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
ViewLinearDepthNormals::ViewLinearDepthNormals( RendererDX11& Renderer, ResourcePtr RenderTarget, ResourcePtr DepthTarget )
{
	m_sParams.iViewType = VT_LINEAR_DEPTH_NORMALS;

	m_RenderTarget = RenderTarget;
	m_DepthTarget = DepthTarget;

	ViewMatrix.MakeIdentity();
	ProjMatrix.MakeIdentity();

	m_pEntity = 0;
	m_vColor.MakeZero();
}
//--------------------------------------------------------------------------------
ViewLinearDepthNormals::~ViewLinearDepthNormals()
{
}
//--------------------------------------------------------------------------------
void ViewLinearDepthNormals::SetBackColor( Vector4f color )
{
	m_vColor = color;
}
//--------------------------------------------------------------------------------
void ViewLinearDepthNormals::Update( float fTime )
{
}
//--------------------------------------------------------------------------------
void ViewLinearDepthNormals::Draw( RendererDX11& Renderer )
{
	if ( m_pEntity != NULL )
		ViewMatrix = m_pEntity->GetView();

//	std::ostringstream s;
//	s << "Perspective: " << this;
//	s << ", Recurrence: " << m_iCurrRecurrence;
//	CLog::Get().Write( s.str() );

	//CEntity3D* pRoot = CEntity3D::m_pRoot;

	if ( m_pRoot )
	{
		// Run through the graph and pre-render the entities
		m_pRoot->PreRender( Renderer, GetType() );

		// Set the parameters for rendering this view
		// TODO: add the depth id into the render view!
		Renderer.BindRenderTargets( m_RenderTarget, m_DepthTarget );
		Renderer.ClearBuffers( m_vColor, 1.0f );

		// Set this view's render parameters
		SetRenderParams( Renderer );

		// Run through the graph and render each of the entities
		m_pRoot->Render( Renderer, GetType() );
	}
}
//--------------------------------------------------------------------------------
void ViewLinearDepthNormals::SetViewPort( DWORD x, DWORD y, DWORD w, DWORD h, float MinZ, float MaxZ )
{
	//m_viewport.X = x;
	//m_viewport.Y = y;
	//m_viewport.Width = w;
	//m_viewport.Height = h;
	//m_viewport.MinZ = MinZ;
	//m_viewport.MaxZ = MaxZ;
}
//--------------------------------------------------------------------------------
void ViewLinearDepthNormals::SetRenderParams( RendererDX11& Renderer )
{
	Renderer.SetViewMatrixParameter( &ViewMatrix );
	Renderer.SetProjMatrixParameter( &ProjMatrix );
}
//--------------------------------------------------------------------------------
void ViewLinearDepthNormals::SetUsageParams( RendererDX11& Renderer )
{

}
//--------------------------------------------------------------------------------
