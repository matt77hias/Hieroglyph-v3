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