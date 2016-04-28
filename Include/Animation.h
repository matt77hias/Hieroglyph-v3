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
// Animation
//--------------------------------------------------------------------------------
#ifndef Animation_h
#define Animation_h
//--------------------------------------------------------------------------------
#include "PCH.h"
#include "Vector3f.h"
//--------------------------------------------------------------------------------
namespace Glyph3
{
	class Animation
	{
	public:
		Animation();
		Animation( const std::wstring& name, float start, float stop );

		std::wstring m_Name;
		float m_fStartTime;
		float m_fEndTime;
	};
};
//--------------------------------------------------------------------------------
#endif // Animation_h
//--------------------------------------------------------------------------------
