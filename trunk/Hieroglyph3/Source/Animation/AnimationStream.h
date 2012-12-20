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
// AnimationStream
//--------------------------------------------------------------------------------
#ifndef AnimationStream_h
#define AnimationStream_h
//--------------------------------------------------------------------------------
#include "PCH.h"
#include "Vector3f.h"
//--------------------------------------------------------------------------------
namespace Glyph3
{
	template < class T >
	class AnimationState
	{
	public:
		AnimationState()
		{
			m_fTimeStamp = 0.0f;
		};
		AnimationState( float time, T data )
		{
			m_tData = data;
			m_fTimeStamp = time;
		};

		T		m_tData;			// Data to be interpolated
		float	m_fTimeStamp;		// Time stamp within the current animation.
	};

	class Animation
	{
	public:
		Animation()
		{
			m_Name = L"";
			m_fStartTime = 0;
			m_fEndTime = 0;
		};

		Animation( std::wstring name, float start, float stop )
		{
			m_Name = name;
			m_fStartTime = start;
			m_fEndTime = stop;
		};

		std::wstring m_Name;
		float m_fStartTime;
		float m_fEndTime;
	};

	template < class T >
	class AnimationStream
	{
	public:
		AnimationStream( );
		virtual ~AnimationStream( );

		virtual void Update( float fTime );
		void AddState( AnimationState<T>& pState );
		void Play( float fStartTime, float fEndTime );
		T& GetState();

		void AddAnimation( Animation& animation );
		void PlayAnimation( unsigned int index );
		void PlayAnimation( std::wstring& name );
		void PlayAllAnimations( );

	protected:
		std::vector<AnimationState<T>>	m_vStates;
		T								m_kCurrState;
		unsigned int					m_uiCurrFrame;
		unsigned int 					m_uiEndFrame;

		bool							m_bRunning;
		float							m_fAnimationTime;

		std::vector<Animation>			m_vAnimations;
	};
};
//--------------------------------------------------------------------------------
#endif // AnimationStream_h
