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
#include "PCH.h"
#include "Animation.h"
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
//template class Glyph3::AnimationStream<Vector3f>;
//--------------------------------------------------------------------------------
Animation::Animation() :
	m_Name( L"" ),
	m_fStartTime( 0 ),
	m_fEndTime( 0 )
{
};
//--------------------------------------------------------------------------------
Animation::Animation( const std::wstring& name, float start, float stop ) :
	m_Name( name ),
	m_fStartTime( start ),
	m_fEndTime( stop )
{
};
//--------------------------------------------------------------------------------
