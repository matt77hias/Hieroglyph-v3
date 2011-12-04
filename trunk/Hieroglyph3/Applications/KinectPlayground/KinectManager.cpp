//--------------------------------------------------------------------------------
// This file is a portion of the Hieroglyph 3 Rendering Engine.  It is distributed
// under the MIT License, available in the root of this distribution and 
// at the following URL:
//
// http://www.opensource.org/licenses/mit-license.php
//
// Copyright (c) 2003-2010 Jason Zink 
//--------------------------------------------------------------------------------
#include "KinectManager.h"
#include "MSR_NuiApi.h"
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
KinectManager::KinectManager(void)
{
    m_hNextDepthFrameEvent = NULL;
    m_hNextVideoFrameEvent = NULL;
    m_hNextSkeletonEvent = NULL;
    m_pDepthStreamHandle = NULL;
    m_pVideoStreamHandle = NULL;
    m_hThNuiProcess = NULL;
    m_hEvNuiProcessStop = NULL;

	m_pSysMemColorBuffer = NULL;
	m_pSysMemDepthBuffer = NULL;
}
//--------------------------------------------------------------------------------
KinectManager::~KinectManager(void)
{
}
//--------------------------------------------------------------------------------
bool KinectManager::Initialize()
{
	HRESULT hr;

    m_hNextDepthFrameEvent = CreateEvent( NULL, TRUE, FALSE, NULL );
    m_hNextVideoFrameEvent = CreateEvent( NULL, TRUE, FALSE, NULL );
    m_hNextSkeletonEvent = CreateEvent( NULL, TRUE, FALSE, NULL );

	hr = NuiInitialize( 
        NUI_INITIALIZE_FLAG_USES_DEPTH_AND_PLAYER_INDEX | 
		NUI_INITIALIZE_FLAG_USES_SKELETON | 
		NUI_INITIALIZE_FLAG_USES_COLOR );
    if( FAILED( hr ) )
    {
        MessageBox( 0, L"Failed to initialize NUI library...", L"Hieroglyph 3 :: Info Message", MB_ICONINFORMATION | MB_SYSTEMMODAL );
        return false;
    }

    hr = NuiImageStreamOpen(
        NUI_IMAGE_TYPE_COLOR,
        NUI_IMAGE_RESOLUTION_640x480,
        0,
        2,
        m_hNextVideoFrameEvent,
        &m_pVideoStreamHandle );
    if( FAILED( hr ) )
    {
        MessageBox( 0, L"Failed to open color image stream...", L"Hieroglyph 3 :: Info Message", MB_ICONINFORMATION | MB_SYSTEMMODAL );
        return false;
    }

    hr = NuiImageStreamOpen(
        NUI_IMAGE_TYPE_DEPTH_AND_PLAYER_INDEX,
        NUI_IMAGE_RESOLUTION_320x240,
        0,
        2,
        m_hNextDepthFrameEvent,
        &m_pDepthStreamHandle );
    if( FAILED( false ) )
    {
        MessageBox( 0, L"Failed to open depth image stream...", L"Hieroglyph 3 :: Info Message", MB_ICONINFORMATION | MB_SYSTEMMODAL );
        return false;
    }

    // Start the Nui processing thread
    m_hEvNuiProcessStop=CreateEvent(NULL,FALSE,FALSE,NULL);
    m_hThNuiProcess=CreateThread(NULL,0,Nui_ProcessThread,this,0,NULL);

	return true;
}
//--------------------------------------------------------------------------------
void KinectManager::Shutdown()
{
    // Stop the Nui processing thread
    if(m_hEvNuiProcessStop!=NULL)
    {
        // Signal the thread
        SetEvent(m_hEvNuiProcessStop);

        // Wait for thread to stop
        if(m_hThNuiProcess!=NULL)
        {
            WaitForSingleObject(m_hThNuiProcess,INFINITE);
            CloseHandle(m_hThNuiProcess);
        }
        CloseHandle(m_hEvNuiProcessStop);
    }

    NuiShutdown( );

    if( m_hNextSkeletonEvent && ( m_hNextSkeletonEvent != INVALID_HANDLE_VALUE ) )
    {
        CloseHandle( m_hNextSkeletonEvent );
        m_hNextSkeletonEvent = NULL;
    }
    if( m_hNextDepthFrameEvent && ( m_hNextDepthFrameEvent != INVALID_HANDLE_VALUE ) )
    {
        CloseHandle( m_hNextDepthFrameEvent );
        m_hNextDepthFrameEvent = NULL;
    }
    if( m_hNextVideoFrameEvent && ( m_hNextVideoFrameEvent != INVALID_HANDLE_VALUE ) )
    {
        CloseHandle( m_hNextVideoFrameEvent );
        m_hNextVideoFrameEvent = NULL;
    }
}
//--------------------------------------------------------------------------------



