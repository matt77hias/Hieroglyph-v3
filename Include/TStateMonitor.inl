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
template <class T>
TStateMonitor<T>::TStateMonitor( T initialState ) : 
	m_bUploadNeeded( false ),
	m_InitialState( initialState ),
	m_State( initialState ),
	m_pSister( nullptr )
{
	InitializeState();
	ResetTracking();
}
//--------------------------------------------------------------------------------
template <class T>
TStateMonitor<T>::~TStateMonitor()
{
}
//--------------------------------------------------------------------------------
template <class T>
void TStateMonitor<T>::SetSister( TStateMonitor<T>* pSister )
{
	m_pSister = pSister;
}
//--------------------------------------------------------------------------------
template <class T>
void TStateMonitor<T>::SetState( T state )
{
	m_State = state;

	// Ensure that a sister state has been set, and if so then use it to manage
	// the choice of if the state needs to be set in the next update to the pipeline.

	// If there is no sister state, then we default to always requiring an upload
	// of the state.

	if ( m_pSister == nullptr )
	{
		m_bUploadNeeded = true;

		return;
	}


	// Since we are only managing a single state, we can update the status based 
	// on a simple compare after the state has been updated.

	m_bUploadNeeded = !SameAsSister();

}
//--------------------------------------------------------------------------------
template <class T>
bool TStateMonitor<T>::SameAsSister()
{
	return( m_State == m_pSister->m_State );
}
//--------------------------------------------------------------------------------
template <class T>
bool TStateMonitor<T>::IsUpdateNeeded()
{
	return( m_bUploadNeeded );
}
//--------------------------------------------------------------------------------
template <class T>
void TStateMonitor<T>::InitializeState()
{
	SetState( m_InitialState );
}
//--------------------------------------------------------------------------------
template <class T>
void TStateMonitor<T>::ResetTracking()
{
	m_bUploadNeeded = false;
}
//--------------------------------------------------------------------------------
template <class T>
T TStateMonitor<T>::GetState() const
{
	return( m_State );
}
//--------------------------------------------------------------------------------
