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
}
//--------------------------------------------------------------------------------
Camera::~Camera()
{
}
//--------------------------------------------------------------------------------
void Camera::RenderFrame()
{
	if ( m_pCameraView )
	{
		// Set the scene's root into the render view

		if ( m_pScene )
			m_pCameraView->SetRoot( m_pScene->GetRoot() );

		// Execute the render view

		//RendererDX11::Get()->AddToViewList( m_pCameraView );
		//RendererDX11::Get()->RenderViewList();
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