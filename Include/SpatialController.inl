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
template <typename T>
SpatialController<T>::SpatialController() :
	m_vRotation( 0.0f, 0.0f, 0.0f ),
	m_vTranslation( 0.0f, 0.0f, 0.0f )
{
}
//--------------------------------------------------------------------------------
template <typename T>
SpatialController<T>::SpatialController( const Vector3f& translation, const Vector3f& rotation ) :
	m_vRotation( rotation ),
	m_vTranslation( translation )
{
}
//--------------------------------------------------------------------------------
template <typename T>
SpatialController<T>::~SpatialController()
{
}
//--------------------------------------------------------------------------------
template <typename T>
void SpatialController<T>::Update( float fTime )
{
	// Set the translation and rotation parameters according to what the user has
	// set in this controller.

	if ( m_pEntity )
	{
		m_pEntity->Transform.Rotation().Rotation( m_vRotation );
		m_pEntity->Transform.Position() = m_vTranslation;
	}

}
//--------------------------------------------------------------------------------
template <typename T>
void SpatialController<T>::SetRotation( const Vector3f& xyz )
{
	m_vRotation = xyz;
}
//--------------------------------------------------------------------------------
template <typename T>
void SpatialController<T>::SetTranslation( const Vector3f& translation )
{
	m_vTranslation = translation;
}
//--------------------------------------------------------------------------------
template <typename T>
Vector3f SpatialController<T>::GetRotation()
{
	return m_vRotation;
}
//--------------------------------------------------------------------------------
template <typename T>
Vector3f SpatialController<T>::GetTranslation()
{
	return m_vTranslation;
}
//--------------------------------------------------------------------------------
template <typename T>
void SpatialController<T>::RotateBy( const Vector3f& xyz )
{
	m_vRotation += xyz;
}
//--------------------------------------------------------------------------------
template <typename T>
void SpatialController<T>::RotateXBy( const float x )
{
	m_vRotation.x += x;
}
//--------------------------------------------------------------------------------
template <typename T>
void SpatialController<T>::RotateYBy( const float y )
{
	m_vRotation.y += y;
}
//--------------------------------------------------------------------------------
template <typename T>
void SpatialController<T>::RotateZBy( const float z )
{
	m_vRotation.z += z;
}
//--------------------------------------------------------------------------------
template <typename T>
void SpatialController<T>::TranslateBy( Vector3f& xyz )
{
	m_vTranslation += xyz;
}
//--------------------------------------------------------------------------------
template <typename T>
void SpatialController<T>::TranslateXBy( const float x )
{
	m_vTranslation.x += x;
}
//--------------------------------------------------------------------------------
template <typename T>
void SpatialController<T>::TranslateYBy( const float y )
{
	m_vTranslation.y += y;
}
//--------------------------------------------------------------------------------
template <typename T>
void SpatialController<T>::TranslateZBy( const float z )
{
	m_vTranslation.z += z;
}
//--------------------------------------------------------------------------------
template <typename T>
void SpatialController<T>::MoveForward( const float distance )
{
	if ( m_pEntity )
	{
		m_vTranslation += m_pEntity->Transform.Rotation().GetRow( 2 ) * distance;
	}
}
//--------------------------------------------------------------------------------
template <typename T>
void SpatialController<T>::MoveBackward( const float distance )
{
	if ( m_pEntity )
	{
		m_vTranslation -= m_pEntity->Transform.Rotation().GetRow( 2 ) * distance;
	}
}
//--------------------------------------------------------------------------------
template <typename T>
void SpatialController<T>::MoveRight( const float distance )
{
	if ( m_pEntity )
	{
		m_vTranslation += m_pEntity->Transform.Rotation().GetRow( 0 ) * distance;
	}
}
//--------------------------------------------------------------------------------
template <typename T>
void SpatialController<T>::MoveLeft( const float distance )
{
	if ( m_pEntity )
	{
		m_vTranslation -= m_pEntity->Transform.Rotation().GetRow( 0 ) * distance;
	}
}
//--------------------------------------------------------------------------------
template <typename T>
void SpatialController<T>::MoveUp( const float distance )
{
	if ( m_pEntity )
	{
		m_vTranslation += m_pEntity->Transform.Rotation().GetRow( 1 ) * distance;
	}
}
//--------------------------------------------------------------------------------
template <typename T>
void SpatialController<T>::MoveDown( const float distance )
{	
	if ( m_pEntity )
	{
		m_vTranslation -= m_pEntity->Transform.Rotation().GetRow( 1 ) * distance;
	}
}
//--------------------------------------------------------------------------------