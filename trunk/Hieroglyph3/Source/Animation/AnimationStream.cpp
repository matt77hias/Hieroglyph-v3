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
#include "AnimationStream.h"
#include "Log.h"
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
template < class T >
AnimationStream<T>::AnimationStream()
{
	m_uiCurrFrame = 0;
	m_uiEndFrame = 0;
	m_bRunning = false;
	m_fAnimationTime = 0.0f;
}
//--------------------------------------------------------------------------------
template < class T >
AnimationStream<T>::~AnimationStream()
{

}
//--------------------------------------------------------------------------------
template < class T >
void AnimationStream<T>::Update( float fTime )
{
	if ( m_bRunning )
	{
		// Update the time of the animation
		m_fAnimationTime += fTime;

		// Check for the next keyframe transition.  This is only valid if there
		// is another state to transition to!  If the current animation time has
		// exceeded the next state's time stamp, then increment to the next state.

		if ( m_uiCurrFrame+1 < m_vStates.size() ) 
		{
			while ( ( m_uiCurrFrame < m_uiEndFrame ) && ( m_vStates[m_uiCurrFrame+1].m_fTimeStamp < m_fAnimationTime ) )
			{
				m_uiCurrFrame++;
			}
		}

		// Check if we have reached the last frame.  If so, we stop the animation
		// mode and set the output value to the final state.

		if ( m_uiCurrFrame >= m_uiEndFrame )
		{
			if ( m_uiCurrFrame > m_uiEndFrame ) m_uiCurrFrame = m_uiEndFrame;
			m_bRunning = false;
			m_kCurrState = m_vStates[m_uiCurrFrame].m_tData;
		}
		else
		{
			// Perform interpolation between the current frame andt he next frame
			// to find the current state.

			float numerator = m_fAnimationTime - m_vStates[m_uiCurrFrame].m_fTimeStamp;
			float denominator = m_vStates[m_uiCurrFrame+1].m_fTimeStamp - m_vStates[m_uiCurrFrame].m_fTimeStamp;

			if ( denominator <= 0.0f )
				denominator = 0.1f;

			float interpolant = numerator / denominator;

			m_kCurrState = m_vStates[m_uiCurrFrame].m_tData * ( 1.0f - interpolant )
				+ m_vStates[m_uiCurrFrame+1].m_tData * interpolant;
		}
	}
}
//--------------------------------------------------------------------------------
template < class T >
void AnimationStream<T>::AddState( AnimationState<T>& state )
{
	// Add the state to the dynamic array.

	m_vStates.push_back( state );

	// If this is the first state to be added, then set it as the current state.
	// This allows the bind pose to be properly calculated without performing an
	// animation play routine.

	if ( m_vStates.size() == 1 )
		m_kCurrState = m_vStates[0].m_tData;
}
//--------------------------------------------------------------------------------
template < class T >
void AnimationStream<T>::Play( float fStartTime, float fEndTime )
{
	// Ensure that there has been at least one state added...
	if ( m_vStates.size() == 0 )
	{
		Log::Get().Write( L"ERROR: Trying to play an animation without any states!!!" );
		return;
	}

	// If we have at least one state, then initialize our internal variables to use
	// the first state.

	m_bRunning = true;
	m_uiCurrFrame = 0;
	m_uiEndFrame = 0;

	// If there are more then one state, search to find where to start and end.  
	// Note that the bounds on the for-loop restricts which indices are used to a
	// valid range!

	if ( m_vStates.size() > 1 ) 
	{
		// Find the starting frame
		for ( m_uiCurrFrame = 0; m_uiCurrFrame < m_vStates.size() - 1; m_uiCurrFrame++ )
		{
			if ( m_vStates[m_uiCurrFrame].m_fTimeStamp <= fStartTime && fStartTime < m_vStates[m_uiCurrFrame+1].m_fTimeStamp )
			{
				break;
			}
		}

		// Find the ending frame
		for ( m_uiEndFrame = m_vStates.size()-1; m_uiEndFrame > 0; m_uiEndFrame-- )
		{
			if ( m_vStates[m_uiEndFrame-1].m_fTimeStamp < fEndTime && fEndTime <= m_vStates[m_uiEndFrame].m_fTimeStamp )
			{
				break;
			}
		}
	}

	m_fAnimationTime = fStartTime;
}
//--------------------------------------------------------------------------------
template < class T >
T& AnimationStream<T>::GetState()
{
	return( m_kCurrState );
}
//--------------------------------------------------------------------------------
template < class T >
void AnimationStream<T>::AddAnimation( Animation& animation )
{
	m_vAnimations.push_back( animation );
}
//--------------------------------------------------------------------------------
template < class T >
void AnimationStream<T>::PlayAnimation( unsigned int index )
{
	if ( index < m_vAnimations.size() )
		Play( m_vAnimations[index].m_fStartTime, m_vAnimations[index].m_fEndTime );
}
//--------------------------------------------------------------------------------
template < class T >
void AnimationStream<T>::PlayAnimation( std::wstring& name )
{
	for ( unsigned int i = 0; i < m_vAnimations.size(); i++ )
	{
		if ( name == m_vAnimations[i].m_Name )
		{
			Play( m_vAnimations[i].m_fStartTime, m_vAnimations[i].m_fEndTime );
			i = m_vAnimations.size();
		}
	}
}
//--------------------------------------------------------------------------------
template < class T >
void AnimationStream<T>::PlayAllAnimations( )
{
	int num = m_vStates.size();

	// Only play this if there is at least one frame in here
	if ( num > 0 )
	{
		Play( m_vStates[0].m_fTimeStamp, m_vStates[num-1].m_fTimeStamp );
	}
}
//--------------------------------------------------------------------------------
template class Glyph3::AnimationStream<Vector3f>;
