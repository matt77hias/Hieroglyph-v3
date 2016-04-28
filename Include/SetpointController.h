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
// SetpointController
//
//--------------------------------------------------------------------------------
#ifndef SetpointController_h
#define SetpointController_h
//--------------------------------------------------------------------------------
#include "IController.h"
#include <functional>
#include <queue>
//#include "Entity3D.h"
#include "Tween.h"
//--------------------------------------------------------------------------------
namespace Glyph3
{
	template <typename T, typename U>
	class SetpointController : public IController<U>
	{
	public:
		SetpointController( 
			std::function<T(U*)> get, 
			std::function<void(U*,T)> set );
		
		virtual ~SetpointController( );

		virtual void Update( float fTime );

		void AddSetpoint( const T& target, float duration );
		void AddSetpoint( const T& target, float duration, std::function<T(T,T,float)> tween );

		void SetDefaultTween( std::function<T(T,T,float)> tween );

	protected:
		struct Setpoint {
			T target;
			float duration;
			std::function<T(T,T,float)> tween;
		};

		Setpoint		m_Setpoint;
		T				m_vStartpoint;
		float			m_fElapsed;
		bool			m_bActive;

		std::function<T(U*)>			m_get;
		std::function<void(U*,T)>	m_set;
		std::function<T(T,T,float)>			m_default;


		std::queue<Setpoint> m_SetpointQueue;
	};

	#include "SetpointController.inl"
};
//--------------------------------------------------------------------------------
#endif // SetpointController_h
//--------------------------------------------------------------------------------