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
		static std::string ToAscii( const std::wstring& input );
		static std::wstring ToUnicode( const std::string& input );

		static std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems);
		static std::vector<std::string> split(const std::string &s, char delim);

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
//--------------------------------------------------------------------------------
#endif // GlyphString_h
//--------------------------------------------------------------------------------

