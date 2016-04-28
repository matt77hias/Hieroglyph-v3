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
// ViewGBuffer
//
//--------------------------------------------------------------------------------
#ifndef ViewGBuffer_h
#define ViewGBuffer_h
//--------------------------------------------------------------------------------
#include "SceneRenderTask.h"
//--------------------------------------------------------------------------------
namespace Glyph3
{
	class ViewGBuffer : public SceneRenderTask
	{
	public:
		ViewGBuffer( RendererDX11& Renderer );
		virtual ~ViewGBuffer();

		virtual void Update( float fTime );
		virtual void QueuePreTasks( RendererDX11* pRenderer );
		virtual void ExecuteTask( PipelineManagerDX11* pPipelineManager, IParameterManager* pParamManager );
		virtual void Resize( UINT width, UINT height );

		virtual void SetRenderParams( IParameterManager* pParamManager );
		virtual void SetUsageParams( IParameterManager* pParamManager );

        void SetTargets( std::vector<ResourcePtr>& GBufferTargets, ResourcePtr DepthTarget,
                          int Viewport );

		virtual std::wstring GetName();

	protected:

        RendererDX11&				m_Renderer;
		std::vector<ResourcePtr>	m_GBufferTargets;
		ResourcePtr					m_DepthTarget;
	};
}
//--------------------------------------------------------------------------------
#endif // ViewGBuffer_h