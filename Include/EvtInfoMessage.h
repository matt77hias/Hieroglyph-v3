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
// EvtInfoMessage
//
// The info message event is used to allow the engine to provide information that
// is not critical to the user.  This message should be displayed by the 
// application, and then the application can continue on.
//--------------------------------------------------------------------------------
#ifndef EvtInfoMessage_h
#define EvtInfoMessage_h
//--------------------------------------------------------------------------------
#include "IEvent.h"
//--------------------------------------------------------------------------------
namespace Glyph3
{
	class EvtInfoMessage : public IEvent
	{
	public:
		EvtInfoMessage( std::wstring& message );
		EvtInfoMessage( const wchar_t* message );
		virtual ~EvtInfoMessage( );

		virtual std::wstring GetEventName( );
		virtual eEVENT GetEventType( );

		std::wstring& GetInfoMessage( );

	protected:
		std::wstring m_Message;
	};

	typedef std::shared_ptr<EvtInfoMessage> EvtInfoMessagePtr;
};
//--------------------------------------------------------------------------------
#endif // EvtInfoMessage_h
