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
// AnimationStream
//--------------------------------------------------------------------------------
#ifndef AnimationStream_h
#define AnimationStream_h
//--------------------------------------------------------------------------------
#include "PCH.h"
#include "Animation.h"
#include "Vector3f.h"
#include "Log.h"
#include "Tween.h"
#include <functional>
//--------------------------------------------------------------------------------
namespace Glyph3
{
	template < class T >
	class AnimationState
	{
	public:
		AnimationState() : m_fTimeStamp( 0.0f ), m_tData() { };
		AnimationState( float time, T data ) : m_tData( data ), m_fTimeStamp( time ) { };

		T		m_tData;			// Data to be interpolated
		float	m_fTimeStamp;		// Time stamp within the current animation.
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
		void PlayAnimation( size_t index );
		void PlayAnimation( std::wstring& name );
		void PlayAllAnimations( );

		void SetInterpolationMethod( std::function<T(const T&,const T&,float)> func );

	protected:
		std::vector<AnimationState<T>>					m_vStates;
		T												m_kCurrState;
		size_t											m_CurrFrame;
		size_t		 									m_EndFrame;

		bool											m_bRunning;
		float											m_fAnimationTime;

		std::function<T(const T&,const T&,float)>		m_tweenFunc;
		std::vector<Animation>							m_vAnimations;
	};

	#include "AnimationStream.inl"
};
//--------------------------------------------------------------------------------
#endif // AnimationStream_h
//--------------------------------------------------------------------------------
