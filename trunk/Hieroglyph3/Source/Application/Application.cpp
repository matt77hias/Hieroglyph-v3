//--------------------------------------------------------------------------------
// This file is a portion of the Hieroglyph 3 Rendering Engine.  It is distributed
// under the MIT License, available in the root of this distribution and 
// at the following URL:
//
// http://www.opensource.org/licenses/mit-license.php
//
// Copyright (c) 2003-2010 Jason Zink 
//--------------------------------------------------------------------------------
// 06.02.2012: BeforeRegisterWindowClass default implementation added by Francois
//             Piette.
//--------------------------------------------------------------------------------
#include "PCH.h"
#include "Application.h"
#include "EvtInfoMessage.h"
#include "EvtErrorMessage.h"
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
// Initialize the application pointer to NULL
Application* Application::ms_pApplication = NULL;
//--------------------------------------------------------------------------------
Application::Application()
	: m_bSaveScreenshot( false )
{
	// Record the this pointer to provide access to the WinMain function.

	ms_pApplication = this;

	// Create and initialize the timer object for use in the subclass.

	m_pTimer = new Timer();
	m_pTimer->Update();

	// Open the log file for use by the engine/application in general.

	Log::Get().Open();

	// Create the event manager here since it must be the first engine component
	// instantiated.

	m_pEventMgr = new EventManager();

	SetEventManager( m_pEventMgr );

	// The application object wants to know about these events, so it 
	// registers itself with the appropriate event IDs.

	RequestEvent( SYSTEM_KEYBOARD_KEYUP );
	RequestEvent( SYSTEM_KEYBOARD_KEYDOWN );
	RequestEvent( SYSTEM_KEYBOARD_CHAR );
	RequestEvent( INFO_MESSAGE );
	RequestEvent( ERROR_MESSAGE );

	// TODO: this should go into the RenderApplication class!
	m_pScene = new Scene();
}
//--------------------------------------------------------------------------------
Application::~Application( )
{
	if ( m_pScene != NULL )
		delete m_pScene;

	if ( m_pTimer != NULL )
		delete m_pTimer;

	if ( m_pEventMgr )
		delete m_pEventMgr;

	Log::Get().Close();
}
//--------------------------------------------------------------------------------
Application* Application::GetApplication( )
{
	return( ms_pApplication );
}
//--------------------------------------------------------------------------------
void Application::RequestTermination( )
{
	// This triggers the termination of the application
	PostQuitMessage( 0 );
}
//--------------------------------------------------------------------------------
bool Application::HandleEvent( IEvent* pEvent )
{
	eEVENT e = pEvent->GetEventType();

	if ( e == INFO_MESSAGE )
	{
		EvtInfoMessage* pInfoMessage = dynamic_cast<EvtInfoMessage*>( pEvent );
		MessageBox( 0, pInfoMessage->GetInfoMessage().c_str(), L"Hieroglyph 3 :: Info Message", MB_ICONINFORMATION | MB_SYSTEMMODAL );
	}
	else if ( e == ERROR_MESSAGE )
	{
		EvtErrorMessage* pErrorMessage = dynamic_cast<EvtErrorMessage*>( pEvent );
		MessageBox( 0, pErrorMessage->GetErrorMessage().c_str(), L"Hieroglyph 3 :: Error Message", MB_ICONERROR | MB_SYSTEMMODAL );
		RequestTermination();
	}
	else if ( e == SYSTEM_KEYBOARD_KEYUP )
	{
		EvtKeyUp* pKeyUp = (EvtKeyUp*)pEvent;

		unsigned int key = pKeyUp->GetCharacterCode();

		if ( key == VK_ESCAPE ) // 'Esc' Key - Exit the application
		{
			this->RequestTermination();
			return( true );
		}
		else if ( key == VK_SPACE ) // 'SPACE' Key - Take a screenshot
		{
			m_bSaveScreenshot = true;
			return( true );
		}
	}

	return( false );
}
//--------------------------------------------------------------------------------
void Application::MessageLoop()
{
	MSG msg;
	bool bLoop = true;
	
	while( bLoop )
	{
		while ( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ) )
		{ 
			if ( msg.message == WM_QUIT )
			{
				bLoop = false;
				break;
			}
			TranslateMessage( &msg );
			DispatchMessage( &msg );
		}

		// Call the overloaded application update function.
		Update();
		TakeScreenShot();
	}
}
//--------------------------------------------------------------------------------
void Application::BeforeRegisterWindowClass( WNDCLASSEX &wc )
{
	// This function is intended to be overriden in dervived classes
}
//--------------------------------------------------------------------------------
LRESULT Application::WindowProc( HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam )
{
	switch( msg )
	{	
		case WM_CREATE: 
			{
				// Automatically return 0 to allow the window to proceed in the
				// creation process.

				return( 0 );
			} break;

		case WM_PAINT:
			{
				// This message is handled by the default handler to avoid a 
				// repeated sending of the message.  This results in the ability
				// to process all pending messages at once without getting stuck
				// in an eternal loop.
			} break;

		case WM_CLOSE:
			{
				// This message is sent when a window or an application should
				// terminate.
			} break;

		case WM_DESTROY: 
			{
				// This message is sent when a window has been destroyed.

				PostQuitMessage(0);
				return( 0 );
			} break;

		case WM_SIZE:
			{				
                EvtWindowResize* pEvent = new EvtWindowResize( hwnd, wparam, lparam );
                EventManager::Get()->ProcessEvent( pEvent );
			} break;


		case WM_LBUTTONUP:
			{				
                EvtMouseLButtonUp* pEvent = new EvtMouseLButtonUp( hwnd, wparam, lparam );
                EventManager::Get()->ProcessEvent( pEvent );
			} break;

		case WM_LBUTTONDOWN:
			{
                EvtMouseLButtonDown* pEvent = new EvtMouseLButtonDown( hwnd, wparam, lparam );
                EventManager::Get()->ProcessEvent( pEvent );
			} break;
			
		case WM_MBUTTONUP:
			{
                EvtMouseMButtonUp* pEvent = new EvtMouseMButtonUp( hwnd, wparam, lparam );
                EventManager::Get()->ProcessEvent( pEvent );
			} break;

		case WM_MBUTTONDOWN:
			{
                EvtMouseMButtonDown* pEvent = new EvtMouseMButtonDown( hwnd, wparam, lparam );
                EventManager::Get()->ProcessEvent( pEvent );
			} break;

		case WM_RBUTTONUP:
			{
                EvtMouseRButtonUp* pEvent = new EvtMouseRButtonUp( hwnd, wparam, lparam );
                EventManager::Get()->ProcessEvent( pEvent );
			} break;

		case WM_RBUTTONDOWN:
			{
                EvtMouseRButtonDown* pEvent = new EvtMouseRButtonDown( hwnd, wparam, lparam );
                EventManager::Get()->ProcessEvent( pEvent );
			} break;

		case WM_MOUSEMOVE:
			{
                EvtMouseMove* pEvent = new EvtMouseMove( hwnd, wparam, lparam );
                EventManager::Get()->ProcessEvent( pEvent );
			} break;

		case WM_MOUSEWHEEL:
			{
                EvtMouseWheel* pEvent = new EvtMouseWheel( hwnd, wparam, lparam );
                EventManager::Get()->ProcessEvent( pEvent );
			} break;

		case WM_CHAR:
			{
				EvtChar* pEvent = new EvtChar( hwnd, wparam, lparam );
				EventManager::Get()->ProcessEvent( pEvent );
				//return( 0 );
			} break;

		case WM_KEYDOWN:
			{
				EvtKeyDown* pEvent = new EvtKeyDown( hwnd, wparam, lparam );
				EventManager::Get()->ProcessEvent( pEvent );
				//return( 0 );
			} break;

		case WM_KEYUP:
			{
				EvtKeyUp* pEvent = new EvtKeyUp( hwnd, wparam, lparam );
				EventManager::Get()->ProcessEvent( pEvent );
				//return( 0 );
			} break;
    }
    return( DefWindowProc( hwnd, msg, wparam, lparam ) );
}
//--------------------------------------------------------------------------------
