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
// KinectManager
//
// This class provides a simple abstraction layer between the Kinect API and the
// Hieroglyph 3 classes that use it.  This effectively contains any changes to the 
// API within this class.
//
// This code was based on the skeletal viewer sample from the Kinect for Windows
// Beta 1 SDK.
//--------------------------------------------------------------------------------
#ifndef KinectManager_h
#define KinectManager_h
//--------------------------------------------------------------------------------
#include <Windows.h>
#include "Timer.h"
//--------------------------------------------------------------------------------
namespace Glyph3
{
	class KinectManager
	{
	public:
		KinectManager();
		~KinectManager();

		bool Initialize( bool enable_skeleton = true, bool enable_color = true, bool enable_depth = true );
		void Shutdown();

		void Nui_GotDepthAlert( );
		void Nui_GotVideoAlert( );
		void Nui_GotSkeletonAlert( );

		void SetSysMemColorBuffer( BYTE* pBuffer );
		void SetSysMemDepthBuffer( BYTE* pBuffer );
		void SetSysMemSkeletonBuffer( BYTE* pBuffer );

		BYTE* GetSysMemColorBuffer( );
		BYTE* GetSysMemDepthBuffer( );
		BYTE* GetSysMemSkeletonBuffer( );

	private:
		static DWORD WINAPI     Nui_ProcessThread(LPVOID pParam);

	    HANDLE      m_hThNuiProcess;
	    HANDLE      m_hEvNuiProcessStop;
	    
		HANDLE      m_hNextDepthFrameEvent;
		HANDLE      m_hNextVideoFrameEvent;
		HANDLE      m_hNextSkeletonEvent;

		HANDLE      m_pDepthStreamHandle;
		HANDLE      m_pVideoStreamHandle;

		BYTE*		m_pSysMemColorBuffer;
		BYTE*		m_pSysMemDepthBuffer;
		BYTE*		m_pSysMemSkeletonBuffer;

		Timer		m_DepthFrameTimer;
		Timer		m_ColorFrameTimer;
		Timer		m_SkeletonFrameTimer;
	};
}
//--------------------------------------------------------------------------------
#endif // KinectManager_h