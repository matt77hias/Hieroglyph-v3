//--------------------------------------------------------------------------------
#include "TextEntity3D.h"
#include "Log.h"
#include <sstream>
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
TextEntity3D::TextEntity3D()
{
	m_sText = "";
	m_vLocation.MakeZero();
	m_vSize.MakeZero();
}
//--------------------------------------------------------------------------------
TextEntity3D::~TextEntity3D()
{
}
//--------------------------------------------------------------------------------
void TextEntity3D::Update( float time )
{
	UpdateLocal( time );
	UpdateWorld( );

	m_sParams.WorldMatrix = m_mWorld;
}
//--------------------------------------------------------------------------------
void TextEntity3D::UpdateLocal( float time )
{
	for ( int i = 0; i < m_Controllers.count(); i++ )
		m_Controllers[i]->Update( time );

	m_mLocal.MakeIdentity( );
	m_mLocal.SetRotation( m_mRotation );
	m_mLocal.SetTranslation( m_vTranslation );
}
//--------------------------------------------------------------------------------
void TextEntity3D::UpdateWorld( )
{
	if (m_pParent)
        m_mWorld = m_mLocal * m_pParent->WorldMatrix();
	else
		m_mWorld = m_mLocal;

	// Update bounding sphere with the new world space position and orientation.
	Vector3f center = m_ModelBoundingSphere.Center;
	Vector4f modelposition = Vector4f( center.x, center.y, center.z, 1.0f );
	Vector4f worldposition = m_mWorld * modelposition;
	m_WorldBoundingSphere.Center = Vector3f( worldposition.x, worldposition.y, worldposition.z );
}
//--------------------------------------------------------------------------------
void TextEntity3D::PreRender( RendererDX11& Renderer, VIEWTYPE view )
{
	// Perform the pre-render function only if the material has been set
	//if ( m_pMaterial )
	//	m_pMaterial->PreRender( Renderer, view );
}
//--------------------------------------------------------------------------------
void TextEntity3D::Render( RendererDX11& Renderer, VIEWTYPE view )
{
	// Test if the entity contains any text
	if ( ( m_sText != "" ) && ( view == VT_GUI_SKIN ) )
	{
		// TODO: Project coordinates to screen space and set location based on that.
		//       This could also be done in the update phase too...

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
void TextEntity3D::SetLocation( Vector2f& location )
{
	m_vLocation = location;
}
//--------------------------------------------------------------------------------
void TextEntity3D::SetSize( Vector2f& size )
{
	m_vSize = size;
}
//--------------------------------------------------------------------------------
Vector2f TextEntity3D::GetLocation( )
{
	return( m_vLocation );
}
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
void TextEntity3D::SetText( std::string text )
{
	m_sText = text;
}
//--------------------------------------------------------------------------------
std::string TextEntity3D::GetText( )
{
	return( m_sText );
}
//--------------------------------------------------------------------------------