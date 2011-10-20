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
#ifndef OutputMergerStageDX11_h
#define OutputMergerStageDX11_h
//--------------------------------------------------------------------------------
#include "PCH.h"
#include "ResourceProxyDX11.h"
//--------------------------------------------------------------------------------
namespace Glyph3
{
	class PipelineManagerDX11;

	class OutputMergerStageDX11
	{
	public:
		OutputMergerStageDX11();
		virtual ~OutputMergerStageDX11();

		void SetFeautureLevel( D3D_FEATURE_LEVEL level );

		// Each of these set methods will buffer their input views for binding
		// at a later time when the BindResources method is called.  Until it is,
		// these views are cached for later use.
		void BindRenderTarget( int index, ResourcePtr Target );
		void BindDepthTarget( ResourcePtr DepthTarget );
		void SetUnorderedAccessView( int index, ID3D11UnorderedAccessView* pUAV, unsigned int initial = -1 );

		// Binding resources actually binds the currently 'set' views and makes 
		// them 'bound' views to the API.
		void BindResources( ID3D11DeviceContext* pContext );
		
		// Clearing resources wipes out the currently 'set' views from the buffer.
		void ClearResources();
		void UnbindResources( ID3D11DeviceContext* pContext );

		// This method will set the current cached API state to the default value.
		// This is commonly used when the context is reset for some reason.
		void SetToDefaultState();

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
#endif // OutputMergerStageDX11_h
//--------------------------------------------------------------------------------

