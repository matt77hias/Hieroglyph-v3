//--------------------------------------------------------------------------------
// This file is a portion of the Hieroglyph 3 Rendering Engine.  It is distributed
// under the MIT License, available in the root of this distribution and
// at the following URL:
//
// http://www.opensource.org/licenses/mit-license.php
//
// Copyright (c) Jason Zink
//--------------------------------------------------------------------------------
#include "PCH.h"

#include "Application.h"

#include "Win32RenderWindow.h"
#include "RendererDX11.h"

#include "SceneRenderTask.h"
#include "ViewTextOverlay.h"
#include "Camera.h"
#include "Scene.h"
#include "ConsoleActor.h"

namespace Glyph3
{
	class RenderApplication : public Application
	{

	public:
		RenderApplication();
		virtual ~RenderApplication();
	
	public:
		virtual bool ConfigureRenderingEngineComponents( UINT width, UINT height, D3D_FEATURE_LEVEL desiredLevel, D3D_DRIVER_TYPE driverType = D3D_DRIVER_TYPE_HARDWARE );
		virtual bool ConfigureRenderingSetup();

		virtual void ShutdownRenderingEngineComponents();
		virtual void ShutdownRenderingSetup();

		virtual void HandleWindowResize( HWND handle, UINT width, UINT height );
		virtual bool HandleEvent( EventPtr pEvent );

		virtual Win32RenderWindow* CreateRenderWindow();

		void ToggleMultiThreadedMode();
		void SetMultiThreadedMode( bool mode );
		bool GetMultiThreadedMode();

		virtual void TakeScreenShot();
		virtual void SetScreenShotName( const std::wstring name );

	protected:

		RendererDX11*			m_pRenderer11;
		Win32RenderWindow*		m_pWindow;

		UINT					m_iWidth;
		UINT					m_iHeight;

		ResourcePtr				m_BackBuffer;

		SceneRenderTask*		m_pRenderView;
		ViewTextOverlay*		m_pTextOverlayView;

		bool					m_bMultithreadedMode;

		std::wstring			m_ScreenShotName;

	public:
		
		enum class InputMode
		{
			Console,
			Camera
		};

		// For the camera, we have a reference to the object itself, and
		// also provide another event manager for it to use.  This allows
		// us to control when the camera receives events and when they 
		// should be passed to a different object.

		EventManager			CameraEventHub;
		Camera*					m_pCamera;

		EventManager			ConsoleEventHub;
		ConsoleActor*			m_pConsole;

		InputMode				m_InputMode;
	};
};