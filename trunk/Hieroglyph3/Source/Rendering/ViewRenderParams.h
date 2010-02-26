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
// ViewRenderParams
//
// The view rendering parameters structure defines all of the variable
// parameters that could potentially change from view to view.  This 
// structure represents all of the ways that a view can communicate
// with the renderer.
//
// The iType variable identifies what kind of render view is being used.
// The iEffect variable provides a default rendering effect to be used
// unless an object has defined a specific effect for a given view type.
//
// Additional storage is provided for up to eight render and depth targets,
// as well as eight four component vectors.  The view is also responsible
// for setting the view and projection matrices for the renderer to use.
//--------------------------------------------------------------------------------
#ifndef ViewRenderParams_h
#define ViewRenderParams_h
//--------------------------------------------------------------------------------
#include "Vector4f.h"
#include "Matrix4f.h"
#include <string>
//--------------------------------------------------------------------------------
namespace Glyph3
{
	// The view type is used to allow a view to identify what type of
	// view it is.  This identifier is also used by objects to specify
	// special rendering effects to use in that particular render view.

	enum VIEWTYPE
	{
		VT_INVALID,
		VT_SIMULATION,
		VT_LINEAR_DEPTH_NORMALS,
		VT_SSAO,
		//VT_STANDARD_SHADOWMAP,
		//VT_CONVOLUTION_SHADOWMAP,
		//VT_EXPONENTIAL_SHADOWMAP,
		VT_DUAL_PARABOLOID_ENVMAP,
		VT_PERSPECTIVE,
//		VT_COPY,
//		VT_NORMALS,
		VT_GUI_SKIN,
		VT_NUM_VIEW_TYPES
	};

	class ViewRenderParams
	{
	public:
		ViewRenderParams();
		static VIEWTYPE ViewNameToIndex( std::string& name );

	public:
		VIEWTYPE		iViewType;
		int				iEffects[8];
		int				iRenderTargets[10];
		int				iDepthTargets[8];
		int				iTextures[8];
		Vector4f		vParameters[8];
	};
};
//--------------------------------------------------------------------------------
#endif // ViewRenderParams_h
