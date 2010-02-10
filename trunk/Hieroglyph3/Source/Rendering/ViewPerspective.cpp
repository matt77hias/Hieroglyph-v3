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
#include "Log.h"
#include <sstream>
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
ViewPerspective::ViewPerspective( RendererDX11& Renderer, int Width, int Height )
{
	m_sParams.iViewType = VT_PERSPECTIVE;

//	m_sParams.iRenderTargets[0] = Renderer.CreateRenderTarget( Width, Height, 1 );
//	m_sParams.iEffects[0] = Renderer.LoadEffect( std::string( "default.fx" ) );

	//int i = m_sParams.iRenderTargets[0];
	//D3DXMatrixPerspectiveFovLH( (D3DXMATRIX*)&m_sParams.mProjMatrix, D3DX_PI/4, 
	//	Renderer.GetRTWidth( i ) / Renderer.GetRTHeight( i ), 0.1f, 1000.0f );

	//m_viewport.X = 0;
	//m_viewport.Y = 0;
	//m_viewport.Width = Width;
	//m_viewport.Height = Height;
	//m_viewport.MinZ = 0.0f;
	//m_viewport.MaxZ = 1.0f;

	m_pEntity = 0;
	m_dColor = 0x80808080;
}
//--------------------------------------------------------------------------------
ViewPerspective::ViewPerspective( RendererDX11& Renderer, int RenderTargetID )
{
	m_sParams.iViewType = VT_PERSPECTIVE;

	//m_sParams.iRenderTargets[0] = RenderTargetID;
	//m_sParams.iEffects[0] = Renderer.LoadEffect( std::string( "normals.fx" ) );

	//int i = m_sParams.iRenderTargets[0];
	//D3DXMatrixPerspectiveFovLH( (D3DXMATRIX*)&m_sParams.mProjMatrix, D3DX_PI/4, 
	//	Renderer.GetRTWidth( i ) / Renderer.GetRTHeight( i ), 0.1f, 1000.0f );

	//m_viewport.X = 0;
	//m_viewport.Y = 0;
	//m_viewport.Width = (DWORD)Renderer.GetRTWidth( RenderTargetID );
	//m_viewport.Height = (DWORD)Renderer.GetRTHeight( RenderTargetID );
	//m_viewport.MinZ = 0.0f;
	//m_viewport.MaxZ = 1.0f;

	m_pEntity = 0;
	m_dColor = 0x00000000;
}
//--------------------------------------------------------------------------------
ViewPerspective::~ViewPerspective()
{
}
//--------------------------------------------------------------------------------
void ViewPerspective::SetBackColor( DWORD color )
{
	m_dColor = color;
}
//--------------------------------------------------------------------------------
void ViewPerspective::Update( Timer& Timer )
{
}
//--------------------------------------------------------------------------------
void ViewPerspective::Draw( RendererDX11& Renderer )
{
	if ( m_pEntity != NULL )
		SetView( m_pEntity->GetView() );

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
		//Renderer.SetRenderTarget( m_sParams.iRenderTargets[0] );
		//Renderer.ClearBuffers( m_dColor, 1.0f );

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
//	Renderer.SetViewMatrix( m_sParams.mViewMatrix );
//	Renderer.SetProjMatrix( m_sParams.mProjMatrix );
}
//--------------------------------------------------------------------------------
void ViewPerspective::SetUsageParams( RendererDX11& Renderer )
{

}
//--------------------------------------------------------------------------------
