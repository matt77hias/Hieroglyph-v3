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
// ViewPerspectiveHighlight
//
//--------------------------------------------------------------------------------
#ifndef ViewPerspectiveHighlight_h
#define ViewPerspectiveHighlight_h
//--------------------------------------------------------------------------------
#include "SceneRenderTask.h"
#include "FullscreenActor.h"
//--------------------------------------------------------------------------------
namespace Glyph3
{
	class Entity3D;

	class ViewPerspectiveHighlight : public SceneRenderTask
	{
	public:
		ViewPerspectiveHighlight( RendererDX11& Renderer, ResourcePtr RenderTarget, ResourcePtr DepthTarget = 0 );
		virtual ~ViewPerspectiveHighlight();

		virtual void Update( float fTime );
		virtual void QueuePreTasks( RendererDX11* pRenderer );
		virtual void ExecuteTask( PipelineManagerDX11* pPipelineManager, IParameterManager* pParamManager );
		virtual void Resize( UINT width, UINT height );

		void SetRenderTargets( ResourcePtr RenderTarget, ResourcePtr DepthTarget = nullptr );

		virtual std::wstring GetName();

		ResourcePtr		m_SilhouetteTarget;
		FullscreenActor m_SilhouetteActor;

	protected:

		ResourcePtr		m_RenderTarget;
		ResourcePtr		m_DepthTarget;
	};
};
//--------------------------------------------------------------------------------
#endif // ViewPerspectiveHighlight_h
