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
// ViewAmbientOcclusion
//
//--------------------------------------------------------------------------------
#ifndef ViewAmbientOcclusion_h
#define ViewAmbientOcclusion_h
//--------------------------------------------------------------------------------
#include "IRenderView.h"
//--------------------------------------------------------------------------------
namespace Glyph3
{
	class Entity3D;

	class ViewAmbientOcclusion : public IRenderView
	{
	public:
		ViewAmbientOcclusion( RendererDX11& Renderer, int SizeX, int SizeY );

		virtual void Update( float fTime );
		virtual void Draw( RendererDX11& Renderer );

		virtual void SetRenderParams( RendererDX11& Renderer );
		virtual void SetUsageParams( RendererDX11& Renderer );


		virtual ~ViewAmbientOcclusion();

	protected:
		int ResolutionX;
		int ResolutionY;

		ResourcePtr DepthNormalBuffer;
		ResourcePtr OcclusionBuffer;
		ResourcePtr BilateralBuffer;

		RenderEffectDX11*	pOcclusionEffect;
		RenderEffectDX11*	pBilateralXEffect;
		RenderEffectDX11*	pBilateralYEffect;
	};
};
//--------------------------------------------------------------------------------
#endif // ViewAmbientOcclusion_h
