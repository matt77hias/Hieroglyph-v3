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
// OutputMergerStageStateDX11
//
//--------------------------------------------------------------------------------
#ifndef OutputMergerStageStateDX11_h
#define OutputMergerStageStateDX11_h
//--------------------------------------------------------------------------------
#include "PCH.h"
//--------------------------------------------------------------------------------
namespace Glyph3
{
	class OutputMergerStageDX11;

	class OutputMergerStageStateDX11
	{
	public:
		OutputMergerStageStateDX11();
		virtual ~OutputMergerStageStateDX11();

		void SetFeautureLevel( D3D_FEATURE_LEVEL level );

		void SetBlendState( int state );
		void SetDepthStencilState( int state, unsigned int stencilRef = 0 );
		void SetRenderTarget( unsigned int slot, int rtv );
		void SetDepthStencilTarget( int dsv );
		void SetUnorderedAccessView( unsigned int slot, int uav, unsigned int initCount = -1 );
		
		int GetBlendState() const;
		int GetDepthStencilState() const;
		unsigned int GetStencilReference() const;
		int GetRenderTarget( unsigned int slot ) const;
		int GetDepthStencilTarget( ) const;
		int GetUnorderedAccessView( unsigned int slot ) const;
		int GetInitialCount( unsigned int slot ) const;

		int GetRenderTargetCount() const;

		int CompareBlendState( OutputMergerStageStateDX11& desired );
		int CompareDepthStencilState( OutputMergerStageStateDX11& desired );
		int CompareRenderTargets( OutputMergerStageStateDX11& desired );
		int CompareUnorderedAccessViews( OutputMergerStageStateDX11& desired );

		void ClearState( );

	protected:

		D3D_FEATURE_LEVEL				m_FeatureLevel;

		int								BlendState;
		int								DepthStencilState;
		unsigned int					StencilRef;
		int								RenderTargetViews[D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT];
		int								DepthTargetViews;
		int								UnorderedAccessViews[D3D11_PS_CS_UAV_REGISTER_COUNT];
		unsigned int					UAVInitialCounts[D3D11_PS_CS_UAV_REGISTER_COUNT];
		
		friend OutputMergerStageDX11;
	};
};
//--------------------------------------------------------------------------------
#endif // OutputMergerStageStateDX11_h
//--------------------------------------------------------------------------------

