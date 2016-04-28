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
// KinectSkeletonActor
//
// The Kinect skeleton data structure simply provides joint positions to represent
// the state of each joint (i.e. there is no rotation data provided).  The data is
// also not hierarchical - the positions are relative to the "skeleton space",
// which is described as having the following ranges:
//
//   [-2.2 < x < 2.2]
//   [-1.6 < y < 1.6]
//   [ 0.0 < z < 4.0]
//
// Since these joint positions are not hierarchical, we represent the skeleton
// actor with a node as the root and then a single entity for each of the joints.
// This is a flat structure, but allows each joint to have its own mesh.  The 
// following list defines each of the joints, as well as the enumeration value
// to index it with.
//
//		NUI_SKELETON_POSITION_HIP_CENTER
//		NUI_SKELETON_POSITION_SPINE
//		NUI_SKELETON_POSITION_SHOULDER_CENTER
//		NUI_SKELETON_POSITION_HEAD
//		NUI_SKELETON_POSITION_SHOULDER_LEFT
//		NUI_SKELETON_POSITION_ELBOW_LEFT
//		NUI_SKELETON_POSITION_WRIST_LEFT
//		NUI_SKELETON_POSITION_HAND_LEFT
//		NUI_SKELETON_POSITION_SHOULDER_RIGHT
//		NUI_SKELETON_POSITION_ELBOW_RIGHT
//		NUI_SKELETON_POSITION_WRIST_RIGHT
//		NUI_SKELETON_POSITION_HAND_RIGHT
//		NUI_SKELETON_POSITION_HIP_LEFT
//		NUI_SKELETON_POSITION_KNEE_LEFT
//		NUI_SKELETON_POSITION_ANKLE_LEFT
//		NUI_SKELETON_POSITION_FOOT_LEFT
//		NUI_SKELETON_POSITION_HIP_RIGHT
//		NUI_SKELETON_POSITION_KNEE_RIGHT
//		NUI_SKELETON_POSITION_ANKLE_RIGHT
//		NUI_SKELETON_POSITION_FOOT_RIGHT
//		NUI_SKELETON_POSITION_COUNT
//
//--------------------------------------------------------------------------------
#ifndef KinectSkeletonActor_h
#define KinectSkeletonActor_h
//--------------------------------------------------------------------------------
#include "GeometryActor.h"
#include "KinectManager.h"
#include "NuiApi.h"
//--------------------------------------------------------------------------------
namespace Glyph3
{
	class MatrixArrayParameterDX11;

	class KinectSkeletonActor : public GeometryActor
	{
	public:
		KinectSkeletonActor();
		virtual ~KinectSkeletonActor();

		void UpdateSkeleton( NUI_SKELETON_DATA* pData );

	private:
		void DrawLinkage( NUI_SKELETON_DATA* pData, NUI_SKELETON_POSITION_INDEX i1, NUI_SKELETON_POSITION_INDEX i2 );

	};
};
//--------------------------------------------------------------------------------
#endif // KinectSkeletonActor_h
