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
// ViewGBuffer
//
//--------------------------------------------------------------------------------
#ifndef ViewGBuffer_h
#define ViewGBuffer_h
//--------------------------------------------------------------------------------
#include "IRenderView.h"
//--------------------------------------------------------------------------------
namespace Glyph3
{
	class ViewGBuffer : public IRenderView
	{
    public:
        // Constants
        static const UINT StencilRef = 0;

	public:		
		ViewGBuffer( RendererDX11& Renderer, ResourcePtr DepthTarget );

		virtual void Update( float fTime );
		virtual void PreDraw( RendererDX11* pRenderer );
		virtual void Draw( PipelineManagerDX11* pPipelineManager, ParameterManagerDX11* pParamManager );

		virtual void SetRenderParams( ParameterManagerDX11* pParamManager );
		virtual void SetUsageParams( ParameterManagerDX11* pParamManager );

        void SetGBufferTargets( TArray<ResourcePtr>& GBufferTargets );

		virtual ~ViewGBuffer();

	protected:

		int					m_iViewport;
		int					m_iDepthStencilState;

        RendererDX11&           m_Renderer;
		TArray<ResourcePtr> 	m_GBufferTargets;
		ResourcePtr				m_DepthTarget;
        ResourcePtr				m_DepthTargetCopy;
	};
}
//--------------------------------------------------------------------------------
#endif // ViewGBuffer_h