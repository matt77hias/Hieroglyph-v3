//--------------------------------------------------------------------------------
// This file is a portion of the Hieroglyph 3 Rendering Engine.  It is distributed
// under the MIT License, available in the root of this distribution and 
// at the following URL:
//
// http://www.opensource.org/licenses/mit-license.php
//
// Copyright (c) Jason Zink 
//--------------------------------------------------------------------------------
#include "GlyphKinect/ArmsForwardPose.h"
#include "Log.h"
#include "Vector3f.h"
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
ArmsForwardPose::ArmsForwardPose(void)
{
}
//--------------------------------------------------------------------------------
ArmsForwardPose::~ArmsForwardPose(void)
{
}
//--------------------------------------------------------------------------------
bool ArmsForwardPose::TestPose( NUI_SKELETON_DATA& data )
{
	Vector3f rightShoulder = Vector3f(	
		data.SkeletonPositions[NUI_SKELETON_POSITION_SHOULDER_RIGHT].x,
		data.SkeletonPositions[NUI_SKELETON_POSITION_SHOULDER_RIGHT].y,
		data.SkeletonPositions[NUI_SKELETON_POSITION_SHOULDER_RIGHT].z );

	Vector3f rightElbow = Vector3f(	
		data.SkeletonPositions[NUI_SKELETON_POSITION_ELBOW_RIGHT].x,
		data.SkeletonPositions[NUI_SKELETON_POSITION_ELBOW_RIGHT].y,
		data.SkeletonPositions[NUI_SKELETON_POSITION_ELBOW_RIGHT].z );

	Vector3f rightWrist = Vector3f(	
		data.SkeletonPositions[NUI_SKELETON_POSITION_WRIST_RIGHT].x,
		data.SkeletonPositions[NUI_SKELETON_POSITION_WRIST_RIGHT].y,
		data.SkeletonPositions[NUI_SKELETON_POSITION_WRIST_RIGHT].z );

	Vector3f v1 = rightElbow - rightShoulder;
	Vector3f v2 = rightWrist - rightElbow;

	v1.Normalize();
	v2.Normalize();

	float fStraight = v1.Dot( v2 );

	float fForward = v1.Dot( Vector3f( 0.0f, 0.0f, -1.0f ) );

	std::wstringstream s;
	s << L"Straightness: " << fStraight << L", Forwardness:" << fForward;
	//Log::Get().Write( s.str() );

	return( fStraight > 0.95f && fForward > 0.95f );
}
//--------------------------------------------------------------------------------



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