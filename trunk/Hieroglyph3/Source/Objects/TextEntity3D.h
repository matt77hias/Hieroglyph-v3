//--------------------------------------------------------------------------------
// TextEntity3D
//
// The 3D version of the text entity is intended to render text in a specific 
// location in the 3D scene.  The position to render the text to is calculated
// by projecting its position to clip space and then changing it to screen 
// coordinates.
//
// Copyright (C) 2003-2008 Jason Zink
//--------------------------------------------------------------------------------
#ifndef TextEntity3D_h
#define TextEntity3D_h
//--------------------------------------------------------------------------------
#include "Entity3D.h"
#include "Vector2f.h"
//--------------------------------------------------------------------------------
namespace Glyph3
{
	class TextEntity3D : public Entity3D
	{
	public:
		TextEntity3D( );
		virtual ~TextEntity3D( );

		virtual void Update( float time );
		virtual void UpdateLocal( float time );
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
#endif // TextEntity3D_h
