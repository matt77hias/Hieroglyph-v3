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
// SkinnedActor
//
//--------------------------------------------------------------------------------
#ifndef SkinnedActor_h
#define SkinnedActor_h
//--------------------------------------------------------------------------------
#include "Actor.h"
#include "SkinnedBoneController.h"
#include "AnimationStream.h"
#include "MatrixArrayParameterWriterDX11.h"
//--------------------------------------------------------------------------------
namespace Glyph3
{
	class MatrixArrayParameterDX11;

	class SkinnedActor : public Actor
	{
	public:
		SkinnedActor();
		virtual ~SkinnedActor();

		void AddBoneNode( Node3D* pBone, 
						Vector3f BindPosition,
						Vector3f BindRotation,
						AnimationStream<Vector3f>* pPositions = 0,
						AnimationStream<Vector3f>* pRotations = 0 ); 
		void SetBindPose( );
		void SetSkinningMatrices( RendererDX11& Renderer );
		void PlayAnimation( int index );
		void PlayAnimation( std::wstring& name );
		void PlayAllAnimations( );

		Entity3D* GetGeometryEntity();

	protected:
		std::vector<SkinnedBoneController<Node3D>*>		m_Bones;
		Matrix4f*										m_pMatrices;
		Matrix4f*										m_pNormalMatrices;
		Entity3D*										m_pGeometryEntity;

		MatrixArrayParameterWriterDX11*					m_pSkinMatrixWriter;
		MatrixArrayParameterWriterDX11*					m_pNormalMatrixWriter;
	};
};
//--------------------------------------------------------------------------------
#endif // SkinnedActor_h
//--------------------------------------------------------------------------------