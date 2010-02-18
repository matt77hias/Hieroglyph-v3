//--------------------------------------------------------------------------------
// This file is a portion of the Hieroglyph 3 Rendering Engine.  It is distributed
// under the MIT License, available in the root of this distribution and 
// at the following URL:
//
// http://www.opensource.org/licenses/mit-license.php
//
// Copyright (c) 2003-2010 Jason Zink 
//--------------------------------------------------------------------------------
#include "Application.h"
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
// Initialize the application pointer to NULL
Application* Application::ms_pApplication = NULL;
//--------------------------------------------------------------------------------
Application::Application()
{
	ms_pApplication = this;

	m_pTimer = new Timer();
	m_pTimer->Update();

	Log::Get().Open();

	m_iWidth = 640;
	m_iHeight = 480;
	m_bWindowed = true;

	m_pEventMgr = 0;
	ms_WindowID = 0;
}
//--------------------------------------------------------------------------------
Application::Application( int iWidth, int iHeight, bool bWindowed )
{
	ms_pApplication = this;

	m_pTimer = new Timer();
	m_pTimer->Update();

	Log::Get().Open();

	m_iWidth = iWidth;
	m_iHeight = iHeight;
	m_bWindowed = bWindowed;

	m_pEventMgr = 0;
	ms_WindowID = 0;
}
//--------------------------------------------------------------------------------
Application::~Application( )
{
	if ( m_pTimer != NULL )
		delete m_pTimer;

	Log::Get().Close();
}
//--------------------------------------------------------------------------------
int Application::DisplayWidth( )
{
	return( m_iWidth );
}
//--------------------------------------------------------------------------------
int Application::DisplayHeight( )
{
	return( m_iHeight );
}
//--------------------------------------------------------------------------------
bool Application::DisplayWindowed( )
{
	return( m_bWindowed );
}
//--------------------------------------------------------------------------------
void Application::Initialize( )
{
}
//--------------------------------------------------------------------------------
void Application::Update( )
{
}
//--------------------------------------------------------------------------------
void Application::Shutdown( )
{
	if ( m_pEventMgr )
		delete m_pEventMgr;
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
void Application::SetApplicationID( HINSTANCE ApplicationID )
{
	m_AppInstance = ApplicationID;
}
//--------------------------------------------------------------------------------
void Application::SetWindowID( HWND WindowID )
{
	ms_WindowID = WindowID;
}
//--------------------------------------------------------------------------------