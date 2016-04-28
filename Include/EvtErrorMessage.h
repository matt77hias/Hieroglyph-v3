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
// EvtErrorMessage
//
// The error message is intended to relay a message to the application (or whoever
// else wants to receive it).  The desired behavior is that the application should
// display the message and then terminate itself.
//--------------------------------------------------------------------------------
#ifndef EvtErrorMessage_h
#define EvtErrorMessage_h
//--------------------------------------------------------------------------------
#include "IEvent.h"
//--------------------------------------------------------------------------------
namespace Glyph3
{
	class EvtErrorMessage : public IEvent
	{
	public:
		EvtErrorMessage( std::wstring& message );
		EvtErrorMessage( const wchar_t* message );
		virtual ~EvtErrorMessage( );

		virtual std::wstring GetEventName( );
		virtual eEVENT GetEventType( );

		std::wstring& GetErrorMessage( );

	protected:
		std::wstring m_Message;
	};

	typedef std::shared_ptr<EvtErrorMessage> EvtErrorMessagePtr;
};
//--------------------------------------------------------------------------------
#endif // EvtErrorMessage_h
