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
// IScriptInterface
//
//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
#ifndef IScriptInterface_h
#define IScriptInterface_h
//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
namespace Glyph3
{
	class IScriptInterface
	{
	public:
		virtual void InitializeInterface( ) = 0;
	};
};
//--------------------------------------------------------------------------------
#endif // IScriptInterface_h
