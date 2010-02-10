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
// Actor
//
//--------------------------------------------------------------------------------
#ifndef Actor_h
#define Actor_h
//--------------------------------------------------------------------------------
#include "Node3D.h"
//--------------------------------------------------------------------------------
namespace Glyph3
{
	class Actor
	{
	public:
		Actor();
		virtual ~Actor();

		Node3D* GetNode();
		Entity3D* GetBody();

	protected:
		Node3D*					m_pRoot;
		Entity3D*				m_pBody;
	};
};
//--------------------------------------------------------------------------------
#endif // Actor_h
