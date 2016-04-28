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
// PickRecord
//
//--------------------------------------------------------------------------------
#ifndef PickRecord_h
#define PickRecord_h
//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
namespace Glyph3
{
	class Entity3D;

	class PickRecord
	{
	public:
		PickRecord( );
		virtual ~PickRecord( );
	
	public:
		Entity3D*	pEntity;
		float		fDistance;
	};
};
//--------------------------------------------------------------------------------
#endif // PickRecord_h
