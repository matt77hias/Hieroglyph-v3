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
// IEventListener
//
//--------------------------------------------------------------------------------
#ifndef IEventListener_h
#define IEventListener_h
//--------------------------------------------------------------------------------
#include "PCH.h"
#include "IEvent.h"
//--------------------------------------------------------------------------------
namespace Glyph3
{
	class EventManager;

	class IEventListener
	{
	public:
		IEventListener( ); 
		virtual ~IEventListener( );

		virtual std::wstring GetName( ) = 0;
		virtual bool HandleEvent( EventPtr pEvent ) = 0;

		void RequestEvent( eEVENT e );
		void UnRequestEvent( eEVENT e );
		void SetEventManager( EventManager* pEventManager );

	private:
		EventManager*	m_pEventManager;
		std::vector< eEVENT > m_RequestedEvents;
	};
};
//--------------------------------------------------------------------------------
#endif // IEventListener_h
