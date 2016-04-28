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
RotationController<T>::RotationController()
{
	m_fSpeed = 0.25f;
	m_kAxis = Vector3f( 0.0f, 1.0f, 0.0f );
}
//--------------------------------------------------------------------------------
template <typename T>
RotationController<T>::RotationController( Vector3f axis, float speed )
{
	m_fSpeed = speed;
	m_kAxis = axis;
}
//--------------------------------------------------------------------------------
template <typename T>
RotationController<T>::~RotationController()
{
}
//--------------------------------------------------------------------------------
template <typename T>
void RotationController<T>::Update( float fTime )
{
	// Calculate the new animation values, then set the local position and 
	// rotation accordingly.  These new values will then be used by the entity
	// to update it's local and world transformation matrices.

	if ( m_pEntity )
	{
		Matrix3f delta;
		delta.RotationEuler( m_kAxis, fTime * m_fSpeed );

		Matrix3f rotation = m_pEntity->Transform.Rotation();
		m_pEntity->Transform.Rotation() = rotation * delta;
		m_pEntity->Transform.Rotation().Orthonormalize();
	}

}
//--------------------------------------------------------------------------------
template <typename T>
void RotationController<T>::SetAxis( Vector3f axis )
{
	m_kAxis = axis;
}
//--------------------------------------------------------------------------------
template <typename T>
void RotationController<T>::SetSpeed( float speed )
{
	m_fSpeed = speed;
}
//--------------------------------------------------------------------------------
