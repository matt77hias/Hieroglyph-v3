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
// Kinect2SkeletonActor
//
// This actor provides a geometric visualization of the skeleton that is tracked
// by the Kinect 2 sensor.
//--------------------------------------------------------------------------------
#ifndef Kinect2SkeletonActor_h
#define Kinect2SkeletonActor_h
//--------------------------------------------------------------------------------
#include "GeometryActor.h"
#include "Kinect2Manager.h"
//--------------------------------------------------------------------------------
namespace Glyph3
{
	class Kinect2SkeletonActor : public GeometryActor
	{
	public:
		Kinect2SkeletonActor();
		virtual ~Kinect2SkeletonActor();

		void UpdateSkeleton( const Kinect2Skeleton& body );

	private:
		void DrawLinkage( const Kinect2Skeleton& body, Kinect2JointType i1, Kinect2JointType i2 );
	};
};
//--------------------------------------------------------------------------------
#endif // Kinect2SkeletonActor_h
//--------------------------------------------------------------------------------