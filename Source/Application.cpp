//--------------------------------------------------------------------------------
// This file is a portion of the Hieroglyph 3 Rendering Engine.  It is distributed
// under the MIT License, available in the root of this distribution and 
// at the following URL:
//
// http://www.opensource.org/licenses/mit-license.php
//
// Copyright (c) Jason Zink 
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
Application::Application() :
	m_bSaveScreenshot( false ),
	m_bLoop( true )
{
	// Record the this pointer to provide access to the WinMain function.

	ms_pApplication = this;

	// Create and initialize the timer object for use in the subclass.

	m_pTimer = new Timer();
	m_pTimer->Update();

	// Open the log file for use by the engine/application in general.

	Log::Get().Open();

	// The event manager can be used here since it must be the first engine component
	// to be active.

	SetEventManager( &EvtManager );

	// The application object wants to know about these events, so it 
	// registers itself with the appropriate event IDs.

	RequestEvent( SYSTEM_KEYBOARD_KEYUP );
	RequestEvent( SYSTEM_KEYBOARD_KEYDOWN );
	RequestEvent( SYSTEM_KEYBOARD_CHAR );
	RequestEvent( INFO_MESSAGE );
	RequestEvent( ERROR_MESSAGE );

	// Create the scene to be manipulated.

	m_pScene = new Scene();
}
//--------------------------------------------------------------------------------
Application::~Application( )
{
	if ( m_pScene != NULL )
		delete m_pScene;

	if ( m_pTimer != NULL )
		delete m_pTimer;

	Log::Get().Close();
}
//--------------------------------------------------------------------------------
Application* Application::GetApplication( )
{
	return( ms_pApplication );
}
//--------------------------------------------------------------------------------
bool Application::ConfigureCommandLine( LPSTR lpcmdline )
{
	// Default to returning true, but allow sub-classes to override this behavior.
	return( true );
}
//--------------------------------------------------------------------------------
void Application::RequestTermination( )
{
	// This triggers the termination of the application
	PostQuitMessage( 0 );
}
//--------------------------------------------------------------------------------
bool Application::HandleEvent( EventPtr pEvent )
{
	eEVENT e = pEvent->GetEventType();

	if ( e == INFO_MESSAGE )
	{
		EvtInfoMessagePtr pInfoMessage = std::static_pointer_cast<EvtInfoMessage>( pEvent );
		MessageBox( 0, pInfoMessage->GetInfoMessage().c_str(), L"Hieroglyph 3 :: Info Message", MB_ICONINFORMATION | MB_SYSTEMMODAL );
	}
	else if ( e == ERROR_MESSAGE )
	{
		EvtErrorMessagePtr pErrorMessage = std::static_pointer_cast<EvtErrorMessage>( pEvent );
		MessageBox( 0, pErrorMessage->GetErrorMessage().c_str(), L"Hieroglyph 3 :: Error Message", MB_ICONERROR | MB_SYSTEMMODAL );
		RequestTermination();
	}
	else if ( e == SYSTEM_KEYBOARD_KEYUP )
	{
		EvtKeyUpPtr pKeyUp = std::static_pointer_cast<EvtKeyUp>( pEvent );

		unsigned int key = pKeyUp->GetCharacterCode();

		if ( key == VK_ESCAPE ) // 'Esc' Key - Exit the application
		{
			RequestTermination();
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
	
	while( true )
	{
		while ( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ) )
		{ 
			if ( msg.message == WM_QUIT )
			{
				return;
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
                EvtWindowResizePtr pEvent = EvtWindowResizePtr( new EvtWindowResize( hwnd, wparam, lparam ) );
                EvtManager.ProcessEvent( pEvent );
			} break;


		case WM_LBUTTONUP:
			{				
                EvtMouseLButtonUpPtr pEvent = EvtMouseLButtonUpPtr( new EvtMouseLButtonUp( hwnd, wparam, lparam ) );
                EvtManager.ProcessEvent( pEvent );
			} break;

		case WM_LBUTTONDOWN:
			{
                EvtMouseLButtonDownPtr pEvent = EvtMouseLButtonDownPtr( new EvtMouseLButtonDown( hwnd, wparam, lparam ) );
                EvtManager.ProcessEvent( pEvent );
			} break;
			
		case WM_MBUTTONUP:
			{
                EvtMouseMButtonUpPtr pEvent = EvtMouseMButtonUpPtr( new EvtMouseMButtonUp( hwnd, wparam, lparam ) );
                EvtManager.ProcessEvent( pEvent );
			} break;

		case WM_MBUTTONDOWN:
			{
                EvtMouseMButtonDownPtr pEvent = EvtMouseMButtonDownPtr( new EvtMouseMButtonDown( hwnd, wparam, lparam ) );
                EvtManager.ProcessEvent( pEvent );
			} break;

		case WM_RBUTTONUP:
			{
                EvtMouseRButtonUpPtr pEvent = EvtMouseRButtonUpPtr( new EvtMouseRButtonUp( hwnd, wparam, lparam ) );
                EvtManager.ProcessEvent( pEvent );
			} break;

		case WM_RBUTTONDOWN:
			{
                EvtMouseRButtonDownPtr pEvent = EvtMouseRButtonDownPtr( new EvtMouseRButtonDown( hwnd, wparam, lparam ) );
                EvtManager.ProcessEvent( pEvent );
			} break;

		case WM_MOUSEMOVE:
			{
                EvtMouseMovePtr pEvent = EvtMouseMovePtr( new EvtMouseMove( hwnd, wparam, lparam ) );
                EvtManager.ProcessEvent( pEvent );
			} break;

		case WM_MOUSEWHEEL:
			{
                EvtMouseWheelPtr pEvent = EvtMouseWheelPtr( new EvtMouseWheel( hwnd, wparam, lparam ) );
                EvtManager.ProcessEvent( pEvent );
			} break;

		case WM_CHAR:
			{
				EvtCharPtr pEvent = EvtCharPtr( new EvtChar( hwnd, wparam, lparam ) );
				EvtManager.ProcessEvent( pEvent );
			} break;

		case WM_KEYDOWN:
			{
				EvtKeyDownPtr pEvent = EvtKeyDownPtr( new EvtKeyDown( hwnd, wparam, lparam ) );
				EvtManager.ProcessEvent( pEvent );
			} break;

		case WM_KEYUP:
			{
				EvtKeyUpPtr pEvent = EvtKeyUpPtr( new EvtKeyUp( hwnd, wparam, lparam ) );
				EvtManager.ProcessEvent( pEvent );
			} break;
    }
    return( DefWindowProc( hwnd, msg, wparam, lparam ) );
}
//--------------------------------------------------------------------------------
