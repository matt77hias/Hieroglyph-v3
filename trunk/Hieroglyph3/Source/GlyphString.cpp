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
#include "GlyphString.h"
#include <locale>
#include <codecvt>
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
GlyphString::GlyphString( )
{
}
//--------------------------------------------------------------------------------
std::string GlyphString::ToAscii( const std::wstring& s )
{
	return std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t>().to_bytes( s ) ;
}
//--------------------------------------------------------------------------------
std::wstring GlyphString::ToUnicode( const std::string& s )
{
	return std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t>().from_bytes( s );
}
//--------------------------------------------------------------------------------

