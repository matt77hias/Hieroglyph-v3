//--------------------------------------------------------------------------------
// This file is a portion of the Hieroglyph 3 Rendering Engine.  It is distributed
// under the MIT License, available in the root of this distribution and 
// at the following URL:
//
// http://www.opensource.org/licenses/mit-license.php
//
// Copyright (c) Jason Zink 
//--------------------------------------------------------------------------------
#include "GlyphKinect/KinectManager.h"
#include "NuiApi.h"
#include "Log.h"
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
KinectManager::KinectManager()
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
	m_pSysMemSkeletonBuffer = NULL;
}
//--------------------------------------------------------------------------------
KinectManager::~KinectManager(void)
{
	std::wstringstream out;
	out << L"Color Framerate: current - " << m_ColorFrameTimer.Framerate() << L", max - " << m_ColorFrameTimer.MaxFramerate() << std::endl;
	out << L"Depth Framerate: current - " << m_DepthFrameTimer.Framerate() << L", max - " << m_DepthFrameTimer.MaxFramerate() << std::endl;
	out << L"Color Framerate: current - " << m_SkeletonFrameTimer.Framerate() << L", max - " << m_SkeletonFrameTimer.MaxFramerate() << std::endl;
	Log::Get().Write( out.str() );
}
//--------------------------------------------------------------------------------
bool KinectManager::Initialize( bool enable_skeleton, bool enable_color, bool enable_depth )
{
	HRESULT hr;

    m_hNextDepthFrameEvent = CreateEvent( NULL, TRUE, FALSE, NULL );
    m_hNextVideoFrameEvent = CreateEvent( NULL, TRUE, FALSE, NULL );
    m_hNextSkeletonEvent = CreateEvent( NULL, TRUE, FALSE, NULL );

	DWORD flags = 0;

	if ( enable_skeleton ) flags |= NUI_INITIALIZE_FLAG_USES_SKELETON;
	if ( enable_color ) flags |= NUI_INITIALIZE_FLAG_USES_COLOR;
	if ( enable_depth ) flags |= NUI_INITIALIZE_FLAG_USES_DEPTH_AND_PLAYER_INDEX;

	hr = NuiInitialize( flags );

	if ( FAILED( hr ) ) {
        MessageBox( 0, L"Failed to initialize NUI library...", L"Hieroglyph 3 :: Info Message", MB_ICONINFORMATION | MB_SYSTEMMODAL );
        return false;
    }

	if ( enable_skeleton )
	{
		hr = NuiSkeletonTrackingEnable( m_hNextSkeletonEvent, 0 );

		if ( FAILED( hr ) ) {
			MessageBox( 0, L"Failed to open skeletal stream...", L"Hieroglyph 3 :: Info Message", MB_ICONINFORMATION | MB_SYSTEMMODAL );
			return false;
		}
	}

	if ( enable_color )
	{
		hr = NuiImageStreamOpen(
			NUI_IMAGE_TYPE_COLOR,
			NUI_IMAGE_RESOLUTION_640x480,
			0,
			2,
			m_hNextVideoFrameEvent,
			&m_pVideoStreamHandle );
    
		if ( FAILED( hr ) ) {
			MessageBox( 0, L"Failed to open color image stream...", L"Hieroglyph 3 :: Info Message", MB_ICONINFORMATION | MB_SYSTEMMODAL );
			return false;
		}
	}

	if ( enable_depth )
	{
		hr = NuiImageStreamOpen(
			NUI_IMAGE_TYPE_DEPTH_AND_PLAYER_INDEX,
			NUI_IMAGE_RESOLUTION_320x240,
			0,
			2,
			m_hNextDepthFrameEvent,
			&m_pDepthStreamHandle );

		if ( FAILED( false ) ) {
			MessageBox( 0, L"Failed to open depth image stream...", L"Hieroglyph 3 :: Info Message", MB_ICONINFORMATION | MB_SYSTEMMODAL );
			return false;
		}
	}

    // Start the Nui processing thread
    m_hEvNuiProcessStop=CreateEvent(NULL,FALSE,FALSE,NULL);
    m_hThNuiProcess=CreateThread(NULL,0,Nui_ProcessThread,this,0,NULL);


	m_DepthFrameTimer.Update();
	m_ColorFrameTimer.Update();
	m_SkeletonFrameTimer.Update();

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
//--------------------------------------------------------------------------------
void KinectManager::Nui_GotVideoAlert( )
{
    const NUI_IMAGE_FRAME * pImageFrame = NULL;

    HRESULT hr = NuiImageStreamGetNextFrame(
        m_pVideoStreamHandle,
        0,
        &pImageFrame );

    if( FAILED( hr ) ) {
        return;
    }

    INuiFrameTexture * pTexture = pImageFrame->pFrameTexture;
    NUI_LOCKED_RECT LockedRect;
    pTexture->LockRect( 0, &LockedRect, NULL, 0 );
    
	if( LockedRect.Pitch != 0 ) {

		// If there is a buffer to fill, then fill it and forget it...
		if ( m_pSysMemColorBuffer != NULL ) {
		
			BYTE * pBuffer = (BYTE*) LockedRect.pBits;

			for( int y = 0 ; y < 480 ; y++ ) {
				for( int x = 0 ; x < 640; x++ ) {
					m_pSysMemColorBuffer[(x+640*y)*4+0] = pBuffer[(x+640*y)*4+2]; // R
					m_pSysMemColorBuffer[(x+640*y)*4+1] = pBuffer[(x+640*y)*4+1]; // G
					m_pSysMemColorBuffer[(x+640*y)*4+2] = pBuffer[(x+640*y)*4+0]; // B
					m_pSysMemColorBuffer[(x+640*y)*4+3] = pBuffer[(x+640*y)*4+3]; // A
				}
			}

			//OutputDebugString( L"Color image processed...\r\n" );

			m_ColorFrameTimer.Update();
			m_pSysMemColorBuffer = NULL;
		}
    } else {
        OutputDebugString( L"Buffer length of received texture is bogus\r\n" );
    }

    NuiImageStreamReleaseFrame( m_pVideoStreamHandle, pImageFrame );
}
//--------------------------------------------------------------------------------
void KinectManager::Nui_GotDepthAlert( )
{
    const NUI_IMAGE_FRAME * pImageFrame = NULL;

    HRESULT hr = NuiImageStreamGetNextFrame(
        m_pDepthStreamHandle,
        0,
        &pImageFrame );

    if( FAILED( hr ) ) {
        return;
    }

    INuiFrameTexture * pTexture = pImageFrame->pFrameTexture;
    NUI_LOCKED_RECT LockedRect;
    pTexture->LockRect( 0, &LockedRect, NULL, 0 );
    
	if( LockedRect.Pitch != 0 ) {

        BYTE * pBuffer = (BYTE*) LockedRect.pBits;


		if ( m_pSysMemDepthBuffer != NULL ) {

			USHORT * pBufferRun = (USHORT*) pBuffer;

			for( int y = 0 ; y < 240 ; y++ ) {
				for( int x = 0 ; x < 320 ; x++ ) {

					USHORT s = *pBufferRun;
					USHORT RealDepth = (s & 0xfff8) >> 3;
					USHORT Player = s & 7;

					pBufferRun++;
					USHORT* pDestBuff = (USHORT*)(&(m_pSysMemDepthBuffer[(x+320*y)*2]));
					*pDestBuff = RealDepth;// << 3;
				}
			}
			
			m_DepthFrameTimer.Update();
			m_pSysMemDepthBuffer = NULL;
			//OutputDebugString( L"Depth image processed...\r\n" );
		}
    } else {
        OutputDebugString( L"Buffer length of received texture is bogus\r\n" );
    }

    NuiImageStreamReleaseFrame( m_pDepthStreamHandle, pImageFrame );
}
//--------------------------------------------------------------------------------
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
    if( bFoundSkeleton ) {
        return;
    }

    // smooth out the skeleton data
    //NuiTransformSmooth(&SkeletonFrame,NULL);

    bool bBlank = true;
    for( int i = 0 ; i < NUI_SKELETON_COUNT ; i++ ) {

        if( SkeletonFrame.SkeletonData[i].eTrackingState == NUI_SKELETON_TRACKED ) {

			if ( m_pSysMemSkeletonBuffer != NULL ) {

				memcpy( m_pSysMemSkeletonBuffer, &SkeletonFrame, sizeof(NUI_SKELETON_FRAME) );

				m_pSysMemSkeletonBuffer = NULL;
				m_SkeletonFrameTimer.Update();
			}
        }
    }
}
//--------------------------------------------------------------------------------
void KinectManager::SetSysMemColorBuffer( BYTE* pBuffer )
{
	m_pSysMemColorBuffer = pBuffer;	
}
//--------------------------------------------------------------------------------
void KinectManager::SetSysMemDepthBuffer( BYTE* pBuffer )
{
	m_pSysMemDepthBuffer = pBuffer;	
}
//--------------------------------------------------------------------------------
void KinectManager::SetSysMemSkeletonBuffer( BYTE* pBuffer )
{
	m_pSysMemSkeletonBuffer = pBuffer;
}
//--------------------------------------------------------------------------------
BYTE* KinectManager::GetSysMemColorBuffer( )
{
	return( m_pSysMemColorBuffer );	
}
//--------------------------------------------------------------------------------
BYTE* KinectManager::GetSysMemDepthBuffer( )
{
	return( m_pSysMemDepthBuffer );	
}
//--------------------------------------------------------------------------------
BYTE* KinectManager::GetSysMemSkeletonBuffer( )
{
	return( m_pSysMemSkeletonBuffer );
}
//--------------------------------------------------------------------------------