DWORD WINAPI KinectManager::Nui_ProcessThread(LPVOID pParam)
{
    KinectManager *pthis = (KinectManager*) pParam;
    HANDLE hEvents[4];
    int nEventIdx; //,t,dt;

    // Configure events to be listened on
    hEvents[0]=pthis->m_hEvNuiProcessStop;
    hEvents[1]=pthis->m_hNextDepthFrameEvent;
    hEvents[2]=pthis->m_hNextVideoFrameEvent;
    hEvents[3]=pthis->m_hNextSkeletonEvent;

    // Main thread loop
    while(1)
    {
        // Wait for an event to be signalled
        nEventIdx=WaitForMultipleObjects(sizeof(hEvents)/sizeof(hEvents[0]),hEvents,FALSE,100);

        // If the stop event, stop looping and exit
        if(nEventIdx==0)
            break;            

        // Perform FPS processing
/*        t = timeGetTime( );
        if( pthis->m_LastFPStime == -1 )
        {
            pthis->m_LastFPStime = t;
            pthis->m_LastFramesTotal = pthis->m_FramesTotal;
        }
        dt = t - pthis->m_LastFPStime;
        if( dt > 1000 )
        {
            pthis->m_LastFPStime = t;
            int FrameDelta = pthis->m_FramesTotal - pthis->m_LastFramesTotal;
            pthis->m_LastFramesTotal = pthis->m_FramesTotal;
            SetDlgItemInt( pthis->m_hWnd, IDC_FPS, FrameDelta,FALSE );
        }

        // Perform skeletal panel blanking
        if( pthis->m_LastSkeletonFoundTime == -1 )
            pthis->m_LastSkeletonFoundTime = t;
        dt = t - pthis->m_LastSkeletonFoundTime;
        if( dt > 250 )
        {
            if( !pthis->m_bScreenBlanked )
            {
                pthis->Nui_BlankSkeletonScreen( GetDlgItem( pthis->m_hWnd, IDC_SKELETALVIEW ) );
                pthis->m_bScreenBlanked = true;
            }
        }
		*/

        // Process signal events
        switch(nEventIdx)
        {
            case 1:
                pthis->Nui_GotDepthAlert();
                //pthis->m_FramesTotal++;
                break;

            case 2:
                pthis->Nui_GotVideoAlert();
                break;

            case 3:
                pthis->Nui_GotSkeletonAlert( );
                break;
        }
    }

    return (0);
}


void KinectManager::Nui_GotVideoAlert( )
{
    const NUI_IMAGE_FRAME * pImageFrame = NULL;

    HRESULT hr = NuiImageStreamGetNextFrame(
        m_pVideoStreamHandle,
        0,
        &pImageFrame );
    if( FAILED( hr ) )
    {
        return;
    }

    INuiFrameTexture * pTexture = pImageFrame->pFrameTexture;
    NUI_LOCKED_RECT LockedRect;
    pTexture->LockRect( 0, &LockedRect, NULL, 0 );
    if( LockedRect.Pitch != 0 )
    {
		// If there is a buffer to fill, then fill it and forget it...
		if ( m_pSysMemColorBuffer != NULL ) {
		
			BYTE * pBuffer = (BYTE*) LockedRect.pBits;

			//memcpy( m_pSysMemColorBuffer, pBuffer, sizeof(BYTE)*640*480*3 );

			for( int y = 0 ; y < 480 ; y++ )
			{
				for( int x = 0 ; x < 640; x++ )
				{
					m_pSysMemColorBuffer[(x+640*y)*4+0] = pBuffer[(x+640*y)*4+2]; // R
					m_pSysMemColorBuffer[(x+640*y)*4+1] = pBuffer[(x+640*y)*4+1]; // G
					m_pSysMemColorBuffer[(x+640*y)*4+2] = pBuffer[(x+640*y)*4+0]; // B
					m_pSysMemColorBuffer[(x+640*y)*4+3] = pBuffer[(x+640*y)*4+3]; // A
				}
			}

			//OutputDebugString( L"Color image processed...\r\n" );

			m_pSysMemColorBuffer = NULL;
		}
    }
    else
    {
        OutputDebugString( L"Buffer length of received texture is bogus\r\n" );
    }

    NuiImageStreamReleaseFrame( m_pVideoStreamHandle, pImageFrame );
}


