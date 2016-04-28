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
template <typename T, typename U>
SetpointController<T,U>::SetpointController(
	std::function<T (U*)> get, 
	std::function<void (U*,T)> set ) :
	m_vStartpoint( ),
	m_Setpoint( ),
	m_fElapsed( 0.0f ),
	m_bActive( false ),
	m_get(get),
	m_set(set),
	m_default(Linear<T>)
{
}
//--------------------------------------------------------------------------------
template <typename T, typename U>
SetpointController<T,U>::~SetpointController()
{
}
//--------------------------------------------------------------------------------
template <typename T, typename U>
void SetpointController<T,U>::Update( float fTime )
{
	if ( !m_bActive )
	{
		if ( !m_SetpointQueue.empty() )
		{
			m_Setpoint = m_SetpointQueue.front();
			m_SetpointQueue.pop();

			m_vStartpoint = m_get( m_pEntity );
			m_fElapsed = 0.0f;

			m_bActive = true;
		}
	}

	if ( m_bActive )
	{
		// Calculate the scale to apply to the entity.
		m_fElapsed += fTime;

		if ( m_fElapsed >= m_Setpoint.duration )
		{
			m_fElapsed = m_Setpoint.duration;
			m_bActive = false;
		}

		float t = m_fElapsed / m_Setpoint.duration;
	
		T vInterpolant = m_Setpoint.tween( m_vStartpoint, m_Setpoint.target, t );

		m_set( m_pEntity, vInterpolant );
	}
}
//--------------------------------------------------------------------------------
template <typename T, typename U>
void SetpointController<T,U>::AddSetpoint( const T& target, float duration )
{
	AddSetpoint( target, duration, m_default );
}
//--------------------------------------------------------------------------------
template <typename T, typename U>
void SetpointController<T,U>::AddSetpoint( const T& target, float duration, std::function<T(T,T,float)> tween )
{
	Setpoint S;
	S.target = target;
	S.duration = duration;
	S.tween = tween;

	m_SetpointQueue.push( S );
}
//--------------------------------------------------------------------------------