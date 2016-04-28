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
// ScriptedGlyphlet
//
// The scripting glyphlet is intended to move the scene generation and 
// manipulation responsibilities to the script world.  This is accomplished by 
// providing each of these instances with their own script environment, which 
// means they will be executing in isolation from other script driven 
// functionality.
//
// The script must define some functions to be called as the 'Initialization', 
// 'Update', and 'Shutdown' methods.  These functions will have at their disposal
// a number of services built into their scripting environment, such as access to
// logging functionality.
//--------------------------------------------------------------------------------
#ifndef ScriptedGlyphlet_h
#define ScriptedGlyphlet_h
//--------------------------------------------------------------------------------
#include "SingleWindowGlyphlet.h"
#include "ScriptManager.h"
//--------------------------------------------------------------------------------
namespace Glyph3
{
	class ScriptedGlyphlet : public SingleWindowGlyphlet
	{
	public:
		ScriptedGlyphlet( );
		virtual ~ScriptedGlyphlet();

		virtual void Initialize();
		virtual void Update( float dt );
		virtual void Shutdown();

		virtual bool HandleEvent( EventPtr pEvent );
		virtual std::wstring GetName( );

	protected:
		ScriptManager*		m_pScriptManager;
	};
};
//--------------------------------------------------------------------------------
#endif // ScriptedGlyphlet_h
