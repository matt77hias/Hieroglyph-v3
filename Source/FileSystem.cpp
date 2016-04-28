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
#include "FileSystem.h"
#include <shlobj.h>
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
std::wstring FileSystem::sDataFolder = L"../Data/";
std::wstring FileSystem::sModelsSubFolder = L"Models/";
std::wstring FileSystem::sScriptsSubFolder = L"Scripts/";
std::wstring FileSystem::sShaderSubFolder = L"Shaders/";
std::wstring FileSystem::sTextureSubFolder = L"Textures/";
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
std::wstring FileSystem::GetDataFolder()
{
	return( sDataFolder );
}
//--------------------------------------------------------------------------------
std::wstring FileSystem::GetModelsFolder()
{
	return( sDataFolder + sModelsSubFolder );
}
//--------------------------------------------------------------------------------
std::wstring FileSystem::GetScriptsFolder()
{
	return( sDataFolder + sScriptsSubFolder );
}
//--------------------------------------------------------------------------------
std::wstring FileSystem::GetShaderFolder()
{
	return( sDataFolder + sShaderSubFolder );
}
//--------------------------------------------------------------------------------
std::wstring FileSystem::GetTextureFolder()
{
	return( sDataFolder + sTextureSubFolder );
}
//--------------------------------------------------------------------------------
void FileSystem::SetDataFolder( const std::wstring& folder )
{
	sDataFolder = folder;
}
//--------------------------------------------------------------------------------
void FileSystem::SetModelsFolder( const std::wstring& folder )
{
	sModelsSubFolder = folder;
}
//--------------------------------------------------------------------------------
void FileSystem::SetScriptsFolder( const std::wstring& folder )
{
	sScriptsSubFolder = folder;
}
//--------------------------------------------------------------------------------
void FileSystem::SetShaderFolder( const std::wstring& folder )
{
	sShaderSubFolder = folder;
}
//--------------------------------------------------------------------------------
void FileSystem::SetTextureFolder( const std::wstring& folder )
{
	sTextureSubFolder = folder;
}
//--------------------------------------------------------------------------------
bool FileSystem::FileExists( const std::wstring& file )
{
	// Check if the file exists, and that it is not a directory

	DWORD dwAttrib = GetFileAttributes( file.c_str() );

	return (dwAttrib != INVALID_FILE_ATTRIBUTES && !(dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
}
//--------------------------------------------------------------------------------
bool FileSystem::FileIsNewer( const std::wstring& file1, const std::wstring& file2 )
{
	// This method assumes that the existance of the files has already been verified!

	HANDLE handle1 = INVALID_HANDLE_VALUE;
	HANDLE handle2 = INVALID_HANDLE_VALUE;

	// Get the file handles for the two files
//#if (_WIN32_WINNT >= 0x0602 /*_WIN32_WINNT_WIN8*/)
//    handle1 = CreateFile2( file1.c_str(),
//                           GENERIC_READ,
//                           FILE_SHARE_READ,
//                           OPEN_EXISTING,
//                           nullptr );
//#else
    handle1 = CreateFileW( file1.c_str(),
						   GENERIC_READ,
                           FILE_SHARE_READ,
                           nullptr,
                           OPEN_EXISTING,
                           FILE_ATTRIBUTE_NORMAL,
                           nullptr );
//#endif


//#if (_WIN32_WINNT >= 0x0602 /*_WIN32_WINNT_WIN8*/)
//    handle2 = CreateFile2( file2.c_str(),
//                           GENERIC_READ,
//                           FILE_SHARE_READ,
//                           OPEN_EXISTING,
//                           nullptr );
//#else
    handle2 = CreateFileW( file2.c_str(),
						   GENERIC_READ,
                           FILE_SHARE_READ,
                           nullptr,
                           OPEN_EXISTING,
                           FILE_ATTRIBUTE_NORMAL,
                           nullptr );
//#endif

	FILETIME fileTime1;
	FILETIME fileTime2;

	// Get the file's time info
	GetFileTime( handle1, nullptr, nullptr, &fileTime1 );
	GetFileTime( handle2, nullptr, nullptr, &fileTime2 );

	// Close the files
	CloseHandle( handle1 );
	CloseHandle( handle2 );

	// Return the comparison
	if ( fileTime1.dwHighDateTime > fileTime2.dwHighDateTime ) {
		return( true );
	} else if ( fileTime1.dwHighDateTime == fileTime2.dwHighDateTime ) {
		return( fileTime1.dwLowDateTime > fileTime2.dwLowDateTime );
	}

	return( false );
}
//--------------------------------------------------------------------------------