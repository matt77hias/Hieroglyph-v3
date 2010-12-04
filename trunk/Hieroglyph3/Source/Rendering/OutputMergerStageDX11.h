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
// OutputMergerStageDX11
//
//--------------------------------------------------------------------------------
#include "RendererDX11.h"
//--------------------------------------------------------------------------------
#ifndef OutputMergerStageDX11_h
#define OutputMergerStageDX11_h
//--------------------------------------------------------------------------------
namespace Glyph3
{
	class OutputMergerStageDX11
	{
	public:
		OutputMergerStageDX11();
		virtual ~OutputMergerStageDX11();

		void SetFeautureLevel( D3D_FEATURE_LEVEL level );

		void SetRenderTargetView( int index, ID3D11RenderTargetView* pBuffer );
		void SetDepthStencilView( ID3D11DepthStencilView* pState );
		void SetUnorderedAccessView( int index, ID3D11UnorderedAccessView* pUAV, unsigned int initial = -1 );

		void BindResources( ID3D11DeviceContext* pContext );
		void ClearResources( ID3D11DeviceContext* pContext );
		void UnbindResources( ID3D11DeviceContext* pContext );

	protected:

		D3D_FEATURE_LEVEL				m_FeatureLevel;

		ID3D11RenderTargetView*			RenderTargetViews[8];
		ID3D11RenderTargetView*			APIRenderTargetViews[8];
		ID3D11DepthStencilView*			DepthTargetViews;
        ID3D11DepthStencilView*			APIDepthTargetViews;
		ID3D11UnorderedAccessView*		UnorderedAccessViews[D3D11_PS_CS_UAV_REGISTER_COUNT];
		unsigned int					UAVInitialCounts[D3D11_PS_CS_UAV_REGISTER_COUNT];

		friend RendererDX11;
	};
};
//--------------------------------------------------------------------------------
#endif // OutputMergerStageDX11_h
//--------------------------------------------------------------------------------

