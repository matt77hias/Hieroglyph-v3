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
#include "GlyphString.h"
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
GlyphString::GlyphString( )
{
}
//--------------------------------------------------------------------------------
std::string GlyphString::ToAscii( std::wstring& s )
{
	std::string temp(s.length(), ' ');
	#pragma warning( disable : 4244 )
	std::copy(s.begin(), s.end(), temp.begin());
	return temp;
}
//--------------------------------------------------------------------------------
std::wstring GlyphString::ToUnicode( std::string& s )
{
	std::wstring temp(s.length(),L' ');
	std::copy(s.begin(), s.end(), temp.begin());
	return temp; 
}
//--------------------------------------------------------------------------------