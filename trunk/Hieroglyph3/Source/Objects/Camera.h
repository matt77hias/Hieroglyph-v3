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
#include "IRenderView.h"
#include "SpatialController.h"
#include "ParameterContainer.h"
//--------------------------------------------------------------------------------
namespace Glyph3
{
	class Scene;
	class SpatialController;

	class Camera : public Actor
	{
	public:
		Camera();
		virtual ~Camera();

		void SetCameraView( IRenderView* pView );
		void SetOverlayView( IRenderView* pView );
		void SetScene( Scene* pScene );

		IRenderView* GetCameraView();
		IRenderView* GetOverlayView();
		Scene* GetScene();

		void RenderFrame( RendererDX11* pRenderer );

		void SetProjectionParams( float zn, float zf, float aspect, float fov );
		void SetClipPlanes( float zn, float zf );
		void SetAspectRatio( float aspect );
		void SetFieldOfView( float fov );

		float GetNearClipPlane();
		float GetFarClipPlane();
		float GetAspectRatio();
		float GetFieldOfView();

		SpatialController& Spatial();

        const Matrix4f& ProjMatrix();

	public:
		ParameterContainer Parameters;

	protected:

		void ApplyProjectionParams();

		IRenderView* m_pCameraView;
		IRenderView* m_pOverlayView;
		Scene* m_pScene;

		float m_fNear;
		float m_fFar;
		float m_fAspect;
		float m_fFov;

        Matrix4f m_ProjMatrix;
		VectorParameterWriterDX11* m_pViewPositionWriter;
		SpatialController*	m_pSpatialController;
	};
};
//--------------------------------------------------------------------------------
#endif // Camera_h
