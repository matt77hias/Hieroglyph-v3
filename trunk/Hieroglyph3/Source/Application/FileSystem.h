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
// FileSystem
//
// This class uses the Monostate pattern to provide data paths for the 
// application.  The paths are all specified as static variables, but are accessed
// with non-static members.  Just make sure you set these paths before doing 
// anything else in the application, including any of the initialization functions
// if you need specialized locations.
//--------------------------------------------------------------------------------
#ifndef FileSystem_h
#define FileSystem_h
//--------------------------------------------------------------------------------
#include <string>
//--------------------------------------------------------------------------------
namespace Glyph3
{
	class FileSystem
	{
	public:
		FileSystem();
		~FileSystem();

		bool OperatingOnVista();
		bool OperatingOnXP();

		std::wstring GetLogFolder();
		
		std::wstring GetDataFolder();
		std::wstring GetModelsFolder();
		std::wstring GetScriptsFolder();
		std::wstring GetShaderFolder();
		std::wstring GetTextureFolder();

		void SetDataFolder( const std::wstring& folder );
		void SetModelsFolder( const std::wstring& folder );
		void SetScriptsFolder( const std::wstring& folder );
		void SetShaderFolder( const std::wstring& folder );
		void SetTextureFolder( const std::wstring& folder );

	private:

		static std::wstring sDataFolder;
		static std::wstring sModelsSubFolder;
		static std::wstring sScriptsSubFolder;
		static std::wstring sShaderSubFolder;
		static std::wstring sTextureSubFolder;
	};
};
//--------------------------------------------------------------------------------
#endif // FileSystem_h