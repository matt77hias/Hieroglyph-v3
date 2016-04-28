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
// ArmsForwardPose
//
//--------------------------------------------------------------------------------
#ifndef ArmsForwardPose_h
#define ArmsForwardPose_h
//--------------------------------------------------------------------------------
#include "PCH.h"
#include "NuiApi.h"
//--------------------------------------------------------------------------------
namespace Glyph3
{
	class ArmsForwardPose
	{
	public:
		ArmsForwardPose();
		virtual ~ArmsForwardPose();

		virtual bool TestPose( NUI_SKELETON_DATA& data );
	};
}
//--------------------------------------------------------------------------------
#endif // ArmsForwardPose_h