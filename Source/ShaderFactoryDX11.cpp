//--------------------------------------------------------------------------------
// This file is a portion of the Hieroglyph 3 Rendering Engine.  It is distributed
// under the MIT License, available in the root of this distribution and 
// at the following URL:
//
// http://www.opensource.org/licenses/mit-license.php
//
// Copyright (c) Jason Zink 
//--------------------------------------------------------------------------------
#pragma warning( disable : 4244 )
//--------------------------------------------------------------------------------
#include "PCH.h"
#include "ShaderFactoryDX11.h"
#include "Log.h"
#include "FileSystem.h"
#include "EventManager.h"
#include "EvtErrorMessage.h"
#include "FileLoader.h"
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

	std::wstringstream message;

	ID3DBlob* pCompiledShader = nullptr;
	ID3DBlob* pErrorMessages = nullptr;

	char AsciiFunction[1024];
	char AsciiModel[1024];
	WideCharToMultiByte(CP_ACP, 0, function.c_str(), -1, AsciiFunction, 1024, NULL, NULL);
	WideCharToMultiByte(CP_ACP, 0, model.c_str(), -1, AsciiModel, 1024, NULL, NULL);

	// TODO: The compilation of shaders has to skip the warnings as errors 
	//       for the moment, since the new FXC.exe compiler in VS2012 is
	//       apparently more strict than before.

    UINT flags = D3DCOMPILE_PACK_MATRIX_ROW_MAJOR;
#ifdef _DEBUG
    flags |= D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION; // | D3DCOMPILE_WARNINGS_ARE_ERRORS;
#endif

	// Get the current path to the shader folders, and add the filename to it.

	FileSystem fs;
	std::wstring filepath = fs.GetShaderFolder() + filename;

	// Load the file into memory

	FileLoader SourceFile;
	if ( !SourceFile.Open( filepath ) ) {
		message << "Unable to load shader from file: " << filepath;
		EventManager::Get()->ProcessEvent( EvtErrorMessagePtr( new EvtErrorMessage( message.str() ) ) );
		return( nullptr );
	}


	if ( FAILED( hr = D3DCompile( 
		SourceFile.GetDataPtr(),
		SourceFile.GetDataSize(),
		nullptr,
		pDefines,
		nullptr,
		AsciiFunction,
		AsciiModel,
		flags,
		0,
		&pCompiledShader,
		&pErrorMessages ) ) )

	//if ( FAILED( hr = D3DX11CompileFromFile(
	//	filename.c_str(),
	//	pDefines,
	//	0,
	//	AsciiFunction,
	//	AsciiModel,
	//	flags,
	//	0,//UINT Flags2,
	//	0,
	//	&pCompiledShader,
	//	&pErrorMessages,
	//	&hr
	//	) ) )
	{
		message << L"Error compiling shader program: " << filepath << std::endl << std::endl;
		message << L"The following error was reported:" << std::endl;

		if ( ( enablelogging ) && ( pErrorMessages != nullptr ) )
		{
			LPVOID pCompileErrors = pErrorMessages->GetBufferPointer();
			const char* pMessage = (const char*)pCompileErrors;
			message << GlyphString::ToUnicode( std::string( pMessage ) );
			Log::Get().Write( message.str() );
		}

		EventManager::Get()->ProcessEvent( EvtErrorMessagePtr( new EvtErrorMessage( message.str() ) ) );

		SAFE_RELEASE( pCompiledShader );
		SAFE_RELEASE( pErrorMessages );

		return( nullptr );
	}

	SAFE_RELEASE( pErrorMessages );

	return( pCompiledShader );
}
//--------------------------------------------------------------------------------
ID3DBlob* ShaderFactoryDX11::GeneratePrecompiledShader( std::wstring& filename, std::wstring& function,
            std::wstring& model )
{
	std::wstringstream message;

	// Determine where to look for the shader file

	FileSystem fs;
	std::wstring filepath = fs.GetShaderFolder() + filename + L"_" + function + L"_" + model + L".cso";

	// Load the file into memory

	FileLoader CompiledObjectFile;
	if ( !CompiledObjectFile.Open( filepath ) ) {
		message << "Unable to load shader from file: " << filepath;
		EventManager::Get()->ProcessEvent( EvtErrorMessagePtr( new EvtErrorMessage( message.str() ) ) );
		return( nullptr );
	}

	// Create a blob to store the object code in
	
	ID3DBlob* pBlob = nullptr;
	HRESULT hr = D3DCreateBlob( CompiledObjectFile.GetDataSize(), &pBlob );

	if ( FAILED( hr ) ) {
		message << "Unable to create a D3DBlob of size: " << CompiledObjectFile.GetDataSize() << L" while compiling shader: " << filepath;
		EventManager::Get()->ProcessEvent( EvtErrorMessagePtr( new EvtErrorMessage( message.str() ) ) );
		return( nullptr );
	}


	// Copy the file data into the blob
	memcpy( pBlob->GetBufferPointer(), CompiledObjectFile.GetDataPtr(), pBlob->GetBufferSize() );

	// The file object will automatically be released when it goes out of scope,
	// and hence will free its loaded contents automatically also.

	return( pBlob );
}
//--------------------------------------------------------------------------------