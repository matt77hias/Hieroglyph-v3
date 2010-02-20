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
#include "Camera.h"
#include "Scene.h"
#include <sstream>
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
Camera::Camera()
{
	m_pCameraView = 0;
	m_pScene = 0;

	m_fNear = 0.1f;
	m_fFar = 100.0f;
	m_fAspect = 1280.0f / 800.0f;
	m_fFov = D3DX_PI/4;
}
//--------------------------------------------------------------------------------
Camera::~Camera()
{
	// TODO: should the camera delete a renderview that has been added to it???
}
//--------------------------------------------------------------------------------
void Camera::RenderFrame( RendererDX11& Renderer )
{
	if ( m_pCameraView )
	{
		// Set the scene's root into the render view

		if ( m_pScene )
			m_pCameraView->SetRoot( m_pScene->GetRoot() );

		// Execute the render view

		m_pCameraView->Draw( Renderer );
	}
}
//--------------------------------------------------------------------------------
void Camera::SetCameraView( IRenderView* pView )
{
	m_pCameraView = pView;
	m_pCameraView->SetEntity( m_pBody );
}
//--------------------------------------------------------------------------------
void Camera::SetScene( Scene* pScene )
{
	m_pScene = pScene;
}
//--------------------------------------------------------------------------------
void Camera::SetProjectionParams( float zn, float zf, float aspect, float fov )
{
	m_fNear = zn;
	m_fFar = zf;
	m_fAspect = aspect;
	m_fFov = fov;

	if ( m_pCameraView )
	{
		// Calculate and set the projection matrix for the view.
		D3DXMatrixPerspectiveFovLH( (D3DXMATRIX*)&m_pCameraView->ProjMatrix, m_fFov, 
			m_fAspect, m_fNear, m_fFar );
	}
}
//--------------------------------------------------------------------------------