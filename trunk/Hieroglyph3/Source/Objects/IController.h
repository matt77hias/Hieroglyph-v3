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
// IController
//
// The controller is the basic unit of animation.  The concept used in developing
// this class was taken from the work of Dave Eberly's Game Engine books.  The
// controller is intended to be attached to an entity, and used to vary some
// property with time.
//
// The animated property will be updated during the scene graph update phase.
//--------------------------------------------------------------------------------
#ifndef IController_h
#define IController_h
//--------------------------------------------------------------------------------
#include "Timer.h"
//--------------------------------------------------------------------------------
namespace Glyph3
{
	class Entity3D;

	class IController
	{
	public:
		IController( );
		virtual ~IController( );
		virtual void Update( float fTime ) = 0;

		void SetEntity( Entity3D* pObject );
		Entity3D* GetEntity( );

	protected:
		Entity3D* m_pEntity;

	};
};
//--------------------------------------------------------------------------------
#endif // IController_h
