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
SkinnedBoneController<T>::SkinnedBoneController()
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
template <typename T>
SkinnedBoneController<T>::~SkinnedBoneController()
{
	SAFE_DELETE( m_pPositionStream );
	SAFE_DELETE( m_pRotationStream );
}
//--------------------------------------------------------------------------------
template <typename T>
void SkinnedBoneController<T>::Update( float fTime )
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
		m_pEntity->Transform.Position() = m_kBindPosition + m_pPositionStream->GetState();
	}

	if ( m_pRotationStream )
	{
		// Update the rotation animation stream with the elapsed time.
		m_pRotationStream->Update( fTime );

		// Update the entity's rotation, which is the bind pose plus the current animated rotation.
		m_pEntity->Transform.Rotation().Rotation( m_kBindRotation + m_pRotationStream->GetState() );
	}
}
//--------------------------------------------------------------------------------
template <typename T>
void SkinnedBoneController<T>::SetBindPose()
{
	// The inverse bind pose is the inverse of the world matrix when the model is 
	// in the bind pose.

	m_InvBindPose = GetEntity()->Transform.WorldMatrix().Inverse();
}
//--------------------------------------------------------------------------------
template <typename T>
Matrix4f SkinnedBoneController<T>::GetTransform()
{
	// The output transform is the inverse bind pose, multiplied with the updated
	// animation based entity world matrix.
	Matrix4f transform = m_InvBindPose * GetEntity()->Transform.WorldMatrix();

	return( transform );
}
//--------------------------------------------------------------------------------
template <typename T>
Matrix4f SkinnedBoneController<T>::GetNormalTransform()
{
	// The output transform is the inverse bind pose, multiplied with the updated
	// animation based entity world matrix.
	Matrix4f transform = m_InvBindPose * GetEntity()->Transform.WorldMatrix();
	Matrix4f InvTransform = transform.Inverse();
	Matrix4f TransposeInvTransform = InvTransform.Transpose();

	return( TransposeInvTransform );
}
//--------------------------------------------------------------------------------
template <typename T>
void SkinnedBoneController<T>::SetLocalSkeleton( )
{
	m_LocalSkeleton.MakeIdentity();

	Matrix3f localRot;
	//localRot.RotationZYX( this->m_kBindRotation );
	localRot.Rotation( this->m_kBindRotation );

	m_LocalSkeleton.SetRotation( localRot );
	m_LocalSkeleton.SetTranslation( m_kBindPosition );
}
//--------------------------------------------------------------------------------
template <typename T>
void SkinnedBoneController<T>::SetGlobalSkeleton( )
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
template <typename T>
void SkinnedBoneController<T>::SetPositionStream( AnimationStream<Vector3f>* pStream )
{
	m_pPositionStream = pStream;
}
//--------------------------------------------------------------------------------
template <typename T>
void SkinnedBoneController<T>::SetRotationStream( AnimationStream<Vector3f>* pStream )
{
	m_pRotationStream = pStream;
}
//--------------------------------------------------------------------------------
template <typename T>
AnimationStream<Vector3f>* SkinnedBoneController<T>::GetPositionStream( )
{
	return( m_pPositionStream );
}
//--------------------------------------------------------------------------------
template <typename T>
AnimationStream<Vector3f>* SkinnedBoneController<T>::GetRotationStream( )
{
	return( m_pRotationStream );
}
//--------------------------------------------------------------------------------
template <typename T>
void SkinnedBoneController<T>::SetBindPosition( Vector3f position )
{
	m_kBindPosition = position;
}
//--------------------------------------------------------------------------------
template <typename T>
void SkinnedBoneController<T>::SetBindRotation( Vector3f rotation )
{
	m_kBindRotation = rotation;
}
//--------------------------------------------------------------------------------
template <typename T>
Vector3f SkinnedBoneController<T>::GetBindPosition( )
{
	return( m_kBindPosition );
}
//--------------------------------------------------------------------------------
template <typename T>
Vector3f SkinnedBoneController<T>::GetBindRotation( )
{
	return( m_kBindRotation );
}
//--------------------------------------------------------------------------------
template <typename T>
void SkinnedBoneController<T>::SetParentBone( SkinnedBoneController* pParent )
{
	this->m_pParentBone = pParent;
}
//--------------------------------------------------------------------------------