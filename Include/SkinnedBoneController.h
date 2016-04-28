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
// SkinnedBoneController
//
//--------------------------------------------------------------------------------
#ifndef SkinnedBoneController_h
#define SkinnedBoneController_h
//--------------------------------------------------------------------------------
#include "IController.h"
#include "AnimationStream.h"
#include "Matrix4f.h"
//--------------------------------------------------------------------------------
namespace Glyph3
{
	template <typename T>
	class SkinnedBoneController : public IController<T>
	{
	public:
		SkinnedBoneController( );
		virtual ~SkinnedBoneController( );
		virtual void Update( float fTime );

		void SetBindPose();
		Matrix4f GetTransform();
		Matrix4f GetNormalTransform();

		void SetPositionStream( AnimationStream<Vector3f>* pStream );
		void SetRotationStream( AnimationStream<Vector3f>* pStream );
		AnimationStream<Vector3f>* GetPositionStream( );
		AnimationStream<Vector3f>* GetRotationStream( );

		void SetBindPosition( Vector3f position );
		void SetBindRotation( Vector3f rotation );
		Vector3f GetBindPosition( );
		Vector3f GetBindRotation( );
		
		void SetLocalSkeleton( );
		void SetGlobalSkeleton( );
		
		void SetParentBone( SkinnedBoneController* pParent );

		
	protected:
		Matrix4f m_LocalSkeleton;
		Matrix4f m_GlobalSkeleton;
		Matrix4f m_InvBindPose;
		SkinnedBoneController* m_pParentBone;

		AnimationStream<Vector3f>*	m_pPositionStream;
		AnimationStream<Vector3f>*	m_pRotationStream;
		Vector3f					m_kBindPosition;
		Vector3f					m_kBindRotation;
		bool						m_bActivate;
	};

	#include "SkinnedBoneController.inl"
};
//--------------------------------------------------------------------------------
#endif // SkinnedBoneController_h
