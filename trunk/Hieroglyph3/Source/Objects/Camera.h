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
		void RenderFrame( RendererDX11* pRenderer );

		void SetProjectionParams( float zn, float zf, float aspect, float fov );

        const Matrix4f& ProjMatrix();

	protected:
		IRenderView* m_pCameraView;
		IRenderView* m_pOverlayView;
		Scene* m_pScene;

		float m_fNear;
		float m_fFar;
		float m_fAspect;
		float m_fFov;

        Matrix4f m_ProjMatrix;
	};
};
//--------------------------------------------------------------------------------
#endif // Camera_h
