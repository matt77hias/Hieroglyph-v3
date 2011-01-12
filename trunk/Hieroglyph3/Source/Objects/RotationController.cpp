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
#include "PCH.h"
#include "RotationController.h"
#include "Entity3D.h"
#include "Log.h"
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
RotationController::RotationController()
{
	m_fSpeed = 0.25f;
	m_kAxis = Vector3f( 0.0f, 1.0f, 0.0f );
}
//--------------------------------------------------------------------------------
RotationController::RotationController( Vector3f axis, float speed )
{
	m_fSpeed = speed;
	m_kAxis = axis;
}
//--------------------------------------------------------------------------------
RotationController::~RotationController()
{
}
//--------------------------------------------------------------------------------
void RotationController::Update( float fTime )
{
	// Calculate the new animation values, then set the local position and 
	// rotation accordingly.  These new values will then be used by the entity
	// to update it's local and world transformation matrices.

	if ( m_pEntity )
	{
		Matrix3f delta;
		delta.RotationEuler( m_kAxis, fTime * m_fSpeed );
		Matrix3f rotation = m_pEntity->Rotation();
		
		m_pEntity->Rotation() = rotation * delta;
	}

}
//--------------------------------------------------------------------------------
void RotationController::SetAxis( Vector3f axis )
{
	m_kAxis = axis;
}
//--------------------------------------------------------------------------------
void RotationController::SetSpeed( float speed )
{
	m_fSpeed = speed;
}
//--------------------------------------------------------------------------------
