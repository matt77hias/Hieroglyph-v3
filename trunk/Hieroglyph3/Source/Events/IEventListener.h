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
// IEventListener
//
//--------------------------------------------------------------------------------
#ifndef IEventListener_h
#define IEventListener_h
//--------------------------------------------------------------------------------
#include "IEvent.h"
//--------------------------------------------------------------------------------
namespace Glyph3
{
	class IEventListener
	{
	public:
		IEventListener( ) {}; 
		virtual ~IEventListener( ) {};

		virtual std::wstring GetName( ) = 0;
		virtual bool HandleEvent( IEvent* pEvent ) = 0;
	};
};
//--------------------------------------------------------------------------------
#endif // IEventListener_h
