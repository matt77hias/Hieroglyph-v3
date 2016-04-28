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
// Different light optimization types
class LightOptMode
{
public:

    enum Settings
    {
        None = 0,
        ScissorRect,
        Volumes,

        NumSettings
    };

    static const wchar_t Key = 'O';

    static Settings Value;

    static std::wstring ToString()
    {
        static const std::wstring Names[NumSettings] =
        {
            L"No Optimizations",
            L"Scissor Rectangle",
            L"Light Volumes"
        };

        std::wstring text = L"Light Optimization Mode(O): ";
        text += Names[Value];

        return text;
    }

    static void Increment()
    {
        IncrementSetting( Value, NumSettings );
    }
};
//--------------------------------------------------------------------------------
// What's displayed on the screen
class DisplayMode
{
public:

    enum Settings
    {
        Final = 0,
        GBuffer,
        Normals,
        DiffuseAlbedo,
        SpecularAlbedo,
        Position,

        NumSettings
    };

    static const wchar_t Key = 'V';

    static Settings Value;

    static std::wstring ToString()
    {
        static const std::wstring Names[NumSettings] =
        {
            L"Final",
            L"G-Buffer",
            L"Normals",
            L"Diffuse Albedo",
            L"Specular Albedo",
            L"Position/Depth"
        };

        std::wstring text = L"Display Mode(V): ";
        text += Names[Value];

        return text;
    }

    static void Increment()
    {
        IncrementSetting( Value, NumSettings );
    }
};
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
// G-Buffer Optimization Modes
class GBufferOptMode
{
public:

    enum Settings
    {
        OptDisabled = 0,
        OptEnabled,

        NumSettings
    };

    static const wchar_t Key = 'K';

    static Settings Value;

    static std::wstring ToString()
    {
        static const std::wstring Names[NumSettings] =
        {
            L"Disabled",
            L"Enabled"
        };

        std::wstring text = L"G-Buffer Optimization Mode(K): ";
        text += Names[Value];

        return text;
    }

    static bool Enabled()
    {
        return Value == OptEnabled;
    }

    static void Increment()
    {
        IncrementSetting( Value, NumSettings );
    }
};
//--------------------------------------------------------------------------------
// Antialiasing modes
class AAMode
{
public:

    enum Settings
    {
        None = 0,
        SSAA,
        MSAA,

        NumSettings
    };

    static const wchar_t Key = 'M';

    static Settings Value;

    static std::wstring ToString()
    {
        static const std::wstring Names[NumSettings] =
        {
            L"None",
            L"Supersampling",
            L"Multisampling"
        };

        std::wstring text = L"Antialiasing Mode(M): ";
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