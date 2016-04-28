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
// RotationController
//
//--------------------------------------------------------------------------------
#ifndef RotationController_h
#define RotationController_h
//--------------------------------------------------------------------------------
#include "IController.h"
#include "AnimationStream.h"
#include "Matrix4f.h"
//--------------------------------------------------------------------------------
namespace Glyph3
{
	template <typename T>
	class RotationController : public IController<T>
	{
	public:
		RotationController( );
		RotationController( Vector3f axis, float speed );
		virtual ~RotationController( );
		virtual void Update( float fTime );

		void SetAxis( Vector3f axis );
		void SetSpeed( float speed );
		
	protected:
		float			m_fSpeed;
		Vector3f		m_kAxis;
	};

	#include "RotationController.inl"
};
//--------------------------------------------------------------------------------
#endif // RotationController_h
