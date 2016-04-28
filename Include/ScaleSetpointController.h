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
// ScaleSetpointController
//
//--------------------------------------------------------------------------------
#ifndef ScaleSetpointController_h
#define ScaleSetpointController_h
//--------------------------------------------------------------------------------
#include "IController.h"
#include "Vector3f.h"
//--------------------------------------------------------------------------------
namespace Glyph3
{
	template <typename T>
	class ScaleSetpointController : public IController<T>
	{
	public:
		ScaleSetpointController( );
		virtual ~ScaleSetpointController( );

		virtual void Update( float fTime );

		void SetSetpoint( const Vector3f& scale, float duration );

	protected:
		Vector3f		m_vStartpoint;
		Vector3f		m_vSetpoint;
		float			m_fDuration;
		float			m_fElapsed;
		bool			m_bActive;
	};

	#include "ScaleSetpointController.inl"
};
//--------------------------------------------------------------------------------
#endif // ScaleSetpointController_h
