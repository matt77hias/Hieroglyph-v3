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
// ViewDepthNormal
//
// ViewDepthNormal is intended to generate a depth/normal buffer of a give scene.
// This allows any other views to utilize a commonly generated buffer instead of
// having to hard code the generation into multiple places.
//--------------------------------------------------------------------------------
#ifndef ViewDepthNormal_h
#define ViewDepthNormal_h
//--------------------------------------------------------------------------------
#include "ViewPerspective.h"
#include "ShaderResourceParameterDX11.h"
//--------------------------------------------------------------------------------
namespace Glyph3
{
	class Entity3D;

	class ViewDepthNormal : public ViewPerspective
	{
	public:
		ViewDepthNormal( RendererDX11& Renderer, ResourcePtr RenderTarget, ResourcePtr DepthTarget );
		virtual ~ViewDepthNormal();

		virtual void SetUsageParams( IParameterManager* pParamManager );

	protected:
		int ResolutionX;
		int ResolutionY;

		ResourcePtr DepthNormalBuffer;
		ShaderResourceParameterDX11* m_pDepthNormalBuffer;
	};
};
//--------------------------------------------------------------------------------
#endif // ViewDepthNormal_h
