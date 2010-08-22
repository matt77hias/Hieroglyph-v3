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
// ViewOcclusion
//
//--------------------------------------------------------------------------------
#ifndef ViewOcclusion_h
#define ViewOcclusion_h
//--------------------------------------------------------------------------------
#include "IRenderView.h"
#include "Actor.h"
//--------------------------------------------------------------------------------
namespace Glyph3
{
	class Entity3D;

	class ViewOcclusion : public IRenderView
	{
	public:
		ViewOcclusion( RendererDX11& Renderer, ResourcePtr OcclusionTarget, ResourcePtr BlurTarget, ResourcePtr DepthNormalTarget );

		virtual void Update( float fTime );
		virtual void PreDraw( RendererDX11* pRenderer );
		virtual void Draw( PipelineManagerDX11* pPipelineManager, ParameterManagerDX11* pParamManager );

		virtual void SetRenderParams( ParameterManagerDX11* pParamManager );
		virtual void SetUsageParams( ParameterManagerDX11* pParamManager );


		virtual ~ViewOcclusion();

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
#endif // ViewOcclusion_h
