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
#include "PCH.h"
#include "FileSystem.h"
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
FileSystem::FileSystem()
{
}
//--------------------------------------------------------------------------------
FileSystem::~FileSystem()
{
}
//--------------------------------------------------------------------------------
bool FileSystem::OperatingOnVista()
{
	OSVERSIONINFO osver;
	memset(&osver, 0, sizeof(OSVERSIONINFO));
	osver.dwOSVersionInfoSize = sizeof( OSVERSIONINFO );

	if (!GetVersionEx( &osver ))
		return FALSE;

	if (   osver.dwPlatformId == VER_PLATFORM_WIN32_NT
		&& osver.dwMajorVersion >= 6  )
		return TRUE;

	return FALSE;
}
//--------------------------------------------------------------------------------
bool FileSystem::OperatingOnXP()
{
	OSVERSIONINFO osver;
	memset(&osver, 0, sizeof(OSVERSIONINFO));
	osver.dwOSVersionInfoSize = sizeof( OSVERSIONINFO );

	if (!GetVersionEx( &osver ))
		return FALSE;

	if (   osver.dwPlatformId == VER_PLATFORM_WIN32_NT
		&& osver.dwMajorVersion >= 5  )
		return TRUE;

	return FALSE;

//bIsWindowsXPorLater = 
  // ( (osvi.dwMajorVersion > 5) ||
   //( (osvi.dwMajorVersion == 5) && (osvi.dwMinorVersion >= 1) );
}
//--------------------------------------------------------------------------------
std::wstring FileSystem::GetLogFolder( )
{
	wchar_t buffer[MAX_PATH];

	HRESULT hr = SHGetFolderPath( 0,
	    CSIDL_LOCAL_APPDATA,
		0,
		SHGFP_TYPE_CURRENT,
		buffer
	);

	std::wstring result = buffer;

	return( result );
}
//--------------------------------------------------------------------------------