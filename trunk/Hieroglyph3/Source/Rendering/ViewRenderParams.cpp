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
#include "ViewRenderParams.h"
#include "Log.h"
//--------------------------------------------------------------------------------
using namespace Glyph3;
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
		vParameters[i].MakeZero();
	}
}
//--------------------------------------------------------------------------------
VIEWTYPE ViewRenderParams::ViewNameToIndex( std::string& name )
{
	if ( name == "perspective" )
		return( VT_PERSPECTIVE );
	else if ( name == "gui_skin" )
		return( VT_GUI_SKIN );
	else if ( name == "paraboloid_env_map" )
		return( VT_DUAL_PARABOLOID_ENVMAP );
	else if ( name == "screen_space_ambient_occlusion" )
		return( VT_SSAO );
	else if ( name == "linear_depth" )
		return( VT_LINEAR_DEPTH );

	// VT_INVALID should be index 0, which shouldn't be used for anything...
	return( VT_INVALID );

}
//--------------------------------------------------------------------------------