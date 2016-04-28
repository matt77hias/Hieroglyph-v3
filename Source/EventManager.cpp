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
#include "PCH.h"
#include "EventManager.h"
#include "Log.h"
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
EventManager* EventManager::m_spEventManager = 0;
//--------------------------------------------------------------------------------
EventManager::EventManager()
{
	if ( !m_spEventManager )
		m_spEventManager = this;
}
//--------------------------------------------------------------------------------
EventManager::~EventManager()
{
	// TODO: Detach any remaining event handlers that were being serviced

	for ( unsigned int e = 0; e < NUM_EVENTS; e++ ) {
		for ( unsigned int i = 0; i < m_EventHandlers[e].size(); i++ ) {
			m_EventHandlers[e][i]->SetEventManager( nullptr );
		}
	}
}
//--------------------------------------------------------------------------------
EventManager* EventManager::Get()
{
	return( m_spEventManager );
}
//--------------------------------------------------------------------------------
bool EventManager::AddEventListener( eEVENT EventID, IEventListener* pListener )
{
	if ( EventID >= NUM_EVENTS )
		return( false );

	m_EventHandlers[EventID].push_back( pListener );

	return( true );
}
//--------------------------------------------------------------------------------
bool EventManager::DelEventListener( eEVENT EventID, IEventListener* pListener )
{
	if ( EventID >= NUM_EVENTS )
		return( false );

	bool bResult = false;

	int index = -1;
	for ( std::vector< IEventListener* >::iterator it = m_EventHandlers[EventID].begin(); it != m_EventHandlers[EventID].end(); ++it )
	{
		if ( *it == pListener )
		{
			m_EventHandlers[EventID].erase( it );
			bResult = true;
			break;
		}
	}


	return( bResult );
}
//--------------------------------------------------------------------------------
bool EventManager::ProcessEvent( EventPtr pEvent )
{
	if ( !pEvent )
		return( false );

	eEVENT e = pEvent->GetEventType();
	unsigned int num = m_EventHandlers[e].size();

	bool bHandled = false;

	for ( unsigned int i = 0; i < num; i++ )
	{
		bHandled = m_EventHandlers[e][i]->HandleEvent( pEvent );
		if ( bHandled )
			break;
	}

	// The event will automatically be destroyed after exiting this method if no
	// other references to the event have been created.

	return( bHandled );
}
//--------------------------------------------------------------------------------
bool EventManager::QueueEvent( EventPtr pEvent )
{
	// TODO: Events are currently not queued - they are handled immediately after
	//       sending them.  This will need to be addressed in the future...

	return( true );
}
//--------------------------------------------------------------------------------
bool EventManager::ProcessEventQueue( )
{
	// TODO: Events are currently not queued - they are handled immediately after
	//       sending them.  This will need to be addressed in the future...

	return( true );
}
//--------------------------------------------------------------------------------
