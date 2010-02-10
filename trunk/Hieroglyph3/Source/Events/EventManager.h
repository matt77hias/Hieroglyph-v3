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
// EventManager
//
//--------------------------------------------------------------------------------
#ifndef EventManager_h
#define EventManager_h
//--------------------------------------------------------------------------------
#include "IEvent.h"
#include "IEventListener.h"
#include "TArray.h"
//--------------------------------------------------------------------------------
namespace Glyph3
{
	class EventManager
	{
	public:
		EventManager( );
		virtual ~EventManager( );

		bool AddEventListener( eEVENT EventID, IEventListener* pListener );
		bool DelEventListener( eEVENT EventID, IEventListener* pListener );

		bool ProcessEvent( IEvent* pEvent );
		bool QueueEvent( IEvent* pEvent );
		bool ProcessEventQueue( );

		static EventManager* Get( );

	protected:
		TArray< IEventListener* > m_EventHandlers[NUM_EVENTS];
		TArray< IEvent* > m_EventQueue;

		static EventManager* m_spEventManager;
	};
};
//--------------------------------------------------------------------------------
#endif // EventManager_h
