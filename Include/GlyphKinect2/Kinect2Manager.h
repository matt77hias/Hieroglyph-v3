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
// Kinect2Manager
//
// This class provides a simple abstraction layer between the Kinect 2.0 API and
// the Hieroglyph 3 classes that use it.  This effectively contains any changes
// to the API within this class.  The class utilizes the Pimpl idiom to hide the
// Kinect SDK dependency from any applications that use the library.  Because of
// this, the joint type enumeration below is provided to replace the Kinect
// specific one.
//--------------------------------------------------------------------------------
#ifndef Kinect2Manager_h
#define Kinect2Manager_h
//--------------------------------------------------------------------------------
#include "Vector3f.h"
//--------------------------------------------------------------------------------
namespace Glyph3
{
	enum class Kinect2JointType
	{
		SpineBase = 0,		// Base of the spine  
		SpineMid = 1,		// Middle of the spine  
		Neck = 2,			// Neck  
		Head = 3,			// Head  

		ShoulderLeft = 4,	// Left shoulder  
		ElbowLeft = 5,		// Left elbow  
		WristLeft = 6,		// Left wrist  
		HandLeft = 7,		// Left hand  

		ShoulderRight = 8,	// Right shoulder  
		ElbowRight = 9,		// Right elbow  
		WristRight = 10,	// Right wrist  
		HandRight = 11,		// Right hand  

		HipLeft = 12,		// Left hip  
		KneeLeft = 13,		// Left knee  
		AnkleLeft = 14,		// Left ankle  
		FootLeft = 15,		// Left foot  

		HipRight = 16,		// Right hip  
		KneeRight = 17,		// Right knee  
		AnkleRight = 18,	// Right ankle  
		FootRight = 19,		// Right foot  

		SpineShoulder = 20, // Spine  

		HandTipLeft = 21,	// Tip of the left hand  
		ThumbLeft = 22,		// Left thumb  

		HandTipRight = 23,	// Tip of the right hand  
		ThumbRight = 24,	// Right thumb  
	};

	const int Kinect2JointCount = static_cast<int>(Kinect2JointType::ThumbRight)+1;
	const int Kinect2BodyCount = 6;

	struct Kinect2Skeleton
	{
		bool tracked;
		Vector3f jointPositions[Kinect2JointCount];
	};

	struct Kinect2SkeletonFrame
	{
		Kinect2Skeleton body[Kinect2BodyCount];
	};

	class Kinect2Manager
	{
	public:
		Kinect2Manager();
		~Kinect2Manager();

		bool Initialize( bool enable_skeleton = true, bool enable_color = true, bool enable_depth = true );
		void Update();
		void Shutdown();

		const Kinect2Skeleton& GetBody( unsigned int body );

	private:
		struct Impl;
		Impl* m_pImpl;
	};
}
//--------------------------------------------------------------------------------
#endif // Kinect2Manager_h
//--------------------------------------------------------------------------------