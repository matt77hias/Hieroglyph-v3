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
#include "PCH.h"
#include "Camera.h"
#include "Scene.h"
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
Camera::Camera()
{
	m_pCameraView = 0;
	m_pOverlayView = 0;
	m_pScene = 0;

	m_fNear = 0.1f;
	m_fFar = 100.0f;
	m_fAspect = 1280.0f / 800.0f;
	m_fFov = static_cast<float>( GLYPH_PI ) / 4.0f;

    m_ProjMatrix.MakeIdentity();

	m_pViewPositionParameter = nullptr;
}
//--------------------------------------------------------------------------------
Camera::~Camera()
{
	// If a render view has been added to the camera, then delete it when the 
	// camera is released.

	SAFE_DELETE( m_pCameraView );
	SAFE_DELETE( m_pOverlayView );
}
//--------------------------------------------------------------------------------
void Camera::RenderFrame( RendererDX11* pRenderer )
{
	// If an overlay is available, add it to the renderer first so that it will
	// be executed last.

	if ( m_pOverlayView )
		pRenderer->QueueRenderView( m_pOverlayView );

	// If a render view for the camera is available, then render it with the given
	// scene.

	if ( m_pCameraView )
	{
		// Set the scene's root into the render view

		if ( m_pScene ) {
			m_pCameraView->SetRoot( m_pScene->GetRoot() );
		}


		// Set the view position in the parameter system, for use by any of the
		// views being used in this frame.

		if ( !m_pViewPositionParameter ) {
			m_pViewPositionParameter = pRenderer->m_pParamMgr->GetVectorParameterRef( std::wstring( L"ViewPosition" ) );
		}

		Vector3f p = GetNode()->Position() + GetBody()->Position();
		Vector4f ViewPosition( p.x, p.y, p.z, 1.0f );
		m_pViewPositionParameter->InitializeParameterData( &ViewPosition );


		// Use the pre-draw method to queue any needed render views in the scene.
		// This is followed by rendering all of the views to generate the current
		// frame.

		m_pCameraView->PreDraw( pRenderer );
		pRenderer->ProcessRenderViewQueue();
	}
}
//--------------------------------------------------------------------------------
void Camera::SetCameraView( IRenderView* pView )
{
	m_pCameraView = pView;
	m_pCameraView->SetEntity( m_pBody );
}
//--------------------------------------------------------------------------------
void Camera::SetOverlayView( IRenderView* pView )
{
	m_pOverlayView = pView;
}
//--------------------------------------------------------------------------------
void Camera::SetScene( Scene* pScene )
{
	m_pScene = pScene;
}
//--------------------------------------------------------------------------------
IRenderView* Camera::GetCameraView()
{
	return( m_pCameraView );
}
//--------------------------------------------------------------------------------
IRenderView* Camera::GetOverlayView()
{
	return( m_pOverlayView );
}
//--------------------------------------------------------------------------------
Scene* Camera::GetScene()
{
	return( m_pScene );
}
//--------------------------------------------------------------------------------
void Camera::SetProjectionParams( float zn, float zf, float aspect, float fov )
{
	m_fNear = zn;
	m_fFar = zf;
	m_fAspect = aspect;
	m_fFov = fov;

	ApplyProjectionParams();
}
//--------------------------------------------------------------------------------
void Camera::SetClipPlanes( float zn, float zf )
{
	m_fNear = zn;
	m_fFar = zf;

	ApplyProjectionParams();
}
//--------------------------------------------------------------------------------
void Camera::SetAspectRatio( float aspect )
{
	m_fAspect = aspect;

	ApplyProjectionParams();
}
//--------------------------------------------------------------------------------
void Camera::SetFieldOfView( float fov )
{
	m_fFov = fov;

	ApplyProjectionParams();
}
//--------------------------------------------------------------------------------
float Camera::GetNearClipPlane()
{
	return( m_fNear );
}
//--------------------------------------------------------------------------------
float Camera::GetFarClipPlane()
{
	return( m_fFar );
}
//--------------------------------------------------------------------------------
float Camera::GetAspectRatio()
{
	return( m_fAspect );
}
//--------------------------------------------------------------------------------
float Camera::GetFieldOfView()
{
	return( m_fFov );
}
//--------------------------------------------------------------------------------
void Camera::ApplyProjectionParams()
{
	m_ProjMatrix = Matrix4f::PerspectiveFovLHMatrix( m_fFov, m_fAspect, m_fNear, m_fFar );

	if ( m_pCameraView )
		m_pCameraView->SetProjMatrix( m_ProjMatrix );	
}
//--------------------------------------------------------------------------------
const Matrix4f& Camera::ProjMatrix()
{
    return m_ProjMatrix;
}
//--------------------------------------------------------------------------------