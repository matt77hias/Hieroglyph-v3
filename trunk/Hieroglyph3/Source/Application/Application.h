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
// Application
//
// This class represents the base application available to the end user.  The 
// Windows Main function is contained withing the .cpp file, and automatically
// checks for an instance of a CApplication class.  If one is not found then the
// program is exited.
//
// The application currently supports Input, Sound, Rendering, Logging, Timing, 
// and profiling.  These are all available to the user when building an 
// application.
//--------------------------------------------------------------------------------
#ifndef Application_h
#define Application_h
//--------------------------------------------------------------------------------
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
//--------------------------------------------------------------------------------
#include <windows.h>
#include <stdio.h>

#include "Log.h"
#include "Timer.h"
#include "EventManager.h"
//--------------------------------------------------------------------------------
namespace Glyph3
{
	class Application
	{
	public:
		Application();
		Application( int iWidth, int iHeight, bool bWindowed );
		virtual ~Application();

		// Initialization functions
		static Application* GetApplication( );

		void SetWindowID( HWND WindowID );
		void SetApplicationID( HINSTANCE ApplicationID );

		// Overloadable functions for end user
		virtual bool ConfigureEngineComponents() = 0;
		virtual void ShutdownEngineComponents() = 0;
		virtual void Initialize();
		virtual void Update();
		virtual void Shutdown();

		// Request an exit from windows
		void RequestTermination();

		int DisplayWidth();
		int DisplayHeight();
		bool DisplayWindowed();

		// Helpers
		Timer* m_pTimer;

		// Engine Components
		EventManager* m_pEventMgr;

	protected:
		// CApplication pointer to ensure single instance
		static Application* ms_pApplication;
		
		// Windows data structures
		HWND ms_WindowID;
		HINSTANCE m_AppInstance;
		
		// Display parameters
		int m_iWidth;
		int m_iHeight;
		bool m_bWindowed;
	};
};
//--------------------------------------------------------------------------------
#endif // Application_h