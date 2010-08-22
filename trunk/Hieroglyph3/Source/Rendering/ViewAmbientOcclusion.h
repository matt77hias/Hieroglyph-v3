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
#include "ViewPerspective.h"
#include "ViewOcclusion.h"
#include "ViewDepthNormal.h"
#include "Actor.h"
//--------------------------------------------------------------------------------
namespace Glyph3
{
	class Entity3D;

	class ViewAmbientOcclusion : public ViewPerspective
	{
	public:
		ViewAmbientOcclusion( RendererDX11& Renderer, ResourcePtr RenderTarget, ResourcePtr DepthTarget );

		virtual void Update( float fTime );
		virtual void PreDraw( RendererDX11* pRenderer );
		virtual void Draw( PipelineManagerDX11* pPipelineManager, ParameterManagerDX11* pParamManager );

		virtual void SetEntity( Entity3D* pEntity );
		virtual void SetRoot( Node3D* pRoot );

		virtual void SetBackColor( Vector4f color );

		virtual void SetRenderParams( ParameterManagerDX11* pParamManager );
		virtual void SetUsageParams( ParameterManagerDX11* pParamManager );

		virtual void SetViewMatrix( Matrix4f& matrix );
		virtual void SetProjMatrix( Matrix4f& matrix );

		virtual ~ViewAmbientOcclusion();

	protected:

		int ResolutionX;
		int ResolutionY;

		ResourcePtr DepthNormalBuffer;
		ResourcePtr OcclusionBuffer;
		ResourcePtr BilateralBuffer;

		//ViewPerspective*	pPerspectiveView;
		ViewOcclusion*		pOcclusionView;
		ViewDepthNormal*	pDepthNormalView;
//		RenderEffectDX11*	pOcclusionEffect;
//		RenderEffectDX11*	pBilateralXEffect;
//		RenderEffectDX11*	pBilateralYEffect;

		Actor*				pVisActor;
	};
};
//--------------------------------------------------------------------------------
#endif // ViewAmbientOcclusion_h
