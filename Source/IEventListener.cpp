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
#include "IEventListener.h"
#include "EventManager.h"
#include "Log.h"
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
IEventListener::IEventListener()
	: m_pEventManager( nullptr )
{
}
//--------------------------------------------------------------------------------
IEventListener::~IEventListener()
{
	// Disconnect from any events that may have been requested before.  This is
	// accomplished by setting the event manager to null, which will automatically
	// de-register all of this object's event listeners from it.

	SetEventManager( nullptr );
}
//--------------------------------------------------------------------------------
void IEventListener::RequestEvent( eEVENT e )
{
	// Check if this event has already been requested
	bool bAlreadyRequested = false;

	for ( auto requested : m_RequestedEvents ) {
		if ( requested == e )
			bAlreadyRequested = true;
	}

	if ( bAlreadyRequested ) {
		
		Log::Get().Write( L"WARNING: Trying to request an event that has already been added!!!" );
	
	} else {
	
		// If it isn't already there, then add it to the list of events to receive
		m_RequestedEvents.push_back( e );

		// Request the event from the currently assigned event manager
		if ( m_pEventManager ) {
			m_pEventManager->AddEventListener( e, this );
		}
	}
}
//--------------------------------------------------------------------------------
void IEventListener::UnRequestEvent( eEVENT e )
{
	// Remove any instances of the event from the requested list.

	for ( std::vector<eEVENT>::iterator it = m_RequestedEvents.begin(); it != m_RequestedEvents.end(); ++it )
	{
		if ( *it == e )
		{
			it = m_RequestedEvents.erase( it );
		}
	}

	// Remove the event from the currently assigned event manager
	if ( m_pEventManager )
	{
		m_pEventManager->DelEventListener( e, this );
	}
}
//--------------------------------------------------------------------------------
void IEventListener::SetEventManager( EventManager* pEventManager )
{
	// Remove any currently requested events from the existing event manager
	if ( m_pEventManager )
	{
		for ( auto e : m_RequestedEvents )
		{
			m_pEventManager->DelEventListener( e, this );
		}
	}

	// Set the new event manager
	m_pEventManager = pEventManager;

	// Request all of the desired events from the new event manager
	if ( m_pEventManager ) 
	{
		for ( auto e : m_RequestedEvents )
		{
			m_pEventManager->AddEventListener( e, this );
		}
	}
}
//--------------------------------------------------------------------------------