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
// KinectPose
//
//--------------------------------------------------------------------------------
#ifndef KinectPose_h
#define KinectPose_h
//--------------------------------------------------------------------------------
#include "PCH.h"
#include "NuiApi.h"
//--------------------------------------------------------------------------------
namespace Glyph3
{
	class KinectPose
	{
	public:
		KinectPose();
		virtual ~KinectPose();

		virtual bool TestPose( NUI_SKELETON_DATA& data ) = 0;
	};
}
//--------------------------------------------------------------------------------
#endif // KinectPose_h