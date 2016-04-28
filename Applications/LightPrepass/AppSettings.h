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
// AppSettings
//
//--------------------------------------------------------------------------------
#ifndef AppSettings_h
#define AppSettings_h
//--------------------------------------------------------------------------------
#include <string>
//--------------------------------------------------------------------------------
template <typename T>
void IncrementSetting( T& setting, int maxValue )
{
    int val = static_cast<int>( setting );
    val = ( val + 1 ) % maxValue;
    setting = static_cast<T>( val );
}
//--------------------------------------------------------------------------------
// Different scene light configurations
class LightMode
{
public:

    enum Settings
    {
        Lights3x3x3 = 0,
        Lights5x5x5,
        Lights7x7x7,

        NumSettings
    };

    static const wchar_t Key = 'N';

    static Settings Value;

    static std::wstring ToString()
    {
        static const std::wstring Names[NumSettings] =
        {
            L"3x3x3",
            L"5x5x5",
            L"7x7x7"
        };

        std::wstring text = L"Number of Lights(N): ";
        text += Names[Value];

        return text;
    }

    static void Increment()
    {
        IncrementSetting( Value, NumSettings );
    }
};
//--------------------------------------------------------------------------------
#endif // AppSettings_h