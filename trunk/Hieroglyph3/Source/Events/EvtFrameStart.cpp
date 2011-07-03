//--------------------------------------------------------------------------------
// This file is a portion of the Hieroglyph 3 Rendering Engine.  It is distributed
// under the MIT License, available in the root of this distribution and 
// at the following URL:
//
// http://www.opensource.org/licenses/mit-license.php
//
// Copyright (c) 2003-2010 Jason Zink 
//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
#include "PCH.h"
#include "EvtFrameStart.h"
#include "Timer.h"
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
EvtFrameStart::EvtFrameStart( Timer& timer )
	: m_Timer( timer )
{
	m_Timer = timer;
}
//--------------------------------------------------------------------------------
EvtFrameStart::~EvtFrameStart( )
{
}
//--------------------------------------------------------------------------------
std::wstring EvtFrameStart::GetEventName( )
{
	return( std::wstring( L"frame_start" ) );
}
//--------------------------------------------------------------------------------
eEVENT EvtFrameStart::GetEventType( )
{
	return( RENDER_FRAME_START );
}
//--------------------------------------------------------------------------------
float EvtFrameStart::GetElapsed()
{
	return( m_Timer.Elapsed() );
}
//--------------------------------------------------------------------------------
float EvtFrameStart::GetFrameRate()
{
	return( static_cast<float>( m_Timer.Framerate() ) );
}
//--------------------------------------------------------------------------------
float EvtFrameStart::GetRuntime()
{
	return( m_Timer.Runtime() );
}
//--------------------------------------------------------------------------------
int EvtFrameStart::GetFrameCount()
{
	return( m_Timer.FrameCount() );
}
//--------------------------------------------------------------------------------