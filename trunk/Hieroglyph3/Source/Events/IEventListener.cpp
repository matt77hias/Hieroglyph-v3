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
#include "IEventListener.h"
#include "EventManager.h"
#include "Log.h"
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
IEventListener::IEventListener()
{
	// We default to using the 'main' event manager.  If there is another one 
	// specified later on, then that will override this association.

	SetEventManager( EventManager::Get() );
}
//--------------------------------------------------------------------------------
IEventListener::~IEventListener()
{
	// Disconnect from any events that may have been requested before.

	if ( m_pEventManager ) {
		for ( int i = 0; i < m_RequestedEvents.count(); i++ ) {
			m_pEventManager->DelEventListener( m_RequestedEvents[i], this );
		}
	}
}
//--------------------------------------------------------------------------------
void IEventListener::RequestEvent( eEVENT e )
{
	// Check if this event has already been requested
	if ( m_RequestedEvents.contains( e ) ) {
		
		Log::Get().Write( L"WARNING: Trying to request an event that has already been added!!!" );
	
	} else {
	
		// If it isn't already there, then add it to the list of events to receive
		m_RequestedEvents.add( e );

		// Request the event from the currently assigned event manager
		if ( m_pEventManager ) {
			m_pEventManager->AddEventListener( e, this );
		}
	}
}
//--------------------------------------------------------------------------------
void IEventListener::UnRequestEvent( eEVENT e )
{
	// Check if this event has already been requested
	if ( !m_RequestedEvents.contains( e ) ) {
		
		Log::Get().Write( L"WARNING: Trying to un-request an event that hasn't already been added!!!" );
	
	} else {

		// If so, then remove it from the list
		m_RequestedEvents.remove( m_RequestedEvents.find( e ) );

		// Remove the event from the currently assigned event manager
		if ( m_pEventManager ) {
			m_pEventManager->DelEventListener( e, this );
		}
	}
}
//--------------------------------------------------------------------------------
void IEventListener::SetEventManager( EventManager* pEventManager )
{
	// Remove any currently requested events from the existing event manager
	
	if ( m_pEventManager ) {
		for ( int i = 0; i < m_RequestedEvents.count(); i++ ) {
			m_pEventManager->DelEventListener( m_RequestedEvents[i], this );
		}
	}

	// Set the new event manager
	m_pEventManager = pEventManager;

	// Request all of the desired events from the new event manager
	if ( m_pEventManager ) {
		for ( int i = 0; i < m_RequestedEvents.count(); i++ ) {
			m_pEventManager->AddEventListener( m_RequestedEvents[i], this );
		}
	}
}
//--------------------------------------------------------------------------------