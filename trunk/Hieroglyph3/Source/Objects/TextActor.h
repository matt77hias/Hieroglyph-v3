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
// TextActor
//
// This class provides a way to generate 3D geometry to represent text.  This is
// accomplished through the use of the sprite font classes to generate a texture
// of glyphs, which is the referenced by quads of vertices to texture map the 
// characters.  The geometry generation is independent of the actual rendering
// of this actor, so if your text doesn't change then you only have to upload
// the data once - which should make for very efficient text rendering.
//
// The text is generated in object space according to the specified origin and
// orientation.  This allows for slanted text, scaling (by using non-normalized
// orientation vectors), and otherwise specially located text drawing routines if
// needed.
//
// The actual rendering is alpha blended, so that only the characters appear in 
// the final rendering (i.e. no background on the letters).  The color setting
// will also affect the alpha, so semi-transparent text is also possible.
//--------------------------------------------------------------------------------
#ifndef TextActor_h
#define TextActor_h
//--------------------------------------------------------------------------------
#include "Actor.h"
#include "DrawIndexedExecutorDX11.h"
#include "SpriteFontDX11.h"
#include "BasicVertexDX11.h"
//--------------------------------------------------------------------------------
namespace Glyph3
{
	typedef std::shared_ptr<DrawIndexedExecutorDX11<BasicVertexDX11::Vertex>> IndexedImmediateGeometryPtr;

	class TextActor : public Actor
	{
	public:
		TextActor();
		virtual ~TextActor();

		// Use these methods for manipulating the text drawn by this class.
		// Clearing the text will eliminate all of the currently drawn text. 
		// Setting the text will clear the buffers, then draw the text. Finally
		// appending text will simply add the text to the existing text, without
		
		void ClearText();
		void SetText( const std::wstring& text );
		void AppendText( const std::wstring& text );
		// TODO: possibly add a based stream function here...

		// These methods provide the ability to draw individual characters, 
		// advance the 'cursor' with a space, or start a new line of text.

		void DrawCharacter( const wchar_t& character );
		void DrawString( const std::wstring& text );
		void AdvanceCursor( float distance );
		void Space();
		void NewLine();

		// Manipulation of the text color can be performed through these methods.

		void SetColor( const Vector4f& color );
		Vector4f GetColor();

		// The position of the text is controlled through these methods.  The 
		// origin provides a starting point for all text drawing, while the 
		// orientation vectors provide the vertical and horizontal directions
		// to use for identifying the plane of the 'paper'.

		void SetTextOrigin( const Vector3f& origin );
		void SetTextOrientation( const Vector3f& xdir, const Vector3f& ydir );

		// The font to use for the drawing of the text goes here.

		void SetFont( SpriteFontDX11* pFont );
		void SetCharacterHeight( float scale );
		
	private:
		void AddVertex( const Vector3f& position, const Vector2f& texcoords );

	protected:
		
		std::wstring							m_sText;

		Vector3f								m_Origin;
		Vector3f								m_LineStart;
		Vector3f								m_Cursor;
		Vector3f								m_xdir;
		Vector3f								m_ydir;

		Vector4f								m_Color;

		IndexedImmediateGeometryPtr				m_pGeometry;
		MaterialPtr								m_pMaterial;

		SpriteFontDX11*							m_pSpriteFont;
		float									m_fCharacterHeight;
		float									m_fPhysicalScale;
		float									m_fTextureXScale;
		float									m_fTextureYScale;
	};
};
//--------------------------------------------------------------------------------
#endif // TextActor_h
