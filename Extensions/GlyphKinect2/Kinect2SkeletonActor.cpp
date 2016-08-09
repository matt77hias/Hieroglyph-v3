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
#include "GlyphKinect2/Kinect2SkeletonActor.h"
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
Kinect2SkeletonActor::Kinect2SkeletonActor()
{
}
//--------------------------------------------------------------------------------
Kinect2SkeletonActor::~Kinect2SkeletonActor()
{
}
//--------------------------------------------------------------------------------
void Kinect2SkeletonActor::UpdateSkeleton( const Kinect2Skeleton& body )
{
	ResetGeometry();
	SetColor( Vector4f( 1.0f, 1.0f, 0.0f, 0.0f ) );

	for ( int joint = 0; joint < 25; ++joint ) {
		DrawSphere( body.jointPositions[joint], 0.03f );
	}

	SetColor( Vector4f( 0.0f, 1.0f, 0.0f, 0.0f ) );

	// Head to hips
	DrawLinkage( body, Kinect2JointType::Head, Kinect2JointType::Neck);
	DrawLinkage( body, Kinect2JointType::Neck, Kinect2JointType::SpineShoulder);
	DrawLinkage( body, Kinect2JointType::SpineShoulder, Kinect2JointType::SpineMid);
	DrawLinkage( body, Kinect2JointType::SpineMid, Kinect2JointType::SpineBase);

	// Left arm
	DrawLinkage( body, Kinect2JointType::SpineShoulder, Kinect2JointType::ShoulderLeft );
	DrawLinkage( body, Kinect2JointType::ShoulderLeft, Kinect2JointType::ElbowLeft );
	DrawLinkage( body, Kinect2JointType::ElbowLeft, Kinect2JointType::WristLeft );
	DrawLinkage( body, Kinect2JointType::WristLeft, Kinect2JointType::HandLeft );
	DrawLinkage( body, Kinect2JointType::HandLeft, Kinect2JointType::HandTipLeft );
	DrawLinkage( body, Kinect2JointType::HandLeft, Kinect2JointType::ThumbLeft );

	// Right arm
	DrawLinkage( body, Kinect2JointType::SpineShoulder, Kinect2JointType::ShoulderRight );
	DrawLinkage( body, Kinect2JointType::ShoulderRight, Kinect2JointType::ElbowRight );
	DrawLinkage( body, Kinect2JointType::ElbowRight, Kinect2JointType::WristRight );
	DrawLinkage( body, Kinect2JointType::WristRight, Kinect2JointType::HandRight );
	DrawLinkage( body, Kinect2JointType::HandRight, Kinect2JointType::HandTipRight );
	DrawLinkage( body, Kinect2JointType::HandRight, Kinect2JointType::ThumbRight );

	// Left leg
	DrawLinkage( body, Kinect2JointType::SpineBase, Kinect2JointType::HipLeft );
	DrawLinkage( body, Kinect2JointType::HipLeft, Kinect2JointType::KneeLeft );
	DrawLinkage( body, Kinect2JointType::KneeLeft, Kinect2JointType::AnkleLeft );
	DrawLinkage( body, Kinect2JointType::AnkleLeft, Kinect2JointType::FootLeft );

	// Right leg
	DrawLinkage( body, Kinect2JointType::SpineBase, Kinect2JointType::HipRight );
	DrawLinkage( body, Kinect2JointType::HipRight, Kinect2JointType::KneeRight );
	DrawLinkage( body, Kinect2JointType::KneeRight, Kinect2JointType::AnkleRight );
	DrawLinkage( body, Kinect2JointType::AnkleRight, Kinect2JointType::FootRight );
}
//--------------------------------------------------------------------------------
void Kinect2SkeletonActor::DrawLinkage( const Kinect2Skeleton& body, Kinect2JointType i1, Kinect2JointType i2  )
{
	Vector3f p1, p2;

	p1 = body.jointPositions[static_cast<int>(i1)];
	p2 = body.jointPositions[static_cast<int>(i2)];

	DrawCylinder( p1, p2, 0.015f, 0.015f );
}
//--------------------------------------------------------------------------------