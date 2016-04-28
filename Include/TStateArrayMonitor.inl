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
template <class T, unsigned int N>
TStateArrayMonitor<T,N>::TStateArrayMonitor( T initialState ) : 
	m_uiStartSlot( 0 ),
	m_uiEndSlot( 0 ),
	m_bUploadNeeded( false ),
	m_InitialState( initialState ),
	m_pSister( nullptr )
{
	InitializeStates();
	ResetTracking();
}
//--------------------------------------------------------------------------------
template <class T, unsigned int N>
TStateArrayMonitor<T,N>::~TStateArrayMonitor()
{
}
//--------------------------------------------------------------------------------
template <class T, unsigned int N>
void TStateArrayMonitor<T,N>::SetSister( TStateArrayMonitor<T,N>* pSister )
{
	m_pSister = pSister;
}
//--------------------------------------------------------------------------------
template <class T, unsigned int N>
void TStateArrayMonitor<T,N>::SetState( unsigned int slot, T state )
{
	assert( slot < N );

	m_States[slot] = state;

	// Ensure that a sister state has been set, and if so then use it to manage
	// which slots need to be set in the next update to the pipeline.

	// If there is no sister state, then we default to always requiring an upload
	// of the entire array.

	if ( m_pSister == nullptr )
	{
		m_bUploadNeeded = true;
		m_uiStartSlot = 0;
		m_uiEndSlot = N-1;

		return;
	}

	// Our actions will depend on if this new state matches the sister state or not.

	bool bSameAsSister = SameAsSister( slot );


	// Begin by checking if any states changes are already pending.  If not, then if
	// the current state change differs from the sister state, then we have a new 
	// start and end slot.

	if ( !m_bUploadNeeded && !bSameAsSister )
	{
		m_bUploadNeeded = true;
		m_uiStartSlot = slot;
		m_uiEndSlot = slot;
	}


	// If there is already an upload needed, then we need to update the status of
	// our list.

	if ( m_bUploadNeeded )
	{
		// Updating slot below the current range
		if ( slot < m_uiStartSlot )
		{
			// If the slot is lower than our current start slot, then if the state is
			// different we should consider this the new start state.
			if ( !bSameAsSister ) {
				m_uiStartSlot = slot;
			}
		}
		// Updating slot at the lower end point
		else if ( slot == m_uiStartSlot )
		{
			// If the states don't match, then we retain the same starting slot.
			// Otherwise, we search toward the end state and see if we can reduce
			// the range of the states that need to be set.
			if ( bSameAsSister ) {
				SearchFromBelow();
			}
		}
		// Updating slot inside the current range
		else if ( m_uiStartSlot < slot && slot < m_uiEndSlot ) 
		{
			// If the change is between our two endpoints, it doesn't matter.  
			// Regardless of if they are the same or not, we still have to bind
			// the same number of states anyways.
		}
		// Updating slot at the upper end point
		else if ( slot == m_uiEndSlot )
		{
			// If the states don't match, then we retain the same ending slot.
			// Otherwise, we search toward the start slot and see if we can reduce
			// the range of the states that need to be set.
			if ( bSameAsSister ) {
				SearchFromAbove();
			}
		}
		// Updating slot above the current range
		else if ( m_uiEndSlot < slot )
		{
			// If the slot is lower than our current start slot, then if the state is
			// different we should consider this the new start state.
			if ( !bSameAsSister ) {
				m_uiEndSlot = slot;
			}
		}
	}
}
//--------------------------------------------------------------------------------
template <class T, unsigned int N>
void TStateArrayMonitor<T,N>::SearchFromBelow()
{
	// Here we start from the current start slot location, and iterate upward
	// until we encounter the end slot.  As soon as we find a difference, we
	// terminate the iteration, which will automatically make the start slot update
	// to the appropriate location.
	//
	// Note that the iteration will not allow the StartSlot and EndSlot to go past
	// eachother.  Instead, they can end at the same location, and then the final
	// check is made after the loop to see if that last slot is the same or not.

	for ( ; m_uiStartSlot < m_uiEndSlot; m_uiStartSlot++ )
	{
		if ( !SameAsSister( m_uiStartSlot ) )
			break;
	}

	// If no differences have been found, we compare the last available slot.  If
	// they are the same, we update the status to reflect that.  This means 
	// resetting the update flag and the slot markers.
	
	if ( m_uiStartSlot == m_uiEndSlot && SameAsSister( m_uiStartSlot ) ) {
		ResetTracking();
	}
}
//--------------------------------------------------------------------------------
template <class T, unsigned int N>
void TStateArrayMonitor<T,N>::SearchFromAbove()
{
	// Here we start from the current end slot location, and iterate downward
	// until we encounter the start slot.  As soon as we find a difference, we
	// terminate the iteration, which will automatically make the end slot update
	// to the appropriate location.
	//
	// Note that the iteration will not allow the StartSlot and EndSlot to go past
	// eachother.  Instead, they can end at the same location, and then the final
	// check is made after the loop to see if that last slot is the same or not.

	for ( ; m_uiEndSlot > m_uiStartSlot; m_uiEndSlot-- )
	{
		if ( !SameAsSister( m_uiEndSlot ) )
			break;
	}

	// If no differences have been found, we compare the last available slot.  If
	// they are the same, we update the status to reflect that.  This means 
	// resetting the update flag and the slot markers.
	
	if ( m_uiStartSlot == m_uiEndSlot && SameAsSister( m_uiEndSlot ) ) {
		ResetTracking();
	}
}
//--------------------------------------------------------------------------------
template <class T, unsigned int N>
bool TStateArrayMonitor<T,N>::SameAsSister( unsigned int slot )
{
	assert( slot < N );

	return( m_States[slot] == m_pSister->m_States[slot] );
}
//--------------------------------------------------------------------------------
template <class T, unsigned int N>
bool TStateArrayMonitor<T,N>::IsUpdateNeeded()
{
	return( m_bUploadNeeded );
}
//--------------------------------------------------------------------------------
template <class T, unsigned int N>
unsigned int TStateArrayMonitor<T,N>::GetStartSlot()
{
	return( m_uiStartSlot );
}
//--------------------------------------------------------------------------------
template <class T, unsigned int N>
unsigned int TStateArrayMonitor<T,N>::GetEndSlot()
{
	return( m_uiEndSlot );
}
//--------------------------------------------------------------------------------
template <class T, unsigned int N>
unsigned int TStateArrayMonitor<T,N>::GetRange()
{
	return( m_uiEndSlot - m_uiStartSlot + 1 );
}
//--------------------------------------------------------------------------------
template <class T, unsigned int N>
void TStateArrayMonitor<T,N>::InitializeStates()
{
	for ( unsigned int i = 0; i < N; i++ )
		SetState( i, m_InitialState );
}
//--------------------------------------------------------------------------------
template <class T, unsigned int N>
void TStateArrayMonitor<T,N>::ResetTracking()
{
	m_uiStartSlot = 0;
	m_uiEndSlot = 0;
	m_bUploadNeeded = false;
}
//--------------------------------------------------------------------------------
template <class T, unsigned int N>
T TStateArrayMonitor<T,N>::GetState( unsigned int slot ) const
{
	assert( slot < N );

	return( m_States[slot] );
}
//--------------------------------------------------------------------------------
template <class T, unsigned int N>
T* TStateArrayMonitor<T,N>::GetFirstSlotLocation()
{
	return( &m_States[m_uiStartSlot] );
}
//--------------------------------------------------------------------------------
template <class T, unsigned int N>
T* TStateArrayMonitor<T,N>::GetSlotLocation( unsigned int slot )
{
	assert( slot < N );

	return( &m_States[slot] );
}
//--------------------------------------------------------------------------------
