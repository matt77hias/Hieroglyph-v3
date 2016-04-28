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
#include "Transform3D.h"
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
Transform3D::Transform3D()
{
	m_vTranslation.MakeZero();
	m_mRotation.MakeIdentity();
	m_vScale = Vector3f( 1.0f, 1.0f, 1.0f );

	m_mWorld.MakeIdentity();
	m_mLocal.MakeIdentity();
}
//--------------------------------------------------------------------------------
Transform3D::~Transform3D()
{
}
//--------------------------------------------------------------------------------
Vector3f& Transform3D::Position()
{
	return( m_vTranslation );
}
//--------------------------------------------------------------------------------
Matrix3f& Transform3D::Rotation()
{
	return( m_mRotation );
}
//--------------------------------------------------------------------------------
Vector3f& Transform3D::Scale( )
{
	return( m_vScale );
}
//--------------------------------------------------------------------------------
void Transform3D::UpdateLocal( )
{
	// Load the local space matrix with the rotation and translation components.

	m_mLocal.MakeIdentity();
	m_mLocal.SetRotation( m_mRotation );
	m_mLocal.SetTranslation( m_vTranslation );
	m_mLocal = Matrix4f::ScaleMatrix( m_vScale ) * m_mLocal;
}
//--------------------------------------------------------------------------------
void Transform3D::UpdateWorld( const Matrix4f& parent )
{
    m_mWorld = m_mLocal * parent;
}
//--------------------------------------------------------------------------------
void Transform3D::UpdateWorld( )
{
	// If no parent matrix is available, then simply make the world matrix the
	// local matrix.
    m_mWorld = m_mLocal;
}
//--------------------------------------------------------------------------------
const Matrix4f& Transform3D::WorldMatrix() const
{
	return( m_mWorld );
}
//--------------------------------------------------------------------------------
const Matrix4f& Transform3D::LocalMatrix() const
{
	return( m_mLocal );
}
//--------------------------------------------------------------------------------
Matrix4f& Transform3D::WorldMatrix()
{
	return( m_mWorld );
}
//--------------------------------------------------------------------------------
Matrix4f& Transform3D::LocalMatrix()
{
	return( m_mLocal );
}
//--------------------------------------------------------------------------------
Matrix4f Transform3D::GetView() const
{
	Vector3f Eye;
	Vector3f At;
	Vector3f Up;

	Eye = m_mWorld.GetTranslation();
	At = m_mWorld.GetTranslation() + m_mWorld.GetBasisZ();
	Up = m_mWorld.GetBasisY();

	return( Matrix4f::LookAtLHMatrix( Eye, At, Up ) );
}
//--------------------------------------------------------------------------------
Vector4f Transform3D::LocalToWorldSpace( const Vector4f& input )
{
	Vector4f result = WorldMatrix() * input;

    return( result );
}
//--------------------------------------------------------------------------------
Vector4f Transform3D::WorldToLocalSpace( const Vector4f& input )
{
	Vector4f result = WorldMatrix().Inverse() * input;

    return( result );
}
//--------------------------------------------------------------------------------
Vector3f Transform3D::LocalVectorToWorldSpace( const Vector3f& input )
{
	Vector4f result = LocalToWorldSpace( Vector4f( input, 0.0f ) ); 

	return( result.xyz() );
}
//--------------------------------------------------------------------------------
Vector3f Transform3D::LocalPointToWorldSpace( const Vector3f& input )
{
	Vector4f result = LocalToWorldSpace( Vector4f( input, 1.0f ) ); 

	return( result.xyz() );
}
//--------------------------------------------------------------------------------
Vector3f Transform3D::WorldVectorToLocalSpace( const Vector3f& input )
{
	Vector4f result = WorldToLocalSpace( Vector4f( input, 0.0f ) ); 

	return( result.xyz() );
}
//--------------------------------------------------------------------------------
Vector3f Transform3D::WorldPointToLocalSpace( const Vector3f& input )
{
	Vector4f result = WorldToLocalSpace( Vector4f( input, 1.0f ) ); 

	return( result.xyz() );
}
//--------------------------------------------------------------------------------