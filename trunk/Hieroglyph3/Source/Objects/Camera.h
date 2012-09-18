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
//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
#ifndef Camera_h
#define Camera_h
//--------------------------------------------------------------------------------
#include "Actor.h"
#include "IRenderView.h"
//--------------------------------------------------------------------------------
namespace Glyph3
{
	class Scene;

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

        const Matrix4f& ProjMatrix();

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
		VectorParameterDX11* m_pViewPositionParameter;
	};
};
//--------------------------------------------------------------------------------
#endif // Camera_h
