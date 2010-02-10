//--------------------------------------------------------------------------------
// TextEntity2D
//
// The 2D version of the text entity should be used to render text on a 2D 
// location of a render target.  This would typically correspond to using GDI to
// draw regular 2D controls.  
//
// The intended usage of these entities is to draw scene level text on the main
// render target, as well as to render text onto a dynamic texture for skinning
// a 3D GUI.  Due to the 2D nature of the entity, the material should be used to
// specify if the text should be rendered for a given view type.
//
// Copyright (C) 2003-2008 Jason Zink
//--------------------------------------------------------------------------------
#ifndef TextEntity2D_h
#define TextEntity2D_h
//--------------------------------------------------------------------------------
#include "Entity3D.h"
#include "Vector2f.h"
//--------------------------------------------------------------------------------
namespace Glyph3
{
	class TextEntity2D : public Entity3D
	{
	public:
		TextEntity2D( );
		virtual ~TextEntity2D( );

		virtual void Update( float time );
		virtual void UpdateLocal( float fTime );
		virtual void UpdateWorld( );

		virtual void PreRender( RendererDX11& Renderer, VIEWTYPE view );
		virtual void Render( RendererDX11& Renderer, VIEWTYPE view );

		virtual std::string toString( );

		void SetText( std::string text );
		std::string GetText( );

		void SetLocation( Vector2f& location );
		void SetSize( Vector2f& size );

		Vector2f GetLocation( );
		Vector2f GetSize( );

		void SetFont( int font );
		int GetFont( );

		void SetColor( Vector4f color );
		Vector4f GetColor( );

		virtual void BuildPickRecord( Ray3f& ray, TArray<PickRecord>& record );

	protected:
		std::string m_sText;
		Vector2f	m_vLocation;
		Vector2f	m_vSize;
		Vector4f	m_vColor;
		int			m_iFont;
	};
};
//--------------------------------------------------------------------------------
#endif // TextEntity2D_h
