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
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
//--------------------------------------------------------------------------------
#include "Timer.h"
#include <windows.h>
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
Timer::Timer()
{
	m_iFramesPerSecond = 0;
	m_iMaxFramesPerSecond = 0;
	m_iFrameCount = 0;

	m_fDelta = 0;

	QueryPerformanceFrequency((LARGE_INTEGER*)&m_TicksPerSecond64);
	QueryPerformanceCounter((LARGE_INTEGER*)&m_CurrentTicks64);
	m_StartupTicks64 = m_CurrentTicks64;
	m_OneSecTicks64 = m_CurrentTicks64;
}
//--------------------------------------------------------------------------------
Timer::~Timer()
{
}
//--------------------------------------------------------------------------------
void Timer::Reset()
{
	m_iFramesPerSecond = 0;
	m_iFrameCount = 0;
	m_fDelta = 0;
}
//--------------------------------------------------------------------------------
void Timer::Update( )
{
	m_LastTicks64 = m_CurrentTicks64;
	QueryPerformanceCounter((LARGE_INTEGER*)&m_CurrentTicks64);

	m_fDelta = (float)((__int64)m_CurrentTicks64 - (__int64)m_LastTicks64)
		/ (__int64)m_TicksPerSecond64;

	if ((float)((__int64)m_CurrentTicks64 - (__int64)m_OneSecTicks64)
		/ (__int64)m_TicksPerSecond64 < 1.0f)
	{
		m_iFrameCount++;
	}
	else
	{
		m_iFramesPerSecond = m_iFrameCount;
		
		if ( m_iFramesPerSecond > m_iMaxFramesPerSecond )
			m_iMaxFramesPerSecond = m_iFramesPerSecond;

		m_iFrameCount = 0;
		m_OneSecTicks64 = m_CurrentTicks64;
	}

}
//--------------------------------------------------------------------------------
float Timer::Elapsed( )
{
	return( m_fDelta );
}
//--------------------------------------------------------------------------------
int Timer::Framerate( )
{
	return( m_iFramesPerSecond );
}
//--------------------------------------------------------------------------------
float Timer::Runtime( )
{
	return ( (float)((__int64)m_CurrentTicks64 - (__int64)m_StartupTicks64)
		/(__int64)m_TicksPerSecond64);
}
//--------------------------------------------------------------------------------
int Timer::MaxFramerate()
{
	return( m_iMaxFramesPerSecond );
}
//--------------------------------------------------------------------------------