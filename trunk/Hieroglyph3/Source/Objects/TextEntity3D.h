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
#include "PCH.h"
#include "Entity3D.h"
#include "Vector2f.h"
#include "SpriteFontDX11.h"
#include "SpriteRendererDX11.h"
//--------------------------------------------------------------------------------
namespace Glyph3
{
	class TextEntity3D : public Entity3D
	{
	public:
		TextEntity3D( );
		virtual ~TextEntity3D( );

		virtual void PreRender( RendererDX11* pRenderer, VIEWTYPE view );
		virtual void Render( PipelineManagerDX11* pPipelineManager, IParameterManager* pParamManager, VIEWTYPE view );

		virtual std::string toString( );

		void SetText( std::wstring text );
		std::wstring GetText( );

		//void SetLocation( Vector2f& location );
		//Vector2f GetLocation( );

		void SetSize( Vector2f& size );
		Vector2f GetSize( );

		void SetFont( int font );
		int GetFont( );

		void SetColor( Vector4f color );
		Vector4f GetColor( );

		virtual void BuildPickRecord( Ray3f& ray, TArray<PickRecord>& record );

	protected:
		std::wstring	m_sText;
		//Vector2f		m_vLocation;
		Vector2f		m_vSize;
		Vector4f		m_vColor;
		int				m_iFont;

		SpriteFontDX11*			m_pSpriteFont;
		SpriteRendererDX11*		m_pSpriteRenderer;
	};
};
//--------------------------------------------------------------------------------
#endif // TextEntity3D_h
