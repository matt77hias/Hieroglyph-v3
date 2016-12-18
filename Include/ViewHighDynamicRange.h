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
// ViewHighDynamicRange
//
//--------------------------------------------------------------------------------
#ifndef ViewHighDynamicRange_h
#define ViewHighDynamicRange_h
//--------------------------------------------------------------------------------
#include "SceneRenderTask.h"
#include "ImageProcessor.h"
#include "FullscreenTexturedActor.h"
//--------------------------------------------------------------------------------
namespace Glyph3
{
	class Entity3D;

	class ViewHighDynamicRange : public SceneRenderTask
	{
	public:
		ViewHighDynamicRange( RendererDX11& Renderer, ResourcePtr RenderTarget, float threshold = 3.0f, ResourcePtr DepthTarget = 0 );
		virtual ~ViewHighDynamicRange();

		virtual void Update( float fTime );
		virtual void QueuePreTasks( RendererDX11* pRenderer );
		virtual void ExecuteTask( PipelineManagerDX11* pPipelineManager, IParameterManager* pParamManager );
		virtual void Resize( UINT width, UINT height );

		void SetRenderTargets( ResourcePtr RenderTarget, ResourcePtr DepthTarget = nullptr );

		virtual std::wstring GetName();

	protected:

		ResourcePtr		m_RenderTarget;
		ResourcePtr		m_DepthTarget;

		ResourcePtr		m_FloatTarget;
		ResourcePtr		m_BlurTargetX;
		ResourcePtr		m_BlurTargetY;

		ImageProcessor	processor;

		unsigned int	m_width;
		unsigned int	m_height;

		float			m_threshold;

		FullscreenTexturedActor fullscreen;
	};
};
//--------------------------------------------------------------------------------
#endif // ViewHighDynamicRange_h
//--------------------------------------------------------------------------------