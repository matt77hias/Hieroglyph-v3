//--------------------------------------------------------------------------------
// This file is a portion of the Hieroglyph 3 Rendering Engine.  It is distributed
// under the MIT License, available in the root of this distribution and 
// at the following URL:
//
// http://www.opensource.org/licenses/mit-license.php
//
// Copyright (c) 2003-2010 Jason Zink 
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
	else
		Log::Get().Write( L"ERROR: Someone create more than one EventManagers!" );
}
//--------------------------------------------------------------------------------
EventManager::~EventManager()
{
	// TODO: Detach any remaining event handlers that were being serviced

	//for ( int e = 0; e < NUM_EVENTS; e++ ) {
	//	for ( int i = 0; i < m_EventHandlers[e].count(); i++ ) {

	//	}
	//}
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

	m_EventHandlers[EventID].add( pListener );

	return( true );
}
//--------------------------------------------------------------------------------
bool EventManager::DelEventListener( eEVENT EventID, IEventListener* pListener )
{
	if ( EventID >= NUM_EVENTS )
		return( false );

	bool bResult = false;

	int index = m_EventHandlers[EventID].find( pListener );		

	if ( index != -1 )
	{
		m_EventHandlers[EventID].remove( index );
		bResult = true;
	}

	return( bResult );
}
//--------------------------------------------------------------------------------
bool EventManager::ProcessEvent( IEvent* pEvent )
{
	if ( !pEvent )
		return( false );

	eEVENT e = pEvent->GetEventType();
	int num = m_EventHandlers[e].count();

	bool bHandled = false;

	for ( int i = 0; i < num; i++ )
	{
		bHandled = m_EventHandlers[e][i]->HandleEvent( pEvent );
		if ( bHandled )
			break;
	}

	// Delete the event after processing.  This may need to check the handled
	// flag to see if it should be re-queued or not...

	delete pEvent;

	return( bHandled );
}
//--------------------------------------------------------------------------------
bool EventManager::QueueEvent( IEvent* pEvent )
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
std::wstring EventManager::GetName( )
{
	return( L"EventManager" );
}
//--------------------------------------------------------------------------------
bool EventManager::HandleEvent( IEvent* pEvent )
{
	// If an event manager receives an event, it will simply forward the event
	// to its own listeners until someone handles it.  In this scenario, the 
	// handling is slightly different than if the event was originally posted to
	// this manager since the event lifetime will be controlled by the originator.

	if ( !pEvent )
		return( false );

	eEVENT e = pEvent->GetEventType();
	int num = m_EventHandlers[e].count();

	bool bHandled = false;

	// Loop through each listener and allow them to handle the event.  Halt the
	// processing if the event is handled.

	for ( int i = 0; i < num; i++ )
	{
		bHandled = m_EventHandlers[e][i]->HandleEvent( pEvent );
		if ( bHandled )
			break;
	}

	// Do not delete the event after processing.  Instead, return the 'handled'
	// result and let the original event manager take care of it.

	return( bHandled );
}
//--------------------------------------------------------------------------------