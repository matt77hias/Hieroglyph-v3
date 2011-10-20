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
// RenderEffectDX11
//
// A render effect is a complete pipeline configuration, excluding only the input
// geometry and the output buffers for rendering commands.  This allows for an
// application to specify a single render effect to completely configure the 
// renderer based on it's current state.
//--------------------------------------------------------------------------------
#ifndef RenderEffectDX11_h
#define RenderEffectDX11_h
//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
namespace Glyph3
{
	class PipelineManagerDX11;
	class IParameterManager;

	class RenderEffectDX11
	{
	public:
		RenderEffectDX11();
		virtual ~RenderEffectDX11();

		void ConfigurePipeline( PipelineManagerDX11* pPipeline, IParameterManager* pParamManager );

	public:
		int		m_iVertexShader;
		int		m_iHullShader;
		int		m_iDomainShader;
		int		m_iGeometryShader;
		int		m_iPixelShader;
		int		m_iComputeShader;

		int		m_iBlendState;
		int		m_iDepthStencilState;
		int		m_iRasterizerState;
        UINT    m_uStencilRef; 
	};
};
//--------------------------------------------------------------------------------
#endif // RenderEffectDX11_h
//--------------------------------------------------------------------------------

