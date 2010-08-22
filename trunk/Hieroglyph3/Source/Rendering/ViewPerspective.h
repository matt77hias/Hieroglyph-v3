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
// ViewPerspective
//
// ViewPerspective is intended to be the primary render view that most of the
// everyday rendering will use.  This render view will use the scene graph to
// select the currently visible objects and render them.
//--------------------------------------------------------------------------------
#ifndef ViewPerspective_h
#define ViewPerspective_h
//--------------------------------------------------------------------------------
#include "IRenderView.h"
//--------------------------------------------------------------------------------
namespace Glyph3
{
	class Entity3D;

	class ViewPerspective : public IRenderView
	{
	public:
		ViewPerspective( RendererDX11& Renderer, ResourcePtr RenderTarget, ResourcePtr DepthTarget );

		virtual void Update( float fTime );
		virtual void PreDraw( RendererDX11* pRenderer );
		virtual void Draw( PipelineManagerDX11* pPipelineManager, ParameterManagerDX11* pParamManager );

		virtual void SetBackColor( Vector4f color );
		virtual void SetViewPort( DWORD x, DWORD y, DWORD w, DWORD h, float MinZ, float MaxZ );

		virtual void SetRenderParams( ParameterManagerDX11* pParamManager );
		virtual void SetUsageParams( ParameterManagerDX11* pParamManager );


		virtual ~ViewPerspective();

	protected:
		Vector4f		m_vColor;
		int				m_iViewport;

		ResourcePtr		m_RenderTarget;
		ResourcePtr		m_DepthTarget;
	};
};
//--------------------------------------------------------------------------------
#endif // ViewPerspective_h
