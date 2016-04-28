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
template <typename T>
RiftController<T>::RiftController( RiftHMDPtr hmd ) :
	m_Hmd( hmd )
{
}
//--------------------------------------------------------------------------------
template <typename T>
RiftController<T>::~RiftController()
{
}
//--------------------------------------------------------------------------------
template <typename T>
void RiftController<T>::Update( float fTime )
{
	// This controller has one simple mission in life - if it is attached to an
	// entity, then it should get the orientation from the Rift and apply it to 
	// the entity.

	if ( m_pEntity )
	{
		m_pEntity->Transform.Rotation() = m_Hmd->GetOrientation( 0.0 );
	}
}
//--------------------------------------------------------------------------------
