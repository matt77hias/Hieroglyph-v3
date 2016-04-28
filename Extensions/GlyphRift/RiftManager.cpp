//--------------------------------------------------------------------------------
// This file is a portion of the Hieroglyph 3 Rendering Engine.  It is distributed
// under the MIT License, available in the root of this distribution and 
// at the following URL:
//
// http://www.opensource.org/licenses/mit-license.php
//
// Copyright (c) Jason Zink 
//--------------------------------------------------------------------------------
#include "GlyphRift/RiftManager.h"
#include "OVR_CAPI.h"
#include "Log.h"
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
RiftManager::RiftManager()
{
	ovrResult result = ovr_Initialize( nullptr );

	if ( result != ovrSuccess ) {
		Log::Get().Write( L"ERROR: libOVR failed to initialize!" );
	}
}
//--------------------------------------------------------------------------------
RiftManager::~RiftManager()
{
	ovr_Shutdown();
}
//--------------------------------------------------------------------------------
