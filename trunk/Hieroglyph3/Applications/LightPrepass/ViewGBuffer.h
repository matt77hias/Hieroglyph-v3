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
#include "RenderEffectDX11.h"
#include "GeometryDX11.h"
#include "ShaderResourceParameterDX11.h"
//--------------------------------------------------------------------------------
namespace Glyph3
{
	class ViewGBuffer : public IRenderView
	{
	public:
		ViewGBuffer( RendererDX11& Renderer );

		virtual void Update( float fTime );
		virtual void PreDraw( RendererDX11* pRenderer );
		virtual void Draw( PipelineManagerDX11* pPipelineManager, IParameterManager* pParamManager );

		virtual void SetRenderParams( IParameterManager* pParamManager );
		virtual void SetUsageParams( IParameterManager* pParamManager );

        void SetTargets( ResourcePtr GBufferTargets, ResourcePtr DepthTarget,
                          int Viewport );

		virtual ~ViewGBuffer();

	protected:

		int					    m_iViewport;

        RendererDX11&           m_Renderer;
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