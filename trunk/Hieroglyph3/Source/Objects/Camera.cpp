//--------------------------------------------------------------------------------
// This file is a portion of the Hieroglyph 3 Rendering Engine.  It is distributed
// under the MIT License, available in the root of this distribution and 
// at the following URL:
//
// http://www.opensource.org/licenses/mit-license.php
//
// Copyright (c) Jason Zink 
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
	m_pCameraView = nullptr;
	m_pOverlayView = nullptr;
	m_pScene = nullptr;

	// Create the spatial controller, which will be used to manipulate the node
	// in a simple way.
	m_pSpatialController = new SpatialController();
	GetNode()->AttachController( m_pSpatialController );

	m_fNear = 0.1f;
	m_fFar = 100.0f;
	m_fAspect = 1280.0f / 800.0f;
	m_fFov = static_cast<float>( GLYPH_PI ) / 4.0f;

    m_ProjMatrix.MakeIdentity();

	m_pViewPositionWriter = Parameters.SetVectorParameter( L"ViewPosition", Vector4f( 0.0f, 0.0f, 0.0f, 0.0f ) );
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
		pRenderer->QueueTask( m_pOverlayView );

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

		Vector3f p = GetNode()->Position() + GetBody()->Position();
		m_pViewPositionWriter->SetValue( Vector4f( p.x, p.y, p.z, 1.0f ) );

		Parameters.InitRenderParams();

		// Use the pre-draw method to queue any needed render views in the scene.
		// This is followed by rendering all of the views to generate the current
		// frame.

		m_pCameraView->QueuePreTasks( pRenderer );
		pRenderer->ProcessTaskQueue();
	}
}
//--------------------------------------------------------------------------------
void Camera::SetCameraView( SceneRenderTask* pTask )
{
	m_pCameraView = pTask;
	m_pCameraView->SetEntity( m_pBody );
}
//--------------------------------------------------------------------------------
void Camera::SetOverlayView( Task* pTask )
{
	m_pOverlayView = pTask;
}
//--------------------------------------------------------------------------------
void Camera::SetScene( Scene* pScene )
{
	m_pScene = pScene;
}
//--------------------------------------------------------------------------------
SceneRenderTask* Camera::GetCameraView()
{
	return( m_pCameraView );
}
//--------------------------------------------------------------------------------
Task* Camera::GetOverlayView()
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
    return( m_ProjMatrix );
}
//--------------------------------------------------------------------------------
SpatialController& Camera::Spatial()
{
	return( *m_pSpatialController );
}
//--------------------------------------------------------------------------------
bool Camera::HandleEvent( EventPtr pEvent )
{
    eEVENT e = pEvent->GetEventType();
	
    return false;
}
//--------------------------------------------------------------------------------
std::wstring Camera::GetName()
{
    return L"Camera";
}
//--------------------------------------------------------------------------------