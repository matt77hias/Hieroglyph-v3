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
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
template < class T >
AnimationStream<T>::AnimationStream()
{
	m_iCurrFrame = 0;
	m_iEndFrame = 0;
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

		// Check for the next keyframe transition
		while ( ( m_vStates[m_iCurrFrame+1].m_fTimeStamp < m_fAnimationTime ) && ( m_iCurrFrame <= m_iEndFrame ) )
		{
			m_iCurrFrame++;
		}

		// Check if we have reached the last frame
		if ( m_iCurrFrame >= m_iEndFrame )
		{
			if ( m_iCurrFrame > m_iEndFrame ) m_iCurrFrame = m_iEndFrame;
			m_bRunning = false;
			m_kCurrState = m_vStates[m_iCurrFrame].m_tData;
		}
		else
		{
			float numerator = m_fAnimationTime - m_vStates[m_iCurrFrame].m_fTimeStamp;
			float denominator = m_vStates[m_iCurrFrame+1].m_fTimeStamp - m_vStates[m_iCurrFrame].m_fTimeStamp;

			if ( denominator <= 0.0f )
				denominator = 0.1f;

			float interpolant = numerator / denominator;

			m_kCurrState = m_vStates[m_iCurrFrame].m_tData * ( 1.0f - interpolant )
				+ m_vStates[m_iCurrFrame+1].m_tData * interpolant;
		}
	}
}
//--------------------------------------------------------------------------------
template < class T >
void AnimationStream<T>::AddState( AnimationState<T>& state )
{
	// Add the state to the dynamic array.

	m_vStates.add( state );

	// If this is the first state to be added, then set it as the current state.
	// This allows the bind pose to be properly calculated without performing an
	// animation play routine.

	if ( m_vStates.count() == 1 )
		m_kCurrState = m_vStates[0].m_tData;
}
//--------------------------------------------------------------------------------
template < class T >
void AnimationStream<T>::Play( float fStartTime, float fEndTime )
{
	m_bRunning = true;
	m_iCurrFrame = 0;
	m_iEndFrame = 0;
	
	// Find the starting frame
	for ( int i = 0; i < m_vStates.count(); i++ )
	{
		if ( fStartTime < m_vStates[i].m_fTimeStamp )
		{
			m_iCurrFrame = i-1;
			break;
		}
	}

	// Find the ending frame
	for ( int i = m_vStates.count()-1; i >= 0; i-- )
	{
		if ( fEndTime >= m_vStates[i].m_fTimeStamp )
		{
			m_iEndFrame = i;
			break;
		}
	}

	// Clamp the values to known good values
	if ( m_iCurrFrame < 0 ) m_iCurrFrame = 0;
	if ( m_iCurrFrame >= m_vStates.count() ) m_iCurrFrame = m_vStates.count();
	if ( m_iEndFrame < 0 ) m_iEndFrame = 0;
	if ( m_iEndFrame >= m_vStates.count() ) m_iEndFrame = m_vStates.count();

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
	m_vAnimations.add( animation );
}
//--------------------------------------------------------------------------------
template < class T >
void AnimationStream<T>::PlayAnimation( int index )
{
	if ( ( index >= 0 ) && ( index < m_vAnimations.count() ) )
		Play( m_vAnimations[index].m_fStartTime, m_vAnimations[index].m_fEndTime );
}
//--------------------------------------------------------------------------------
template < class T >
void AnimationStream<T>::PlayAnimation( std::wstring& name )
{
	for ( int i = 0; i < m_vAnimations.count(); i++ )
	{
		if ( name == m_vAnimations[i].m_Name )
		{
			Play( m_vAnimations[i].m_fStartTime, m_vAnimations[i].m_fEndTime );
			i = m_vAnimations.count();
		}
	}
}
//--------------------------------------------------------------------------------
template < class T >
void AnimationStream<T>::PlayAllAnimations( )
{
	int num = m_vStates.count();

	// Only play this if there is at least one frame in here
	if ( num > 0 )
	{
		Play( m_vStates[0].m_fTimeStamp, m_vStates[num-1].m_fTimeStamp );
	}
}
//--------------------------------------------------------------------------------
template class AnimationStream<Vector3f>;
