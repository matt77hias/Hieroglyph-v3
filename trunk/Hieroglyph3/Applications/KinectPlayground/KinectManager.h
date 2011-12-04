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
// KinectManager
//
//--------------------------------------------------------------------------------
#ifndef KinectManager_h
#define KinectManager_h
//--------------------------------------------------------------------------------
#include <Windows.h>
#include "PCH.h"
//--------------------------------------------------------------------------------
namespace Glyph3
{
	class KinectManager
	{
	public:
		KinectManager();
		~KinectManager();

		bool Initialize();
		void Shutdown();

		void Nui_GotDepthAlert( );
		void Nui_GotVideoAlert( );
		void Nui_GotSkeletonAlert( );

		void SetSysMemColorBuffer( BYTE* pBuffer );
		void SetSysMemDepthBuffer( BYTE* pBuffer );
		BYTE* GetSysMemColorBuffer( );
		BYTE* GetSysMemDepthBuffer( );

	private:
		static DWORD WINAPI     Nui_ProcessThread(LPVOID pParam);

	    HANDLE        m_hThNuiProcess;
	    HANDLE        m_hEvNuiProcessStop;
	    HANDLE        m_hNextDepthFrameEvent;
		HANDLE        m_hNextVideoFrameEvent;
		HANDLE        m_hNextSkeletonEvent;
		HANDLE        m_pDepthStreamHandle;
		HANDLE        m_pVideoStreamHandle;

		BYTE*		m_pSysMemColorBuffer;
		BYTE*		m_pSysMemDepthBuffer;
	};
}
//--------------------------------------------------------------------------------
#endif // KinectManager_h