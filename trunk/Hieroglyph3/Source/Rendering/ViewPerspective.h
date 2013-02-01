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
		ViewPerspective( RendererDX11& Renderer, ResourcePtr RenderTarget, ResourcePtr DepthTarget = 0 );

		virtual void Update( float fTime );
		virtual void PreDraw( RendererDX11* pRenderer );
		virtual void Draw( PipelineManagerDX11* pPipelineManager, IParameterManager* pParamManager );
		virtual void Resize( UINT width, UINT height );

		virtual void SetRenderParams( IParameterManager* pParamManager );
		virtual void SetUsageParams( IParameterManager* pParamManager );


		virtual ~ViewPerspective();

	protected:

		ResourcePtr		m_RenderTarget;
		ResourcePtr		m_DepthTarget;
	};
};
//--------------------------------------------------------------------------------
#endif // ViewPerspective_h
