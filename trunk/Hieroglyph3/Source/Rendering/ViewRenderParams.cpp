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
#include "PCH.h"
#include "ViewRenderParams.h"
#include "Log.h"
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
static const std::string ViewNames[VT_NUM_VIEW_TYPES] = 
{
    "invalid",
    "simulation",
    "linear_depth",
    "screen_space_ambient_occlusion",
    "paraboloid_env_map",
    "perspective",
    "planar_reflection",
    "gui_skin",
    "g_buffer",
    "lights",
    "final_pass",
	"ambient_occlusion"
};
//--------------------------------------------------------------------------------
ViewRenderParams::ViewRenderParams()
{
	iViewType = VT_INVALID;

	for ( int i = 0; i < 8; i++ )
	{
		iRenderTargets[i] = -1;
		iDepthTargets[i] = -1;
		iTextures[i] = -1;
		iEffects[i] = -1;
	}
}
//--------------------------------------------------------------------------------
VIEWTYPE ViewRenderParams::ViewNameToIndex( std::string& name )
{
    for ( int i = 0; i < VT_NUM_VIEW_TYPES; ++i )
        if ( name == ViewNames[i] )
            return static_cast<VIEWTYPE>( i );
	
	// VT_INVALID should be index 0, which shouldn't be used for anything...
	return( VT_INVALID );

}
//--------------------------------------------------------------------------------
std::string ViewRenderParams::ViewIndexToName( VIEWTYPE index )
{
    return ViewNames[index];
}
//--------------------------------------------------------------------------------