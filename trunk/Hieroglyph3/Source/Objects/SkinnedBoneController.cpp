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
#include "SkinnedBoneController.h"
#include "Entity3D.h"
#include "Log.h"
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
SkinnedBoneController::SkinnedBoneController()
{
	m_kBindPosition.MakeZero();
	m_kBindRotation.MakeZero();
	m_pPositionStream = 0;
	m_pRotationStream = 0;

	m_pParentBone = 0;
	m_LocalSkeleton.MakeIdentity();
	m_GlobalSkeleton.MakeIdentity();

	m_bActivate = false;
}
//--------------------------------------------------------------------------------
SkinnedBoneController::~SkinnedBoneController()
{
	SAFE_DELETE( m_pPositionStream );
	SAFE_DELETE( m_pRotationStream );
}
//--------------------------------------------------------------------------------
void SkinnedBoneController::Update( float fTime )
{
	// Skip the first update to allow the bind pose to be read.
	if ( !m_bActivate )
	{
		m_bActivate = true;
		return;
	}

	// Calculate the new animation values, then set the local position and 
	// rotation accordingly.  These new values will then be used by the entity
	// to update it's local and world transformation matrices.
	
	if ( m_pPositionStream )
	{
		// Update the position animation stream with the elapsed time.
		m_pPositionStream->Update( fTime );

		// Update the entity's position, which is the bind pose plus the current animated position.
		m_pEntity->Position() = m_kBindPosition + m_pPositionStream->GetState();
	}

	if ( m_pRotationStream )
	{
		// Update the rotation animation stream with the elapsed time.
		m_pRotationStream->Update( fTime );

		// Update the entity's rotation, which is the bind pose plus the current animated rotation.
		m_pEntity->Rotation().Rotation( m_kBindRotation + m_pRotationStream->GetState() );
	}
}
//--------------------------------------------------------------------------------
void SkinnedBoneController::SetBindPose()
{
	// The inverse bind pose is the inverse of the world matrix when the model is 
	// in the bind pose.

	m_InvBindPose = GetEntity()->WorldMatrix().Inverse();
}
//--------------------------------------------------------------------------------
Matrix4f SkinnedBoneController::GetTransform()
{
	// The output transform is the inverse bind pose, multiplied with the updated
	// animation based entity world matrix.
	Matrix4f Transform = m_InvBindPose * GetEntity()->WorldMatrix();

	return( Transform );
}
//--------------------------------------------------------------------------------
Matrix4f SkinnedBoneController::GetNormalTransform()
{
	// The output transform is the inverse bind pose, multiplied with the updated
	// animation based entity world matrix.
	Matrix4f Transform = m_InvBindPose * GetEntity()->WorldMatrix();
	Matrix4f InvTransform = Transform.Inverse();
	Matrix4f TransposeInvTransform = InvTransform.Transpose();

	return( TransposeInvTransform );
}
//--------------------------------------------------------------------------------
void SkinnedBoneController::SetLocalSkeleton( )
{
	m_LocalSkeleton.MakeIdentity();

	Matrix3f localRot;
	//localRot.RotationZYX( this->m_kBindRotation );
	localRot.Rotation( this->m_kBindRotation );

	m_LocalSkeleton.SetRotation( localRot );
	m_LocalSkeleton.SetTranslation( m_kBindPosition );
}
//--------------------------------------------------------------------------------
void SkinnedBoneController::SetGlobalSkeleton( )
{
	// Concatenate this bone's parent skeleton matrix with its local skeleton matrix.
	if ( !m_pParentBone )
		this->m_GlobalSkeleton = m_LocalSkeleton;
	else
	{
		//m_pParentBone->SetGlobalSkeleton();
		this->m_GlobalSkeleton = m_pParentBone->m_GlobalSkeleton * this->m_LocalSkeleton;
	}
}
//--------------------------------------------------------------------------------
void SkinnedBoneController::SetPositionStream( AnimationStream<Vector3f>* pStream )
{
	m_pPositionStream = pStream;
}
//--------------------------------------------------------------------------------
void SkinnedBoneController::SetRotationStream( AnimationStream<Vector3f>* pStream )
{
	m_pRotationStream = pStream;
}
//--------------------------------------------------------------------------------
AnimationStream<Vector3f>* SkinnedBoneController::GetPositionStream( )
{
	return( m_pPositionStream );
}
//--------------------------------------------------------------------------------
AnimationStream<Vector3f>* SkinnedBoneController::GetRotationStream( )
{
	return( m_pRotationStream );
}
//--------------------------------------------------------------------------------
void SkinnedBoneController::SetBindPosition( Vector3f position )
{
	m_kBindPosition = position;
}
//--------------------------------------------------------------------------------
void SkinnedBoneController::SetBindRotation( Vector3f rotation )
{
	m_kBindRotation = rotation;
}
//--------------------------------------------------------------------------------
Vector3f SkinnedBoneController::GetBindPosition( )
{
	return( m_kBindPosition );
}
//--------------------------------------------------------------------------------
Vector3f SkinnedBoneController::GetBindRotation( )
{
	return( m_kBindRotation );
}
//--------------------------------------------------------------------------------
void SkinnedBoneController::SetParentBone( SkinnedBoneController* pParent )
{
	this->m_pParentBone = pParent;
}
//--------------------------------------------------------------------------------