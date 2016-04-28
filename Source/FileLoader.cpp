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
#include "FileLoader.h"
#include "FileSystem.h"
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
FileLoader::FileLoader()
{
	m_pData = nullptr;
	m_uiSize = 0;
}
//--------------------------------------------------------------------------------
FileLoader::~FileLoader()
{
	Close();
}
//--------------------------------------------------------------------------------
bool FileLoader::Open( const std::wstring& filename )
{
	// Close the current file if one is open.
	Close();


	// TODO: This needs to be updated to correctly choose CreateFile2 whenever 
	//       the library is being compiled for a WinRT target...

//#if (_WIN32_WINNT >= 0x0602 /*_WIN32_WINNT_WIN8*/)
//    HANDLE hFile = CreateFile2( filename.c_str(),
//                                GENERIC_READ,
//                                FILE_SHARE_READ,
//                                OPEN_EXISTING,
//                                nullptr );
//#else
    HANDLE hFile = CreateFileW( filename.c_str(),
                                GENERIC_READ,
                                FILE_SHARE_READ,
                                nullptr,
                                OPEN_EXISTING,
                                FILE_ATTRIBUTE_NORMAL,
                                nullptr
                              );
//#endif

    if (INVALID_HANDLE_VALUE == hFile) {
        return( false );
    }

    // Get the file size
    LARGE_INTEGER FileSize = { 0 };

#if (_WIN32_WINNT >= _WIN32_WINNT_VISTA)
    FILE_STANDARD_INFO fileInfo;
    if ( !GetFileInformationByHandleEx( hFile, FileStandardInfo, &fileInfo, sizeof(fileInfo) ) ) {
        CloseHandle( hFile );
        return( nullptr );
    }
    FileSize = fileInfo.EndOfFile;
#else
    GetFileSizeEx( hFile, &FileSize );
#endif

    // File is too big for 32-bit allocation, so reject read
    if (FileSize.HighPart > 0) {
        CloseHandle( hFile );
        return( false );
    }

	m_uiSize = FileSize.LowPart;

    // create enough space for the file data
    m_pData = new char[ FileSize.LowPart ];
    
	if (!(m_pData)) {
        CloseHandle( hFile );
		Close();
        return( false );
    }

    // read the data in
    DWORD BytesRead = 0;
    if (!ReadFile( hFile,
                   m_pData,
                   FileSize.LowPart,
                   &BytesRead,
                   nullptr
                 )) {
        CloseHandle( hFile );
		Close();
		return( false );
    }

	CloseHandle( hFile );

    if (BytesRead < FileSize.LowPart) {
		Close();
		return( false );
    }

	
	return( true );
}
//--------------------------------------------------------------------------------
bool FileLoader::Close( )
{
	if ( nullptr != m_pData ) {
		delete [] m_pData;
		m_pData = nullptr;
	}

	m_uiSize = 0;

	return( true );
}
//--------------------------------------------------------------------------------
char* FileLoader::GetDataPtr()
{
	return( m_pData );
}
//--------------------------------------------------------------------------------
unsigned int FileLoader::GetDataSize()
{
	return( m_uiSize );
}
//--------------------------------------------------------------------------------
