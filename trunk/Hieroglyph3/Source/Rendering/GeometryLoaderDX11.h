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
// GeometryLoaderDX11
//
//--------------------------------------------------------------------------------
#ifndef GeometryLoaderDX11_h
#define GeometryLoaderDX11_h
//--------------------------------------------------------------------------------
#include "GeometryDX11.h"
#include "SkinnedActor.h"
#include <string>
//--------------------------------------------------------------------------------
namespace Glyph3
{
	class GeometryLoaderDX11
	{
	public:
		//static GeometryDX11* loadPlyFile( std::wstring filename );
		//static GeometryDX11* loadMS3DFile( std::wstring filename );		
		static GeometryDX11* loadMS3DFile2( std::wstring filename );		
		static GeometryDX11* loadMS3DFileWithAnimation( std::wstring filename, SkinnedActor* pActor = 0 );		
		
		//static void removeWhiteSpace( std::wstring& s );
		//static std::wstring getElementName( int usage, int index );

	private:
		GeometryLoaderDX11();
		
	};
};
#endif // GeometryLoaderDX11_h


