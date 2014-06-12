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
#include "RiftController.h"
#include "Entity3D.h"
#include "Log.h"
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
RiftController::RiftController( RiftHMDPtr hmd ) :
	m_Hmd( hmd )
{
}
//--------------------------------------------------------------------------------
RiftController::~RiftController()
{
}
//--------------------------------------------------------------------------------
void RiftController::Update( float fTime )
{
	// This controller has one simple mission in life - if it is attached to an
	// entity, then it should get the orientation from the Rift and apply it to 
	// the entity.

	if ( m_pEntity )
	{
		m_pEntity->Rotation() = m_Hmd->GetOrientation( 0.0 );
	}
}
//--------------------------------------------------------------------------------
