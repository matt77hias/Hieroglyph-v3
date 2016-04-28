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
// Timer
//
//--------------------------------------------------------------------------------
#ifndef Timer_h
#define Timer_h
//--------------------------------------------------------------------------------
namespace Glyph3
{
	class Timer
	{
	public:
		Timer();
		~Timer();

		void Update();
		void Reset();
		float Runtime();
		float Elapsed();
		int Framerate();
		int MaxFramerate();
		int FrameCount();
		float Frametime();

		void SetFixedTimeStep( float step );

	private:
		float m_fDelta;
		int m_iFramesPerSecond;
		int m_iMaxFramesPerSecond;
		int m_iFrameCount;

		float m_fFixedDelta;
		bool m_bUseFixedStep;

		unsigned __int64 m_TicksPerSecond64;
		unsigned __int64 m_StartupTicks64;
		unsigned __int64 m_CurrentTicks64;
		unsigned __int64 m_OneSecTicks64;
		unsigned __int64 m_LastTicks64;
	};
};
//--------------------------------------------------------------------------------
#endif // Timer_h