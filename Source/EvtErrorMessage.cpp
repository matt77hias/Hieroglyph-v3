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
#include "EvtErrorMessage.h"
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
EvtErrorMessage::EvtErrorMessage( std::wstring& message )
{
	m_Message = message;
}
//--------------------------------------------------------------------------------
EvtErrorMessage::EvtErrorMessage( const wchar_t* pMessage )
{
	m_Message = std::wstring( pMessage );
}
//--------------------------------------------------------------------------------
EvtErrorMessage::~EvtErrorMessage( )
{
}
//--------------------------------------------------------------------------------
std::wstring EvtErrorMessage::GetEventName( )
{
	return( std::wstring( L"info_message" ) );
}
//--------------------------------------------------------------------------------
eEVENT EvtErrorMessage::GetEventType( )
{
	return( ERROR_MESSAGE );
}
//--------------------------------------------------------------------------------
std::wstring& EvtErrorMessage::GetErrorMessage( )
{
	return( m_Message );
}
//--------------------------------------------------------------------------------