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
// StatefulSetpointController
//
//--------------------------------------------------------------------------------
#ifndef StatefulSetpointController_h
#define StatefulSetpointController_h
//--------------------------------------------------------------------------------
#include "IController.h"
#include <functional>
#include <queue>
#include "Tween.h"
//--------------------------------------------------------------------------------
namespace Glyph3
{
	template <typename StateType, typename HostType>
	class StatefulSetpointController : public IController<HostType>
	{
	public:
		StatefulSetpointController( 
			const StateType& initial_state,
			std::function<void(HostType*,StateType)> apply );
		
		virtual ~StatefulSetpointController( );

		virtual void Update( float fTime );

		void AddSetpoint( const StateType& target, float duration );
		void AddSetpoint( const StateType& target, float duration, std::function<StateType(StateType,StateType,float)> tween );

		void SetDefaultTween( std::function<StateType(StateType,StateType,float)> tween );

	protected:
		struct Setpoint {
			StateType target;
			float duration;
			std::function<StateType(StateType,StateType,float)> tween;
		};

		Setpoint		setpoint;
		StateType		start;
		float			elapsed;
		bool			active;

		std::function<void(HostType*,StateType)>			apply;

		std::queue<Setpoint> setpoint_queue;
	};

	#include "StatefulSetpointController.inl"
};
//--------------------------------------------------------------------------------
#endif // StatefulSetpointController_h
//--------------------------------------------------------------------------------