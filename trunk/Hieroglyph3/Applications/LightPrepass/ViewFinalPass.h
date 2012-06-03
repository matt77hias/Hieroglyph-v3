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
// ViewFinalPass
//
//--------------------------------------------------------------------------------
#ifndef ViewFinalPass_h
#define ViewFinalPass_h
//--------------------------------------------------------------------------------
#include "IRenderView.h"
#include "ShaderResourceParameterDX11.h"
//--------------------------------------------------------------------------------
namespace Glyph3
{
    class ViewFinalPass : public IRenderView
    {
    public:
        ViewFinalPass( RendererDX11& Renderer );

        virtual void Update( float fTime );
        virtual void PreDraw( RendererDX11* pRenderer );
        virtual void Draw( PipelineManagerDX11* pPipelineManager, IParameterManager* pParamManager );
		virtual void Resize( UINT width, UINT height );

        virtual void SetRenderParams( IParameterManager* pParamManager );
        virtual void SetUsageParams( IParameterManager* pParamManager );

        void SetTargets( ResourcePtr LightTarget, ResourcePtr RenderTarget, ResourcePtr DepthTarget, int Viewport );

        virtual ~ViewFinalPass();

    protected:

        RendererDX11&           m_Renderer;
        ResourcePtr 	        m_LightTarget;
        ResourcePtr 	        m_RenderTarget;
        ResourcePtr				m_DepthTarget;

		ShaderResourceParameterDX11*	m_pLightTexture;
    };
}
//--------------------------------------------------------------------------------
#endif // ViewFinalPass_h