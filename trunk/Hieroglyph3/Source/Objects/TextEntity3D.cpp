//--------------------------------------------------------------------------------
#include "PCH.h"
#include "TextEntity3D.h"
#include "Log.h"
#include <sstream>
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
TextEntity3D::TextEntity3D()
{
	m_sText = L"";
	//m_vLocation.MakeZero();
	m_vSize.MakeZero();
	m_vColor = Vector4f( 1.0f, 1.0f, 1.0f, 1.0f );

	// Create the text rendering classes.
	m_pSpriteFont = new SpriteFontDX11();
	m_pSpriteFont->Initialize( L"Consolas", 12.0f, 0, false );
	
	m_pSpriteRenderer = new SpriteRendererDX11();
	m_pSpriteRenderer->Initialize();
}
//--------------------------------------------------------------------------------
TextEntity3D::~TextEntity3D()
{
	SAFE_DELETE( m_pSpriteFont );
	SAFE_DELETE( m_pSpriteRenderer );
}
//--------------------------------------------------------------------------------
void TextEntity3D::PreRender( RendererDX11* pRenderer, VIEWTYPE view )
{
	// Perform the pre-render function only if the material has been set
	//if ( m_pMaterial )
	//	m_pMaterial->PreRender( Renderer, view );
}
//--------------------------------------------------------------------------------
void TextEntity3D::Render( PipelineManagerDX11* pPipelineManager, IParameterManager* pParamManager, VIEWTYPE view )
{
	// Test if the entity contains any text
	//if ( m_sText != "" )
	{
		// TODO: Project coordinates to screen space and set location based on that.
		//       This could also be done in the update phase too...

		m_pSpriteRenderer->RenderText( pPipelineManager, pParamManager, *m_pSpriteFont, m_sText.c_str(), Matrix4f::Identity(), m_vColor );
		//Renderer.DrawTextA( m_iFont, (int)m_vLocation.x, (int)m_vLocation.y, (int)m_vSize.x, (int)m_vSize.y, m_vColor.toARGB(), m_sText );
	}
}
//--------------------------------------------------------------------------------
std::string TextEntity3D::toString( )
{
	std::stringstream objString;

	for ( int i = 0; i < GraphDepth(); i++ )
	{
		if ( i < GraphDepth() - 1 )
			objString << "| ";
		else
			objString << "+-";
	}

	objString << "TextEntity3D : ID : " << m_iEntityID << "\n";

	return( objString.str() );
}
//--------------------------------------------------------------------------------
//void TextEntity3D::SetLocation( Vector2f& location )
//{
//	m_vLocation = location;
//}
//--------------------------------------------------------------------------------
void TextEntity3D::SetSize( Vector2f& size )
{
	m_vSize = size;
}
//--------------------------------------------------------------------------------
//Vector2f TextEntity3D::GetLocation( )
//{
//	return( m_vLocation );
//}
//--------------------------------------------------------------------------------
Vector2f TextEntity3D::GetSize( )
{
	return( m_vSize );
}
//--------------------------------------------------------------------------------
void TextEntity3D::SetFont( int font )
{
	m_iFont = font;
}
//--------------------------------------------------------------------------------
int TextEntity3D::GetFont( )
{
	return( m_iFont );
}
//--------------------------------------------------------------------------------
void TextEntity3D::SetColor( Vector4f color )
{
	m_vColor = color;
}
//--------------------------------------------------------------------------------
Vector4f TextEntity3D::GetColor( )
{
	return( m_vColor );
}
//--------------------------------------------------------------------------------
void TextEntity3D::BuildPickRecord( Ray3f& ray, TArray<PickRecord>& record )
{
	// Do nothing - the text should not intersect a pick ray.
}
//--------------------------------------------------------------------------------
void TextEntity3D::SetText( std::wstring text )
{
	m_sText = text;
}
//--------------------------------------------------------------------------------
std::wstring TextEntity3D::GetText( )
{
	return( m_sText );
}
//--------------------------------------------------------------------------------