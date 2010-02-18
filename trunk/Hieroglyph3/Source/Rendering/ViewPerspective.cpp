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
ViewPerspective::ViewPerspective( RendererDX11& Renderer, int RenderTargetViewID )
{
	m_sParams.iViewType = VT_PERSPECTIVE;

	m_iRenderTargetViewID = RenderTargetViewID;

	D3DXVECTOR3 vLookAt = D3DXVECTOR3( 0.0f, 0.75f, 0.0f );
	D3DXVECTOR3 vLookFrom = D3DXVECTOR3( -15.0f, 5.5f, -5.0f );
	D3DXVECTOR3 vLookUp = D3DXVECTOR3( 0.0f, 1.0f, 0.0f );
	D3DXMatrixLookAtLH( (D3DXMATRIX*)&ViewMatrix, &vLookFrom, &vLookAt, &vLookUp );

	//ViewMatrix.MakeIdentity();

	D3DXMatrixPerspectiveFovLH( (D3DXMATRIX*)&ProjMatrix, D3DX_PI/4, 
		640.0f / 480.0f, 0.1f, 100.0f );

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
void ViewPerspective::Update( Timer& Timer )
{
}
//--------------------------------------------------------------------------------
void ViewPerspective::Draw( RendererDX11& Renderer )
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
		Renderer.BindRenderTargets( m_iRenderTargetViewID, 0 );
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
