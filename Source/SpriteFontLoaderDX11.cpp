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
#include "SpriteFontLoaderDX11.h"
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
std::vector< SpriteFontPtr > SpriteFontLoaderDX11::FontList;
//--------------------------------------------------------------------------------
SpriteFontLoaderDX11::SpriteFontLoaderDX11()
{
}
//--------------------------------------------------------------------------------
SpriteFontLoaderDX11::~SpriteFontLoaderDX11()
{
}
//--------------------------------------------------------------------------------
SpriteFontPtr SpriteFontLoaderDX11::LoadFont( std::wstring& fontName, float fontSize, UINT fontStyle, bool antiAliased )
{
	SpriteFontPtr pFont = nullptr;

	// Search our cache for existing font objects that match the requested
	// parameters.

	for ( auto pCachedFont : FontList )
	{
		if ( pCachedFont->FontName().compare( fontName ) == 0
			&& pCachedFont->Size() == fontSize
			&& pCachedFont->FontStyle() == fontStyle
			&& pCachedFont->AntiAliased() == antiAliased )
		{
			pFont = pCachedFont;
			break;
		}
	}

	// If the font was not found, then we must create a new one and add 
	// it to the list for next time.

	if ( pFont == nullptr )
	{
		pFont = SpriteFontPtr( new SpriteFontDX11() );
		pFont->Initialize( fontName, fontSize, fontStyle, antiAliased );
		FontList.push_back( pFont );
	}

	// Regardless of if it was cached or newly created, return the font reference.
	
	return( pFont );
}
//--------------------------------------------------------------------------------
