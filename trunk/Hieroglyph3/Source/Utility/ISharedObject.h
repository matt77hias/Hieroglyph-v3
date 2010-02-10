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
// ISharedObject
//
//--------------------------------------------------------------------------------
#ifndef ISharedObject_h
#define ISharedObject_h
//--------------------------------------------------------------------------------
#include <string>
//--------------------------------------------------------------------------------
namespace Glyph3
{
	class ISharedObject
	{
	public:

		ISharedObject();
		virtual ~ISharedObject();

		void AddReference();
		void Release();

	protected:
		int	m_iRefCount;
	};
};
//--------------------------------------------------------------------------------
#endif // ISharedObject_h
//--------------------------------------------------------------------------------