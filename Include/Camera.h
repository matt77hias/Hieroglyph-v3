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
// Camera
//
// This class specializes an actor, and provides an interface for easily setting 
// particular camera based parameters like the aspect ration and field-of-view.  
// In addition, it allows the specification of a 'render-view' which it will
// execute with the camera parameters that the user has specified.
//        
// In addition, a second render view slot is provided to allow the insertion of 
// an overlay visualization.  If one is set, it will automatically be rendered
// after the main view has been.
//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
#ifndef Camera_h
#define Camera_h
//--------------------------------------------------------------------------------
#include "Actor.h"
#include "SceneRenderTask.h"
#include "SpatialController.h"
#include "ParameterContainer.h"
#include "IEventListener.h"
//--------------------------------------------------------------------------------
namespace Glyph3
{
	class Scene;

	class Camera : public Actor, public IEventListener
	{
	public:
		Camera();
		virtual ~Camera();

		void SetCameraView( SceneRenderTask* pTask );
		void SetOverlayView( Task* pTask );
		void SetScene( Scene* pScene );

		SceneRenderTask* GetCameraView();
		Task* GetOverlayView();
		Scene* GetScene();

		void RenderFrame( RendererDX11* pRenderer );

		void SetProjectionParams( float zn, float zf, float aspect, float fov );
		void SetOrthographicParams( float zn, float zf, float width, float height );
		void SetOffsetProjectionParams( float l, float r, float b, float t, float zn, float zf );

		void SetClipPlanes( float zn, float zf );
		void SetAspectRatio( float aspect );
		void SetFieldOfView( float fov );

		float GetNearClipPlane();
		float GetFarClipPlane();
		float GetAspectRatio();
		float GetFieldOfView();

		SpatialController<Node3D>& Spatial();
		const Matrix4f& ProjMatrix() const;

		Ray3f GetWorldSpacePickRay( const Vector2f& location ) const;
		Vector2f WorldToScreenSpace( const Vector3f& point );
		Vector3f ScreenToWorldSpace( const Vector2f& cursor );

        virtual bool HandleEvent( EventPtr pEvent );
        virtual std::wstring GetName();

		ParameterContainer Parameters;

	protected:

		void ApplyProjectionParams();
		void ApplyOrthographicParams();

		SceneRenderTask* m_pCameraView;
		Task* m_pOverlayView;
		Scene* m_pScene;

		float m_fNear;
		float m_fFar;
		float m_fAspect;
		float m_fFov;

		float m_fWidth;
		float m_fHeight;

        Matrix4f m_ProjMatrix;
		VectorParameterWriterDX11* m_pViewPositionWriter;
		SpatialController<Node3D>*	m_pSpatialController;
	};
};
//--------------------------------------------------------------------------------
#endif // Camera_h
//--------------------------------------------------------------------------------