void KinectManager::Nui_GotDepthAlert( )
{
    const NUI_IMAGE_FRAME * pImageFrame = NULL;

    HRESULT hr = NuiImageStreamGetNextFrame(
        m_pDepthStreamHandle,
        0,
        &pImageFrame );

    if( FAILED( hr ) )
    {
        return;
    }

    INuiFrameTexture * pTexture = pImageFrame->pFrameTexture;
    NUI_LOCKED_RECT LockedRect;
    pTexture->LockRect( 0, &LockedRect, NULL, 0 );
    if( LockedRect.Pitch != 0 )
    {
        BYTE * pBuffer = (BYTE*) LockedRect.pBits;

        // draw the bits to the bitmap
//        RGBQUAD * rgbrun = m_rgbWk;
		if ( m_pSysMemDepthBuffer != NULL ) {
			USHORT * pBufferRun = (USHORT*) pBuffer;
			for( int y = 0 ; y < 240 ; y++ )
			{
				for( int x = 0 ; x < 320 ; x++ )
				{
					//USHORT s = pBuffer[(x+320*y)*2];
					USHORT s = *pBufferRun;
					USHORT RealDepth = (s & 0xfff8) >> 3;
					USHORT Player = s & 7;

					// transform 13-bit depth information into an 8-bit intensity appropriate
					// for display (we disregard information in most significant bit)
					BYTE l = 255 - (BYTE)(256*RealDepth/0x0fff);

					pBufferRun++;
					m_pSysMemDepthBuffer[(x+320*y)] = l;
					m_pSysMemDepthBuffer[(x+320*y)] = (s&0xff00) >> 8;
				}
			}
			
			m_pSysMemDepthBuffer = NULL;
			//OutputDebugString( L"Depth image processed...\r\n" );
		}
    }
    else
    {
        OutputDebugString( L"Buffer length of received texture is bogus\r\n" );
    }

    NuiImageStreamReleaseFrame( m_pDepthStreamHandle, pImageFrame );
}


void KinectManager::Nui_GotSkeletonAlert( )
{
    NUI_SKELETON_FRAME SkeletonFrame;

    HRESULT hr = NuiSkeletonGetNextFrame( 0, &SkeletonFrame );

    bool bFoundSkeleton = true;
    for( int i = 0 ; i < NUI_SKELETON_COUNT ; i++ )
    {
        if( SkeletonFrame.SkeletonData[i].eTrackingState == NUI_SKELETON_TRACKED )
        {
            bFoundSkeleton = false;
        }
    }

    // no skeletons!
    //
    if( bFoundSkeleton )
    {
        return;
    }

    // smooth out the skeleton data
    NuiTransformSmooth(&SkeletonFrame,NULL);

	/*
    // we found a skeleton, re-start the timer
    m_bScreenBlanked = false;
    m_LastSkeletonFoundTime = -1;

    // draw each skeleton color according to the slot within they are found.
    //
    bool bBlank = true;
    for( int i = 0 ; i < NUI_SKELETON_COUNT ; i++ )
    {
        if( SkeletonFrame.SkeletonData[i].eTrackingState == NUI_SKELETON_TRACKED )
        {
            Nui_DrawSkeleton( bBlank, &SkeletonFrame.SkeletonData[i], GetDlgItem( m_hWnd, IDC_SKELETALVIEW ), i );
            bBlank = false;
        }
    }

    Nui_DoDoubleBuffer(GetDlgItem(m_hWnd,IDC_SKELETALVIEW), m_SkeletonDC);
	*/
}


void KinectManager::SetSysMemColorBuffer( BYTE* pBuffer )
{
	m_pSysMemColorBuffer = pBuffer;	
}


void KinectManager::SetSysMemDepthBuffer( BYTE* pBuffer )
{
	m_pSysMemDepthBuffer = pBuffer;	
}


BYTE* KinectManager::GetSysMemColorBuffer( )
{
	return( m_pSysMemColorBuffer );	
}


BYTE* KinectManager::GetSysMemDepthBuffer( )
{
	return( m_pSysMemDepthBuffer );	
}
