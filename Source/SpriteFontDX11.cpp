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
#include "SpriteFontDX11.h"
#include "Texture2DConfigDX11.h"
//--------------------------------------------------------------------------------
using namespace Glyph3;
using namespace Gdiplus;
//--------------------------------------------------------------------------------
SpriteFontDX11::SpriteFontDX11() :  
	m_FontName( L"" ),
	m_fSize( 0 ),
	m_uiFontStyle( 0 ),
	m_bAntiAliased( false ),
	m_uTexHeight( 0 ),
	m_fSpaceWidth( 0 ),
	m_fCharHeight( 0 )
{

}
//--------------------------------------------------------------------------------
SpriteFontDX11::~SpriteFontDX11()
{

}
//--------------------------------------------------------------------------------
bool SpriteFontDX11::Initialize( std::wstring& fontName, float fontSize, UINT fontStyle, bool antiAliased )
{
	m_FontName = fontName;
	m_fSize = fontSize;
	m_uiFontStyle = fontStyle;
	m_bAntiAliased = antiAliased;

	TextRenderingHint hint = antiAliased ? TextRenderingHintAntiAliasGridFit : TextRenderingHintSingleBitPerPixelGridFit;

	// Init GDI+
	ULONG_PTR token = NULL;
	GdiplusStartupInput startupInput (NULL, TRUE, TRUE);
	GdiplusStartupOutput startupOutput;
	GdiPlusCall( GdiplusStartup( &token, &startupInput, &startupOutput ) );

	// Create the font
	Gdiplus::Font font( m_FontName.c_str(), fontSize, fontStyle, UnitPixel, NULL );

	// Check for error during construction
	GdiPlusCall( font.GetLastStatus() );

	// Create a temporary Bitmap and Graphics for figuring out the rough size required
	// for drawing all of the characters
	int size = static_cast<int>( fontSize * NumChars * 2 ) + 1;
	Bitmap sizeBitmap( size, size, PixelFormat32bppARGB );
	GdiPlusCall( sizeBitmap.GetLastStatus() );

	Graphics sizeGraphics( &sizeBitmap );
	GdiPlusCall( sizeGraphics.GetLastStatus() );
	GdiPlusCall( sizeGraphics.SetTextRenderingHint( hint ) );

	m_fCharHeight = font.GetHeight(&sizeGraphics) * 1.0f;

	WCHAR allChars[NumChars + 1];
	for( WCHAR i = 0; i < NumChars; ++i )
		allChars[i] = i + StartChar;
	allChars[NumChars] = 0;

	RectF sizeRect;
	GdiPlusCall( sizeGraphics.MeasureString( allChars, NumChars, &font, PointF( 0, 0 ), &sizeRect ) );
	int numRows = static_cast<int>( sizeRect.Width / TexWidth ) + 1;
	m_uTexHeight = static_cast<int>( numRows * m_fCharHeight ) + 1;

	// Create a temporary Bitmap and Graphics for drawing the characters one by one
	int tempSize = static_cast<int>( fontSize * 2 );
	Bitmap drawBitmap( tempSize, tempSize, PixelFormat32bppARGB );
	GdiPlusCall( drawBitmap.GetLastStatus());

	Graphics drawGraphics( &drawBitmap );
	GdiPlusCall( drawGraphics.GetLastStatus() );
	GdiPlusCall( drawGraphics.SetTextRenderingHint( hint ) );

	// Create a temporary Bitmap + Graphics for creating a full character set
	Bitmap textBitmap ( TexWidth, m_uTexHeight, PixelFormat32bppARGB );
	GdiPlusCall( textBitmap.GetLastStatus() );

	Graphics textGraphics ( &textBitmap );	GdiPlusCall( textGraphics.GetLastStatus() );
	GdiPlusCall( textGraphics.Clear( Color( 0, 255, 255, 255 ) ) );
	GdiPlusCall( textGraphics.SetCompositingMode( CompositingModeSourceCopy ) );

	// Solid brush for text rendering
	SolidBrush brush ( Color( 255, 255, 255, 255 ) );
	GdiPlusCall( brush.GetLastStatus() );

	// Draw all of the characters, and copy them to the full character set
	WCHAR charString [2];
	charString[1] = 0;
	int currentX = 0;
	int currentY = 0;
	for(UINT64 i = 0; i < NumChars; ++i)
	{
		charString[0] = static_cast<WCHAR>(i + StartChar);

		// Draw the character
		GdiPlusCall( drawGraphics.Clear( Color( 0, 255, 255, 255 ) ) );
		GdiPlusCall( drawGraphics.DrawString( charString, 1, &font, PointF( 0, 0 ), &brush ) );

		// Figure out the amount of blank space before the character
		int minX = 0;
		for( int x = 0; x < tempSize; ++x )
		{
			for( int y = 0; y < tempSize; ++y )
			{
				Color color;
				GdiPlusCall( drawBitmap.GetPixel( x, y, &color ) );
				if( color.GetAlpha() > 0 )
				{
					minX = x;
					x = tempSize;
					break;
				}
			}
		}

		// Figure out the amount of blank space after the character
		int maxX = tempSize - 1;
		for( int x = tempSize - 1; x >= 0; --x )
		{
			for( int y = 0; y < tempSize; ++y )
			{
				Color color;
				GdiPlusCall( drawBitmap.GetPixel( x, y, &color ) );
				if( color.GetAlpha() > 0 )
				{
					maxX = x;
					x = -1;
					break;
				}
			}
		}

		int charWidth = maxX - minX + 1;

		// Figure out if we need to move to the next row
		if ( currentX + charWidth >= TexWidth )
		{
			currentX = 0;
			currentY += static_cast<int>( m_fCharHeight ) + 1;
		}

		// Fill out the structure describing the character position
		m_CharDescs[i].X = static_cast<float>( currentX );
		m_CharDescs[i].Y = static_cast<float>( currentY );
		m_CharDescs[i].Width = static_cast<float>( charWidth );
		m_CharDescs[i].Height = static_cast<float>( m_fCharHeight );

		// Copy the character over
		int height = static_cast<int>( m_fCharHeight + 1 );
		GdiPlusCall(textGraphics.DrawImage( &drawBitmap, currentX, currentY, minX, 0, charWidth, height, UnitPixel ) );

		currentX += charWidth + 1;
	}

	// Figure out the width of a space character
	charString[0] = ' ';
	charString[1] = 0;
	GdiPlusCall(drawGraphics.MeasureString( charString, 1, &font, PointF( 0, 0 ), &sizeRect ) );
	m_fSpaceWidth = sizeRect.Width;

	// Lock the bitmap for direct memory access
	BitmapData bmData;
	GdiPlusCall(textBitmap.LockBits( &Rect( 0, 0, TexWidth, m_uTexHeight ), ImageLockModeRead, PixelFormat32bppARGB, &bmData ) );

	// Create a D3D texture, initalized with the bitmap data
	Texture2dConfigDX11 config;
	config.SetBindFlags( D3D11_BIND_SHADER_RESOURCE );
	config.SetFormat( DXGI_FORMAT_B8G8R8A8_UNORM );
	config.SetUsage( D3D11_USAGE_IMMUTABLE );
	config.SetWidth( TexWidth );
	config.SetHeight( m_uTexHeight );

	D3D11_SUBRESOURCE_DATA data;
	data.pSysMem = bmData.Scan0;
	data.SysMemPitch = TexWidth * 4;
	data.SysMemSlicePitch = 0;

	// Get the renderer
	RendererDX11* renderer = RendererDX11::Get();

	// Create the texture
	m_pTexture = renderer->CreateTexture2D( &config, &data );

	// Unlock the bitmap
	GdiPlusCall( textBitmap.UnlockBits( &bmData ) );

	return true;
}
//--------------------------------------------------------------------------------
const SpriteFontDX11::CharDesc* SpriteFontDX11::CharDescriptors() const
{
	return m_CharDescs;
}
//--------------------------------------------------------------------------------
const SpriteFontDX11::CharDesc& SpriteFontDX11::GetCharDescriptor(WCHAR character) const
{
	_ASSERT(character >= StartChar && character <= EndChar);
	return m_CharDescs[character - StartChar];
}
//--------------------------------------------------------------------------------
std::wstring SpriteFontDX11::FontName() const
{
	return( m_FontName );
}
//--------------------------------------------------------------------------------
float SpriteFontDX11::Size() const
{
	return( m_fSize );
}
//--------------------------------------------------------------------------------
UINT SpriteFontDX11::FontStyle() const
{
	return( m_uiFontStyle );
}
//--------------------------------------------------------------------------------
bool SpriteFontDX11::AntiAliased() const
{
	return( m_bAntiAliased );
}
//--------------------------------------------------------------------------------
UINT SpriteFontDX11::TextureWidth() const
{
	return TexWidth;
}
//--------------------------------------------------------------------------------
UINT SpriteFontDX11::TextureHeight() const
{
	return m_uTexHeight;
}
//--------------------------------------------------------------------------------
float SpriteFontDX11::SpaceWidth() const
{
	return m_fSpaceWidth;
}
//--------------------------------------------------------------------------------
float SpriteFontDX11::CharHeight() const
{
	return m_fCharHeight;
}
//--------------------------------------------------------------------------------
ResourcePtr SpriteFontDX11::TextureResource() const
{
	return m_pTexture;
}
//--------------------------------------------------------------------------------
float SpriteFontDX11::GetStringWidth( const std::wstring& text )
{
	float fWidth = 0.0f;

	for ( UINT i = 0; i < text.length(); i++ )
	{
		wchar_t character = text[i];

		if ( character == ' ' ) 
		{
			// Add the width of one space.
			fWidth += SpaceWidth();
		} 
		else if ( character == '\n' )
		{
			// Do nothing - don't count a newline in the width calculation.
		} 
		else 
		{
			// Lookup the character's width and add it to the tally.
			fWidth += this->GetCharDescriptor( character ).Width;
		}
	}

	return( fWidth );
}
//--------------------------------------------------------------------------------
