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
#include "PCH.h"
#include "Application.h"
#include "ScriptManager.h"

#include "Timer.h"
#include "Log.h"

#include <sstream>
//--------------------------------------------------------------------------------
using namespace Glyph3;

int WINAPI WinMain(	HINSTANCE h_Inst, HINSTANCE h_PrevInst,	LPSTR lpcmdline, int ncmdshow)
{
	// The instance of the app must already be created by now

	Application* m_pApp = Application::GetApplication();
	if (!m_pApp)
	{
		Log::Get().Write( L"There was no instance of the application." );
		return(-1);
	}

	// Give the application a chance to process the command line arguments.  The
	// default behavior is to simply return without doing anything on the parameters,
	// but a customized application can do as they wish with the command line.

	if ( !m_pApp->ConfigureCommandLine( lpcmdline ) )
	{
        Log::Get().Write( L"Failed to process the command line arguments!" );
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

    
	// Call the application message loop function - this function
	// could be overloaded specifically for each the applications.

	m_pApp->MessageLoop();


	// Call the overloaded application shutdown method.

	m_pApp->Shutdown();
	m_pApp->ShutdownEngineComponents();

	return( true );
}
//--------------------------------------------------------------------------------
