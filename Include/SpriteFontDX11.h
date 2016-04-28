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
// SpriteFontDX11
//
//--------------------------------------------------------------------------------
#ifndef SpriteFontDX11_h
#define SpriteFontDX11_h
//--------------------------------------------------------------------------------
#include "RendererDX11.h"
#include "Log.h"
#include <GdiPlus.h>

#pragma comment( lib, "gdiplus.lib" )
//--------------------------------------------------------------------------------
#define GdiPlusCall( status )													 \
	do																			 \
	{																			 \
		if ( status != Gdiplus::Ok )											 \
		{																		 \
			Log::Get().Write( std::wstring( L"GDI+ error while creating SpriteFont " ) ); \
			return false;														 \
		}																	     \
	}																		     \
	while ( false )
//--------------------------------------------------------------------------------
namespace Glyph3
{
	class SpriteFontDX11
	{

	public:

		enum FontStyle
		{
			Regular = 0,
			Bold = 1 << 0,
			Italic = 1 << 1,
			BoldItalic = Bold | Italic,
			Underline = 1 << 2,
			Strikeout = 1 << 3
		};

		struct CharDesc
		{
			float X;
			float Y;
			float Width;
			float Height;
		};

		static const WCHAR StartChar = '!';
		static const WCHAR EndChar = 127;
		static const UINT NumChars = EndChar - StartChar;
		static const UINT TexWidth = 1024;

		// Lifetime
		SpriteFontDX11();
		~SpriteFontDX11();

		bool Initialize( std::wstring& fontName, float fontSize, UINT fontStyle, bool antiAliased );

		// Accessors
		std::wstring FontName() const;
		float Size() const;
		UINT FontStyle() const;
		bool AntiAliased() const;

		const CharDesc* CharDescriptors() const;
		const CharDesc& GetCharDescriptor(WCHAR character) const;
		ResourcePtr TextureResource() const;
		UINT TextureWidth() const;
		UINT TextureHeight() const;
		float SpaceWidth() const;
		float CharHeight() const;

		float GetStringWidth( const std::wstring& line );

	protected:
		std::wstring m_FontName;
		float m_fSize;
		UINT m_uiFontStyle;
		bool m_bAntiAliased;

		ResourcePtr m_pTexture;
		CharDesc m_CharDescs [NumChars];
		UINT m_uTexHeight;
		float m_fSpaceWidth;
		float m_fCharHeight;
	};

	typedef std::shared_ptr<SpriteFontDX11> SpriteFontPtr;
}

//--------------------------------------------------------------------------------
#endif // SpriteFontDX11_h
//--------------------------------------------------------------------------------