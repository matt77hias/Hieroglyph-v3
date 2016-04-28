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
// RiftManager
//
// This is a simple RAII style class for initializing and uninitializing the 
// Oculus SDK.
//--------------------------------------------------------------------------------
#ifndef RiftManager_h
#define RiftManager_h
//--------------------------------------------------------------------------------
#include <memory>
//--------------------------------------------------------------------------------
namespace Glyph3
{
	class RiftManager
	{
	public:
		RiftManager();
		~RiftManager();
	};

	typedef std::shared_ptr<RiftManager> RiftManagerPtr;
}
//--------------------------------------------------------------------------------
#endif // RiftManager_h
//--------------------------------------------------------------------------------
