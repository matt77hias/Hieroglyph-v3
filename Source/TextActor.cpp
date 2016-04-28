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
#include "TextActor.h"
#include "MaterialGeneratorDX11.h"
#include "ShaderResourceParameterWriterDX11.h"
#include "RasterizerStateConfigDX11.h"
#include "BlendStateConfigDX11.h"
#include "DepthStencilStateConfigDX11.h"
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
TextActor::TextActor()
	: m_sText(),
      m_Origin( 0.0f, 0.0f, 0.0f ),
	  m_LineStart( 0.0f, 0.0f, 0.0f ),
      m_Cursor( 0.0f, 0.0f, 0.0f ),
	  m_xdir( 1.0f, 0.0f, 0.0f ),
	  m_ydir( 0.0f, 1.0f, 0.0f ),
	  m_pSpriteFont( nullptr ),
	  m_fCharacterHeight( 0.8f ),
	  m_fPhysicalScale( m_fCharacterHeight / 20.0f ),
	  m_fTextureXScale( 0.1f ),
	  m_fTextureYScale( 0.1f ),
	  m_TextReference( TextOriginReference::TOP ),
	  m_LineJustification( LineJustification::LEFT )
{
	RendererDX11* pRenderer = RendererDX11::Get();

	m_pGeometry = IndexedImmediateGeometryPtr( new DrawIndexedExecutorDX11<BasicVertexDX11::Vertex>() );
	m_pGeometry->SetPrimitiveType( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
	m_pGeometry->SetLayoutElements( BasicVertexDX11::GetElementCount(), BasicVertexDX11::Elements );

	m_pMaterial = MaterialGeneratorDX11::GenerateTextMaterial( *pRenderer );

	SetColor( Vector4f( 1.0f, 1.0f, 1.0f, 1.0f ) );


	GetBody()->Visual.SetGeometry( m_pGeometry );
	GetBody()->Visual.SetMaterial( m_pMaterial );

	SpriteFontPtr pSpriteFont = SpriteFontLoaderDX11::LoadFont( std::wstring( L"Consolas" ), 40.0f, 0, true );
	SetFont( pSpriteFont );
}
//--------------------------------------------------------------------------------
TextActor::~TextActor()
{
}
//--------------------------------------------------------------------------------
void TextActor::ClearText()
{
	// Reset the text and the geometry to ensure there are no existing 
	// characters in the buffer for the next text addition.

	m_sText = L"";
	m_pGeometry->ResetGeometry();
	ResetCursor();
}
//--------------------------------------------------------------------------------
void TextActor::SetText( const std::wstring& text )
{
	// Clear out the buffers, then add the needed geometry to the buffers to 
	// represent this text.

	ClearText();
	DrawString( text );

	m_sText = text;
}
//--------------------------------------------------------------------------------
void TextActor::AppendText( const std::wstring& text )
{
	// Add the needed geometry to the buffers to represent the new text.

	DrawString( text );

	m_sText += text;
}
//--------------------------------------------------------------------------------
void TextActor::SetColor( const Vector4f& color )
{
	m_Color = color;
}
//--------------------------------------------------------------------------------
Vector4f TextActor::GetColor()
{
	return( m_Color );
}
//--------------------------------------------------------------------------------
void TextActor::SetTextOrigin( const Vector3f& location )
{
	// Setting the origin is like setting the top left side of a document.  This
	// makes the line start location as well as the cursor location default to
	// the origin.

	//m_Origin = location;

	switch ( m_TextReference )
	{
	case TextOriginReference::TOP:
		m_Origin = location;
		break;
	case TextOriginReference::MIDDLE:
		m_Origin = location + m_ydir * m_fCharacterHeight * 0.5f;
		break;
	case TextOriginReference::BOTTOM:
		m_Origin = location + m_ydir * m_fCharacterHeight * 1.0f;
		break;
	}

	m_LineStart = m_Origin;
	m_Cursor = m_LineStart;
}
//--------------------------------------------------------------------------------
void TextActor::SetTextOrientation( const Vector3f& xdir, const Vector3f& ydir )
{
	m_xdir = xdir;
	m_ydir = ydir;
}
//--------------------------------------------------------------------------------
void TextActor::AdvanceCursor( float distance )
{
	// Advance the cursor based on the current xdir vector

	m_Cursor = m_Cursor + m_xdir * distance;
}
//--------------------------------------------------------------------------------
void TextActor::Space()
{
	// Advance the cursor based on the current xdir vector

	AdvanceCursor( m_pSpriteFont->SpaceWidth() * m_fPhysicalScale );
}
//--------------------------------------------------------------------------------
void TextActor::NewLine()
{
	// For a line return, we go back to zero for the horizontal location, and 
	// advance downward by the scaled size of the character set height.

	m_LineStart = m_LineStart - m_ydir * m_pSpriteFont->CharHeight() * m_fPhysicalScale;
	m_Cursor = m_LineStart;
}
//--------------------------------------------------------------------------------
void TextActor::ResetCursor()
{
	// For a line return, we go back to zero for the horizontal location, and 
	// advance downward by the scaled size of the character set height.

	m_Cursor = m_Origin;
	m_LineStart = m_Origin;
}
//--------------------------------------------------------------------------------
void TextActor::AddVertex( const Vector3f& position, const Vector2f& texcoords )
{
	BasicVertexDX11::Vertex vertex;

	vertex.position = position;
	vertex.normal = Vector3f( 0.0f, 1.0f, 0.0f );
	vertex.color = m_Color;
	vertex.texcoords = texcoords;
	
	m_pGeometry->AddVertex( vertex );
}
//--------------------------------------------------------------------------------
void TextActor::DrawString( const std::wstring& text )
{
	std::wstringstream			tempStream( text );
	std::wstring				line;


	while ( std::getline( tempStream, line ) ) 
	{
		DrawLine( line );
		if ( !tempStream.eof() )
			NewLine();
	}

}
//--------------------------------------------------------------------------------
void TextActor::DrawLine( const std::wstring& text )
{
	// Check the length of the string, and use the line justification to advance 
	// the cursor an appropriate amount before actually drawing the line of text.

	float fWidth = m_pSpriteFont->GetStringWidth( text ) * m_fPhysicalScale;

	switch( m_LineJustification )
	{
	case LineJustification::LEFT:
		// No change needed - just draw the text from teh current cursor position
		break;
	case LineJustification::CENTER:
		// Advance cursor to the 'left' by half the string width
		AdvanceCursor( -fWidth * 0.5f );
		break;
	case LineJustification::RIGHT:
		// Advance the cursor by the full size of the string.
		AdvanceCursor( -fWidth );
	}


	for ( UINT i = 0; i < text.length(); i++ )
	{
		wchar_t character = text[i];

		if ( character == ' ' ) {
			
			// For a space, we simply move over one space's width for the next
			// character.
			Space();

		} else if ( character == '\n' ) {
			
			// Go back to the original location on xdir, and advance along ydir.
			NewLine();

		} else {
			
			// If this is an actual character, then draw it!
			DrawCharacter( character );

		}
	}

	//NewLine();
}
//--------------------------------------------------------------------------------
void TextActor::DrawCharacter( const wchar_t& character )
{
	// Get the character description in a local variable for quick access.
	SpriteFontDX11::CharDesc desc = m_pSpriteFont->GetCharDescriptor(character);

	Vector3f p;
	Vector2f t;

	// Grap the base vertex index for this quad.  This will update after
	// each character's vertices are added, so we need to cache the value here.
	unsigned int baseVertex = m_pGeometry->GetVertexCount();

	// TODO: The texture scale multiply below should not be necessary.  This should
	//       be calculated once and stored in the sprite font class, and then just
	//       looked up at runtime.  It would save a little computation, which may
	//       be important for large amounts of text.

	// Top left vertex
	p = m_Cursor;
	t.x = ( desc.X ) * m_fTextureXScale;
	t.y = ( desc.Y ) * m_fTextureYScale;
	AddVertex( p, t );

	// Top right vertex
	p = m_Cursor + m_xdir * desc.Width * m_fPhysicalScale; 
	t.x = ( desc.X + desc.Width ) * m_fTextureXScale;
	t.y = ( desc.Y ) * m_fTextureYScale;
	AddVertex( p, t );

	// Bottom left vertex
	p = m_Cursor - m_ydir * desc.Height * m_fPhysicalScale;
	t.x = ( desc.X ) * m_fTextureXScale;
	t.y = ( desc.Y + desc.Height ) * m_fTextureYScale;
	AddVertex( p, t );

	// Bottom right vertex
	p = m_Cursor + ( m_xdir * desc.Width - m_ydir * desc.Height ) * m_fPhysicalScale;
	t.x = ( desc.X + desc.Width ) * m_fTextureXScale;
	t.y = ( desc.Y + desc.Height ) * m_fTextureYScale;
	AddVertex( p, t );

	// Add the indices to link the vertices together.  Here we simply
	// use two triangles, which have three vertex indices each.
	m_pGeometry->AddIndices( baseVertex+0, baseVertex+1, baseVertex+2 );
	m_pGeometry->AddIndices( baseVertex+3, baseVertex+2, baseVertex+1 );

	// Advance to the next character and the subsequent next location.
	AdvanceCursor( (desc.Width+1) * m_fPhysicalScale );
}
//--------------------------------------------------------------------------------
void TextActor::SetFont( SpriteFontPtr pFont )
{
	m_pSpriteFont = pFont;
	
	// Update the material parameters to account for the new font's texture.
	m_pMaterial->Parameters.SetShaderResourceParameter( L"ColorTexture", m_pSpriteFont->TextureResource() );

	// Update the physical scaling with the new sprite's character height.
	SetCharacterHeight( m_fCharacterHeight );


	// Cache the scale factors to be used on the texture coordinates.

	if ( m_pSpriteFont->TextureWidth() != 0.0f ) {
		m_fTextureXScale = 1.0f / m_pSpriteFont->TextureWidth();
	} else {
		m_fTextureXScale = 0.1f;
	}

	if ( m_pSpriteFont->TextureHeight() != 0.0f ) {
		m_fTextureYScale = 1.0f / m_pSpriteFont->TextureHeight();
	} else {
		m_fTextureYScale = 0.0f;
	}

	// After setting a new font, we should regenerate the geometry.  First clear
	// the geometry and reset the cursor, then regenerate the contents of the
	// geometry by drawing into it.

	m_pGeometry->ResetGeometry();
	ResetCursor();
	DrawString( m_sText );
}
//--------------------------------------------------------------------------------
void TextActor::SetCharacterHeight( float height )
{
	m_fCharacterHeight = height;
	m_fPhysicalScale = m_fCharacterHeight / m_pSpriteFont->CharHeight();
}
//--------------------------------------------------------------------------------
void TextActor::SetTextLineReference( TextOriginReference reference )
{
	m_TextReference = reference;
}
//--------------------------------------------------------------------------------
void TextActor::SetLineJustification( LineJustification justification )
{
	m_LineJustification = justification;
}
//--------------------------------------------------------------------------------
