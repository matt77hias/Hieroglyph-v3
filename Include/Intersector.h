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
// Intersector
//
//--------------------------------------------------------------------------------
#ifndef Intersector_h
#define Intersector_h
//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
namespace Glyph3
{
	class Intersector
	{
	public:
		virtual ~Intersector( );
	
		virtual bool Test();
		virtual bool Find();

	protected:
		Intersector( );
	};
};
//--------------------------------------------------------------------------------
#endif // Intersector_h
