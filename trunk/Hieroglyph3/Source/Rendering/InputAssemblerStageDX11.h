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
// InputAssemblerStageDX11
//
//--------------------------------------------------------------------------------
#ifndef InputAssemblerStageDX11_h
#define InputAssemblerStageDX11_h
//--------------------------------------------------------------------------------
#include "RendererDX11.h"
//--------------------------------------------------------------------------------
namespace Glyph3
{
	class InputAssemblerStageDX11
	{
	public:
		InputAssemblerStageDX11();
		virtual ~InputAssemblerStageDX11();

		void SetFeautureLevel( D3D_FEATURE_LEVEL level );

		// Each of these set methods will buffer their input views for binding
		// at a later time when the BindResources method is called.  Until it is,
		// these views are cached for later use.
		void SetRenderTargetView( int index, ID3D11RenderTargetView* pBuffer );
		void SetDepthStencilView( ID3D11DepthStencilView* pState );
		void SetUnorderedAccessView( int index, ID3D11UnorderedAccessView* pUAV, unsigned int initial = -1 );

		// Binding resources actually binds the currently 'set' views and makes 
		// them 'bound' views to the API.
		void BindResources( ID3D11DeviceContext* pContext );
		
		// Clearing resources wipes out the currently 'set' views from the buffer.
		void ClearResources( ID3D11DeviceContext* pContext );
		void UnbindResources( ID3D11DeviceContext* pContext );

		// The number of views 'set' indicates how many views will be bound after
		// the next call to bind resources (i.e. the number of views planned to be
		// bound).
		unsigned int GetViewsSetCount();

		// The number of views bound indicates how many views are bound to the pipeline
		// in the API currently according to the tracked state of the context.
		unsigned int GetViewsBoundCount();

	protected:

		D3D_FEATURE_LEVEL				m_FeatureLevel;

		ID3D11RenderTargetView*			RenderTargetViews[D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT];
		ID3D11RenderTargetView*			APIRenderTargetViews[D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT];
		ID3D11DepthStencilView*			DepthTargetViews;
        ID3D11DepthStencilView*			APIDepthTargetViews;
		ID3D11UnorderedAccessView*		UnorderedAccessViews[D3D11_PS_CS_UAV_REGISTER_COUNT];
		unsigned int					UAVInitialCounts[D3D11_PS_CS_UAV_REGISTER_COUNT];

		friend PipelineManagerDX11;
	};
};
//--------------------------------------------------------------------------------
#endif // InputAssemblerStageDX11_h
//--------------------------------------------------------------------------------

