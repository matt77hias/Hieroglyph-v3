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
// RiftController
//
// This is a controller that reads spatial data from the Oculus Rift and applies
// it to the entity that it is attached to.
//--------------------------------------------------------------------------------
#ifndef RiftController_h
#define RiftController_h
//--------------------------------------------------------------------------------
#include "IController.h"
#include <memory>
#include "RiftHMD.h"
//--------------------------------------------------------------------------------
namespace Glyph3
{
	template <typename T>
	class RiftController : public IController<T>
	{
	public:
		RiftController( RiftHMDPtr hmd );
		virtual ~RiftController( );
		virtual void Update( float fTime );

	private:
		RiftHMDPtr m_Hmd;
	};

	#include "RiftController.inl"
};
//--------------------------------------------------------------------------------
#endif // RiftController_h
//--------------------------------------------------------------------------------