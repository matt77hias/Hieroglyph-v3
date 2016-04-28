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
#include "ShaderDX11.h"
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

		void SetVertexShader( int index );
		void SetHullShader( int index );
		void SetDomainShader( int index );
		void SetGeometryShader( int index );
		void SetPixelShader( int index );
		void SetComputeShader( int index );

		int GetVertexShader( );
		int GetHullShader( );
		int GetDomainShader( );
		int GetGeometryShader( );
		int GetPixelShader( );
		int GetComputeShader( );

		void ConfigurePipeline( PipelineManagerDX11* pPipeline, IParameterManager* pParamManager );

		int		m_iBlendState;
		int		m_iDepthStencilState;
		int		m_iRasterizerState;
        UINT    m_uStencilRef; 

	protected:

		void UpdateConstantBufferList( );

		int					m_aiIndices[6];
		ShaderDX11*			m_apShaders[6];

		std::vector< RenderParameterDX11* > m_uniqueConstBuffers;
	};
};
//--------------------------------------------------------------------------------
#endif // RenderEffectDX11_h
//--------------------------------------------------------------------------------

