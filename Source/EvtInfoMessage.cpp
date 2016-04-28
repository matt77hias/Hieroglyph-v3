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
#include "EvtInfoMessage.h"
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
EvtInfoMessage::EvtInfoMessage( std::wstring& message )
{
	m_Message = message;
}
//--------------------------------------------------------------------------------
EvtInfoMessage::EvtInfoMessage( const wchar_t* pMessage )
{
	m_Message = std::wstring( pMessage );
}
//--------------------------------------------------------------------------------
EvtInfoMessage::~EvtInfoMessage( )
{
}
//--------------------------------------------------------------------------------
std::wstring EvtInfoMessage::GetEventName( )
{
	return( std::wstring( L"info_message" ) );
}
//--------------------------------------------------------------------------------
eEVENT EvtInfoMessage::GetEventType( )
{
	return( INFO_MESSAGE );
}
//--------------------------------------------------------------------------------
std::wstring& EvtInfoMessage::GetInfoMessage( )
{
	return( m_Message );
}
//--------------------------------------------------------------------------------