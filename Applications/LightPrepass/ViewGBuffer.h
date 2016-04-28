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
#include "RenderEffectDX11.h"
#include "GeometryDX11.h"
#include "ShaderResourceParameterDX11.h"
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

        void SetTargets( ResourcePtr GBufferTargets, ResourcePtr DepthTarget,
                          int Viewport );

		virtual std::wstring GetName();

	protected:

		ResourcePtr 	        m_GBufferTarget;
		ResourcePtr				m_DepthTarget;

        int                     m_iMaskDSState;
        int                     m_iMaskRSState;
        RenderEffectDX11		m_MaskEffect;
        GeometryPtr				m_QuadGeometry;

		ShaderResourceParameterDX11*	m_pGBufferTexture;
	};
}
//--------------------------------------------------------------------------------
#endif // ViewGBuffer_h