//--------------------------------------------------------------------------------
// This file is a portion of the Hieroglyph 3 Rendering Engine.  It is distributed
// under the MIT License, available in the root of this distribution and 
// at the following URL:
//
// http://www.opensource.org/licenses/mit-license.php
//
// Copyright (c) 2003-2010 Jason Zink 
//--------------------------------------------------------------------------------
#pragma warning( disable : 4244 )
//--------------------------------------------------------------------------------
#include "PCH.h"
#include "ShaderFactoryDX11.h"
#include "Log.h"
#include "FileSystem.h"
#include "EventManager.h"
#include "EvtErrorMessage.h"
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
ShaderFactoryDX11::ShaderFactoryDX11( )
{
}
//--------------------------------------------------------------------------------
ShaderFactoryDX11::~ShaderFactoryDX11()
{
}
//--------------------------------------------------------------------------------
ID3DBlob* ShaderFactoryDX11::GenerateShader( ShaderType type, std::wstring& filename, std::wstring& function,
            std::wstring& model, const D3D_SHADER_MACRO* pDefines, bool enablelogging )
{
	HRESULT hr = S_OK;

	ID3DBlob* pCompiledShader = nullptr;
	ID3DBlob* pErrorMessages = nullptr;

	char AsciiFunction[1024];
	char AsciiModel[1024];
	WideCharToMultiByte(CP_ACP, 0, function.c_str(), -1, AsciiFunction, 1024, NULL, NULL);
	WideCharToMultiByte(CP_ACP, 0, model.c_str(), -1, AsciiModel, 1024, NULL, NULL);

    UINT flags = D3DCOMPILE_PACK_MATRIX_ROW_MAJOR;
#ifdef _DEBUG
    flags |= D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION | D3DCOMPILE_WARNINGS_ARE_ERRORS;
#endif

	// Get the current path to the shader folders, and add the filename to it.

	FileSystem fs;
	filename = fs.GetShaderFolder() + filename;

	if ( FAILED( hr = D3DX11CompileFromFile(
		filename.c_str(),
		pDefines,
		0,
		AsciiFunction,
		AsciiModel,
		flags,
		0,//UINT Flags2,
		0,
		&pCompiledShader,
		&pErrorMessages,
		&hr
		) ) )
	{

		std::wstringstream message;

		message << L"Error compiling shader program: " << filename << std::endl << std::endl;
		message << L"The following error was reported:" << std::endl;

		if ( ( enablelogging ) && ( pErrorMessages != nullptr ) )
		{
			LPVOID pCompileErrors = pErrorMessages->GetBufferPointer();
			const char* pMessage = (const char*)pCompileErrors;
			message << GlyphString::ToUnicode( std::string( pMessage ) );
			Log::Get().Write( message.str() );
		}

		EventManager::Get()->ProcessEvent( new EvtErrorMessage( message.str() ) );

		SAFE_RELEASE( pCompiledShader );
		SAFE_RELEASE( pErrorMessages );

		return( nullptr );
	}

	SAFE_RELEASE( pErrorMessages );

	return( pCompiledShader );
}
//--------------------------------------------------------------------------------
ID3DBlob* ShaderFactoryDX11::GenerateShader( std::wstring& filename )
{
	// Load the pre-compiled shader to memory
	
#if (_WIN32_WINNT >= 0x0602 /*_WIN32_WINNT_WIN8*/)
    HANDLE hFile = CreateFile2( filename.c_str(),
                                GENERIC_READ,
                                FILE_SHARE_READ,
                                OPEN_EXISTING,
                                nullptr );
#else
    HANDLE hFile = CreateFileW( filename.c_str(),
                                GENERIC_READ,
                                FILE_SHARE_READ,
                                nullptr,
                                OPEN_EXISTING,
                                FILE_ATTRIBUTE_NORMAL,
                                nullptr
                              );
#endif

    if (INVALID_HANDLE_VALUE == hFile) {
        return( nullptr );
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
        return( nullptr );
    }

    // create enough space for the file data
    char* pData = new char[ FileSize.LowPart ];
    if (!(pData)) {
        CloseHandle( hFile );
        return( nullptr );
    }

    // read the data in
    DWORD BytesRead = 0;
    if (!ReadFile( hFile,
                   pData,
                   FileSize.LowPart,
                   &BytesRead,
                   nullptr
                 )) {
        CloseHandle( hFile );
        delete [] pData;
        pData = nullptr;
        return( nullptr );
    }

	CloseHandle( hFile );

    if (BytesRead < FileSize.LowPart) {
        delete [] pData;
        pData = nullptr;
        return( nullptr );
    }


	// Create a blob to store the object code in
	
	ID3DBlob* pBlob = nullptr;
	HRESULT hr = D3DCreateBlob( FileSize.LowPart, &pBlob );

	// TODO: check for failure here...


	// Copy the file data into the blob
	memcpy( pBlob->GetBufferPointer(), pData, pBlob->GetBufferSize() );

	// Release the system memory and return the blob
	delete [] pData;
	return( pBlob );
}
//--------------------------------------------------------------------------------