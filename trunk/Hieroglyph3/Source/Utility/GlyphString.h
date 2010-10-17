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
// GlyphString
//
//--------------------------------------------------------------------------------
#ifndef GlyphString_h
#define GlyphString_h
//--------------------------------------------------------------------------------
#include "PCH.h"
//--------------------------------------------------------------------------------
namespace Glyph3
{
	class GlyphString
	{
	public:
		static std::string ToAscii( std::wstring& input );
		static std::wstring ToUnicode( std::string& input );            

	private:
		GlyphString();
		
	};

    // Converts a number to a string
    template<typename T> inline std::wstring ToString(const T& val)
    {
        std::wostringstream stream;
        if (!(stream << val))
            return L"";
        return stream.str();
    }
};
#endif // GlyphString_h


