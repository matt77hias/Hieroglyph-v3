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
#include "SpatialController.h"
#include "Entity3D.h"
#include "Log.h"
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
SpatialController::SpatialController() :
	m_vRotation( 0.0f, 0.0f, 0.0f ),
	m_vTranslation( 0.0f, 0.0f, 0.0f )
{
}
//--------------------------------------------------------------------------------
SpatialController::SpatialController( const Vector3f& translation, const Vector3f& rotation ) :
	m_vRotation( rotation ),
	m_vTranslation( translation )
{
}
//--------------------------------------------------------------------------------
SpatialController::~SpatialController()
{
}
//--------------------------------------------------------------------------------
void SpatialController::Update( float fTime )
{
	// Set the translation and rotation parameters according to what the user has
	// set in this controller.

	if ( m_pEntity )
	{
		m_pEntity->Rotation().Rotation( m_vRotation );
		m_pEntity->Position() = m_vTranslation;
	}

}
//--------------------------------------------------------------------------------
void SpatialController::SetRotation( const Vector3f& xyz )
{
	m_vRotation = xyz;
}
//--------------------------------------------------------------------------------
void SpatialController::SetTranslation( const Vector3f& translation )
{
	m_vTranslation = translation;
}
//--------------------------------------------------------------------------------
void SpatialController::RotateBy( const Vector3f& xyz )
{
	m_vRotation += xyz;
}
//--------------------------------------------------------------------------------
void SpatialController::RotateXBy( const float x )
{
	m_vRotation.x += x;
}
//--------------------------------------------------------------------------------
void SpatialController::RotateYBy( const float y )
{
	m_vRotation.y += y;
}
//--------------------------------------------------------------------------------
void SpatialController::RotateZBy( const float z )
{
	m_vRotation.z += z;
}
//--------------------------------------------------------------------------------
void SpatialController::TranslateBy( Vector3f& xyz )
{
	m_vTranslation += xyz;
}
//--------------------------------------------------------------------------------
void SpatialController::TranslateXBy( const float x )
{
	m_vTranslation.x += x;
}
//--------------------------------------------------------------------------------
void SpatialController::TranslateYBy( const float y )
{
	m_vTranslation.y += y;
}
//--------------------------------------------------------------------------------
void SpatialController::TranslateZBy( const float z )
{
	m_vTranslation.z += z;
}
//--------------------------------------------------------------------------------
void SpatialController::MoveForward( const float distance )
{
	if ( m_pEntity )
	{
		m_vTranslation += m_pEntity->Rotation().GetRow( 2 ) * distance;
	}
}
//--------------------------------------------------------------------------------
void SpatialController::MoveBackward( const float distance )
{
	if ( m_pEntity )
	{
		m_vTranslation -= m_pEntity->Rotation().GetRow( 2 ) * distance;
	}
}
//--------------------------------------------------------------------------------
void SpatialController::MoveRight( const float distance )
{
	if ( m_pEntity )
	{
		m_vTranslation += m_pEntity->Rotation().GetRow( 0 ) * distance;
	}
}
//--------------------------------------------------------------------------------
void SpatialController::MoveLeft( const float distance )
{
	if ( m_pEntity )
	{
		m_vTranslation -= m_pEntity->Rotation().GetRow( 0 ) * distance;
	}
}
//--------------------------------------------------------------------------------
void SpatialController::MoveUp( const float distance )
{
	if ( m_pEntity )
	{
		m_vTranslation += m_pEntity->Rotation().GetRow( 1 ) * distance;
	}
}
//--------------------------------------------------------------------------------
void SpatialController::MoveDown( const float distance )
{	
	if ( m_pEntity )
	{
		m_vTranslation -= m_pEntity->Rotation().GetRow( 1 ) * distance;
	}
}
//--------------------------------------------------------------------------------