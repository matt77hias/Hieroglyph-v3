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
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
//--------------------------------------------------------------------------------
#include "PCH.h"
#include "Application.h"
#include "ScriptManager.h"

#include "Timer.h"
#include "Log.h"

// Keyboard Events
#include "EvtChar.h"
#include "EvtKeyUp.h"
#include "EvtKeyDown.h"

// Mouse Events
#include "EvtMouseWheel.h"
#include "EvtMouseMove.h"
#include "EvtMouseLeave.h"
#include "EvtMouseLButtonUp.h"
#include "EvtMouseLButtonDown.h"
#include "EvtMouseMButtonUp.h"
#include "EvtMouseMButtonDown.h"
#include "EvtMouseRButtonUp.h"
#include "EvtMouseRButtonDown.h"


#include <sstream>
//--------------------------------------------------------------------------------
using namespace Glyph3;

LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);


int WINAPI WinMain(	HINSTANCE h_Inst, HINSTANCE h_PrevInst,	LPSTR lpcmdline, int ncmdshow)
{
	// The instance of the app must already be created by now
	Application* m_pApp = Application::GetApplication();
	if (!m_pApp)
	{
		Log::Get().Write( L"There was no instance of the application." );
		return(-1);
	}


	// Call the application's ConfigureEngineComponent method.  This will load
	// and create each of the engine components that are needed by the application,
	// as well as the windows that are needed for interacting with the user.

	if ( !m_pApp->ConfigureEngineComponents() )
	{
		m_pApp->ShutdownEngineComponents();
		return( false );
	}


	// Call the application initialize function - this function
	// will be overloaded specifically for each of the applications.
	
	m_pApp->Initialize();


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
		m_pApp->Update();
	}
	
	// Call the overloaded application shutdown method.
	m_pApp->Shutdown();
	m_pApp->ShutdownEngineComponents();

	return( true );
}
//--------------------------------------------------------------------------------
LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
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