//--------------------------------------------------------------------------------
#include "TextEntity2D.h"
#include "Log.h"
#include <sstream>
//#include "TextController.h"
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
TextEntity2D::TextEntity2D()
{
	m_sText = "";
	m_vLocation.MakeZero();
	m_vSize.MakeZero();

	//// Hack here to test out the text controller...
	//TextController* pTextController = new TextController();
	//
	//TextState states[4];
	//
	//states[0].fDuration = 2.0f;
	//states[0].fTimeLeft = 2.0f;
	//states[0].vColor = CVector4f( 1.0f, 0.0f, 0.0f, 1.0f );
	//states[0].vPosition = CVector2f( 2.0f, 2.0f );
	//states[0].vSize = CVector2f( 256.0f, 256.0f );

	//states[1].fDuration = 2.0f;
	//states[1].fTimeLeft = 2.0f;
	//states[1].vColor = CVector4f( 0.0f, 1.0f, 0.0f, 1.0f );
	//states[1].vPosition = CVector2f( 200.0f, 200.0f );
	//states[1].vSize = CVector2f( 256.0f, 256.0f );

	//states[2].fDuration = 2.0f;
	//states[2].fTimeLeft = 2.0f;
	//states[2].vColor = CVector4f( 0.0f, 0.0f, 1.0f, 1.0f );
	//states[2].vPosition = CVector2f( 2.0f, 100.0f );
	//states[2].vSize = CVector2f( 256.0f, 256.0f );

	//states[3].fDuration = 2.0f;
	//states[3].fTimeLeft = 2.0f;
	//states[3].vColor = CVector4f( 1.0f, 1.0f, 0.0f, 1.0f );
	//states[3].vPosition = CVector2f( 2.0f, 2.0f );
	//states[3].vSize = CVector2f( 256.0f, 256.0f );

	//for ( int i = 0; i < 4; i++ )
	//	pTextController->AddState( states[i] );

	//this->AttachController( pTextController );

}
//--------------------------------------------------------------------------------
TextEntity2D::~TextEntity2D()
{
}
//--------------------------------------------------------------------------------
void TextEntity2D::Update( float time )
{
	UpdateLocal( time );
	UpdateWorld( );

	m_sParams.WorldMatrix = m_mWorld;
}
//--------------------------------------------------------------------------------
void TextEntity2D::UpdateLocal( float fTime )
{
	for ( int i = 0; i < m_Controllers.count(); i++ )
		m_Controllers[i]->Update( fTime );

	m_mLocal.MakeIdentity( );
	m_mLocal.SetRotation( m_mRotation );
	m_mLocal.SetTranslation( m_vTranslation );
}
//--------------------------------------------------------------------------------
void TextEntity2D::UpdateWorld( )
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
void TextEntity2D::PreRender( RendererDX11& Renderer, VIEWTYPE view )
{
	// Perform the pre-render function only if the material has been set
	//if ( m_pMaterial )
	//	m_pMaterial->PreRender( Renderer, view );
}
//--------------------------------------------------------------------------------
void TextEntity2D::Render( RendererDX11& Renderer, VIEWTYPE view )
{
	// Test if the entity contains any geometry, and it has a material
	if ( ( m_sText != "" ) && ( m_pMaterial ) )
	{
		// Only render if the material indicates that you should
		//if ( m_pMaterial->Params[view].bRender )
		{
			// Note: 6/24/2009 J.Z.
			// This draw text call originally had the material's first parameter
			// as the color.  This was probably due to the XML based materials 
			// lack of data description abilities, which is no longer needed with
			// the Lua based materials.  Now it will use the text entity color 
			// instead...

			//Renderer.DrawTextA( m_iFont, (int)m_vLocation.x(), (int)m_vLocation.y(), 
			//	(int)m_vSize.x(), (int)m_vSize.y(), m_vColor.toARGB(), m_sText );
		}
	}
}
//--------------------------------------------------------------------------------
std::string TextEntity2D::toString( )
{
	std::stringstream objString;

	for ( int i = 0; i < GraphDepth(); i++ )
	{
		if ( i < GraphDepth() - 1 )
			objString << "| ";
		else
			objString << "+-";
	}

	objString << "TextEntity2D : ID : " << m_iEntityID << "\n";

	return( objString.str() );
}
//--------------------------------------------------------------------------------
void TextEntity2D::SetLocation( Vector2f& location )
{
	m_vLocation = location;
}
//--------------------------------------------------------------------------------
void TextEntity2D::SetSize( Vector2f& size )
{
	m_vSize = size;
}
//--------------------------------------------------------------------------------
Vector2f TextEntity2D::GetLocation( )
{
	return( m_vLocation );
}
//--------------------------------------------------------------------------------
Vector2f TextEntity2D::GetSize( )
{
	return( m_vSize );
}
//--------------------------------------------------------------------------------
void TextEntity2D::SetFont( int font )
{
	m_iFont = font;
}
//--------------------------------------------------------------------------------
int TextEntity2D::GetFont( )
{
	return( m_iFont );
}
//--------------------------------------------------------------------------------
void TextEntity2D::SetColor( Vector4f color )
{
	m_vColor = color;
}
//--------------------------------------------------------------------------------
Vector4f TextEntity2D::GetColor( )
{
	return( m_vColor );
}
//--------------------------------------------------------------------------------
void TextEntity2D::BuildPickRecord( Ray3f& ray, TArray<PickRecord>& record )
{
	// Do nothing - the text should not intersect a pick ray.
}
//--------------------------------------------------------------------------------
void TextEntity2D::SetText( std::string text )
{
	m_sText = text;
}
//--------------------------------------------------------------------------------
std::string TextEntity2D::GetText( )
{
	return( m_sText );
}
//--------------------------------------------------------------------------------