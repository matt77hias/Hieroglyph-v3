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

		bool OperatingOnVista( );
		bool OperatingOnXP( );

		std::wstring GetLogFolder( );

	private:
	};
};
//--------------------------------------------------------------------------------
#endif // FileSystem_h