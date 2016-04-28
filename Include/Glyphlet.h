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
// Glyphlet
//
//--------------------------------------------------------------------------------
#ifndef Glyphlet_h
#define Glyphlet_h
//--------------------------------------------------------------------------------
#include "Log.h"
#include "Timer.h"
#include "EventManager.h"
#include "Scene.h"
//--------------------------------------------------------------------------------
namespace Glyph3
{
	class Glyphlet : public IEventListener
	{

	public:

		// The general glyphlet interface is defined by these four functions. 
		// Their individual implementations can perform whatever functionality
		// is needed for a given situation, thus no data members are assumed in
		// this base class.

		virtual void Initialize() = 0;
		virtual void Update( float dt ) = 0;
		virtual void Shutdown() = 0;
		virtual bool HandleEvent( EventPtr pEvent ) = 0;
	};
};
//--------------------------------------------------------------------------------
#endif // Glyphlet_h