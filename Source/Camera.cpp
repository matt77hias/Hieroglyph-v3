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
#include "ViewPortDX11.h"
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
Camera::Camera() :
	m_pCameraView( nullptr ),
	m_pOverlayView( nullptr ),
	m_pScene( nullptr ),
	m_fNear( 0.1f ),
	m_fFar( 100.0f ),
	m_fAspect( 1280.0f / 800.0f ),
	m_fFov( static_cast<float>( GLYPH_PI ) / 4.0f ),
	m_fWidth( 1280.0f ),
	m_fHeight( 800.0f ),
	m_ProjMatrix(),
	m_pViewPositionWriter( nullptr ),
	m_pSpatialController( nullptr )
{
	// Create the spatial controller, which will be used to manipulate the node
	// in a simple way.

	m_pSpatialController = new SpatialController<Node3D>();
	GetNode()->Controllers.Attach( m_pSpatialController );

    m_ProjMatrix.MakeIdentity();

	m_pViewPositionWriter = Parameters.SetVectorParameter( L"ViewPosition", Vector4f( 0.0f, 0.0f, 0.0f, 0.0f ) );

	// By default, the camera body is not pickable.  This behavior can be updated
	// by the client simply by adding the picking geometry to the entity.
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
			m_pCameraView->SetScene( m_pScene );
		}


		// Set the view position in the parameter system, for use by any of the
		// views being used in this frame.

		Vector3f p = GetBody()->Transform.LocalPointToWorldSpace( Vector3f( 0.0f, 0.0f, 0.0f ) );
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
void Camera::SetOrthographicParams( float zn, float zf, float width, float height )
{
	m_fNear = zn;
	m_fFar = zf;
	m_fWidth = width;
	m_fHeight = height;

	ApplyOrthographicParams();
}
//--------------------------------------------------------------------------------
void Camera::SetOffsetProjectionParams( float l, float r, float b, float t, float zn, float zf )
{
	m_ProjMatrix[0] = 2.0f * zn / ( r - l );
	m_ProjMatrix[1] = 0.0f;
	m_ProjMatrix[2] = 0.0f;
	m_ProjMatrix[3] = 0.0f;

	m_ProjMatrix[4] = 0.0f;
	m_ProjMatrix[5] = 2.0f * zn / ( t - b );
	m_ProjMatrix[6] = 0.0f;
	m_ProjMatrix[7] = 0.0f;

	m_ProjMatrix[8] = (l+r)/(l-r);
	m_ProjMatrix[9] = (t+b)/(b-t);
	m_ProjMatrix[10] = zf/(zf-zn);
	m_ProjMatrix[11] = 1.0f;

	m_ProjMatrix[12] = 0.0f;
	m_ProjMatrix[13] = 0.0f;
	m_ProjMatrix[14] = zn*zf/(zn-zf);
	m_ProjMatrix[15] = 0.0f;

	if ( m_pCameraView )
		m_pCameraView->SetProjMatrix( m_ProjMatrix );	
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
void Camera::ApplyOrthographicParams()
{
	m_ProjMatrix = Matrix4f::OrthographicLHMatrix( m_fNear, m_fFar, m_fWidth, m_fHeight );

	if ( m_pCameraView )
		m_pCameraView->SetProjMatrix( m_ProjMatrix );	
}
//--------------------------------------------------------------------------------
const Matrix4f& Camera::ProjMatrix() const
{
    return( m_ProjMatrix );
}
//--------------------------------------------------------------------------------
SpatialController<Node3D>& Camera::Spatial()
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
    return( L"Camera" );
}
//--------------------------------------------------------------------------------
Ray3f Camera::GetWorldSpacePickRay( const Vector2f& location ) const
{
	// Given an input coordinate pair, we need to use the SceneRenderTask's 
	// render target size to be able to calculate the view space pick ray.

	ViewPortDX11 Viewport = m_pCameraView->GetViewPort( 0 );
	Vector2f normalized = Viewport.GetClipSpacePosition( location );

	// Create a point that represents the clip space location on the screen surface.

	Vector4f ScreenPoint = Vector4f( normalized.x, normalized.y, 0.0f, 1.0f );

	// Calculate the view matrix inverse, and the view*proj inverse.  These are 
	// used to calculate the corresponding world space points that we need to
	// build the ray.

	Matrix4f View = GetBody()->Transform.GetView();
	Matrix4f ViewProj = View * ProjMatrix();
	Matrix4f ViewInverse = View.Inverse();
	Matrix4f ViewProjInverse = ViewProj.Inverse();
	
	// The world space screen point takes the clip space position on the near
	// clip plane and back projects it to world space by multiplying by the 
	// view projection inverse.  The eye point in world space is found by taking
	// the origin in view space (which is, by default, the location of the camera)
	// and multiplying by the inverse of the view matrix.

	Vector4f ScreenPointWS = ViewProjInverse * ScreenPoint;
	ScreenPointWS = ScreenPointWS / ScreenPointWS.w;

	Vector4f EyePointWS = ViewInverse * Vector4f( 0.0f, 0.0f, 0.0f, 1.0f );

	// Construct the ray as having direction from the world space eye point to the
	// world space location on the near clipping plane that we are pointing to. 
	// The origin of the ray is the world space camera location.

	Vector3f direction = Vector3f::Normalize( ScreenPointWS.xyz() - EyePointWS.xyz() );
	Vector3f origin = EyePointWS.xyz();

	return( Ray3f( origin, direction ) );
}
//--------------------------------------------------------------------------------
Vector2f Camera::WorldToScreenSpace( const Vector3f& point )
{
	// Get the view and projection matrices from the SceneRenderTask, and then 
	// calculate the world to clip space matrix (i.e. view*proj).

	Matrix4f view = this->GetCameraView()->GetViewMatrix();
    Matrix4f proj = this->GetCameraView()->GetProjMatrix();

    Matrix4f viewProj = view * proj;

	// Transform from world to clip space, and then normalize the w component.
	// This produces the point in the < [-1,1], [-1,1], [0,1], 1 > ranges.

    Vector4f modelViewProjPos = viewProj * Vector4f(point.x, point.y, point.z, 1);
	modelViewProjPos = modelViewProjPos / modelViewProjPos.w;

	// Map the clips space to screen space using the viewport.

	ViewPortDX11 Viewport = m_pCameraView->GetViewPort( 0 );
	Vector2f screen = Viewport.GetScreenSpacePosition( modelViewProjPos.xy() );

	return( screen );
}
//--------------------------------------------------------------------------------
Vector3f Camera::ScreenToWorldSpace( const Vector2f& cursor )
{
	// This method returns the world space position of the given screen space
	// coordinates on the near clipping plane.  That means this is the projection
	// of the screen space point onto the near clipping plane!

	// Given an input coordinate pair, we need to use the SceneRenderTask's 
	// render target size to be able to calculate the world space location.

	ViewPortDX11 Viewport = m_pCameraView->GetViewPort( 0 );
	Vector2f normalized = Viewport.GetClipSpacePosition( cursor );

	// Create a point that represents the clip space location on the screen surface.

	Vector4f ScreenPoint = Vector4f( normalized.x, normalized.y, 0.0f, 1.0f );

	// Calculate the view*proj inverse.  These are used to calculate the 
	// corresponding world space points to the screen coordinate.

	Matrix4f View = GetBody()->Transform.GetView();
	Matrix4f ViewProj = View * ProjMatrix();
	Matrix4f ViewProjInverse = ViewProj.Inverse();
	
	// The world space screen point takes the clip space position on the near
	// clip plane and back projects it to world space by multiplying by the 
	// view projection inverse.

	Vector4f ScreenPointWS = ViewProjInverse * ScreenPoint;
	ScreenPointWS = ScreenPointWS / ScreenPointWS.w;

	return( ScreenPointWS.xyz() );
}
//--------------------------------------------------------------------------------