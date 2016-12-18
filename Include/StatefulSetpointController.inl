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
template <typename StateType, typename HostType>
StatefulSetpointController<StateType,HostType>::StatefulSetpointController(
	const StateType& initial_state, std::function<void (HostType*,StateType)> set ) :
		start( initial_state ),
		setpoint( ),
		elapsed( 0.0f ),
		active( false ),
		apply( set )
{
}
//--------------------------------------------------------------------------------
template <typename StateType, typename HostType>
StatefulSetpointController<StateType,HostType>::~StatefulSetpointController()
{
}
//--------------------------------------------------------------------------------
template <typename StateType, typename HostType>
void StatefulSetpointController<StateType,HostType>::Update( float fTime )
{
	if ( !active )
	{
		if ( !setpoint_queue.empty() )
		{
			setpoint = setpoint_queue.front();
			setpoint_queue.pop();
			elapsed = 0.0f;
			active = true;
		}
	}

	if ( active )
	{
		// Calculate the scale to apply to the entity.
		elapsed += fTime;

		// If the duration of the current setpoint has been exceeded, then we
		// apply the setpoint state directly and deactivate.  Otherwise, we
		// interpolate from the start state to the target.
		if ( elapsed >= setpoint.duration )
		{
			elapsed = setpoint.duration;
			start = setpoint.target;
			apply( m_pEntity, setpoint.target );
			active = false;
		}
		else
		{
			float t = elapsed / setpoint.duration;
			StateType vInterpolant = setpoint.tween( start, setpoint.target, t );
			apply( m_pEntity, vInterpolant );
		}
	}
}
//--------------------------------------------------------------------------------
template <typename StateType, typename HostType>
void StatefulSetpointController<StateType,HostType>::AddSetpoint( const StateType& target, float duration )
{
	AddSetpoint( target, duration, Linear<StateType> );
}
//--------------------------------------------------------------------------------
template <typename StateType, typename HostType>
void StatefulSetpointController<StateType,HostType>::AddSetpoint( const StateType& target, float duration, std::function<StateType(StateType,StateType,float)> tween )
{
	Setpoint S;
	S.target = target;
	S.duration = duration;
	S.tween = tween;

	setpoint_queue.push( S );
}
//--------------------------------------------------------------------------------