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
#include "OutputMergerStageStateDX11.h"
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

		void ClearDesiredState( );
		void ClearCurrentState( );
		void ApplyDesiredRenderTargetStates( ID3D11DeviceContext* pContext );
		void ApplyDesiredBlendAndDepthStencilStates( ID3D11DeviceContext* pContext );
		void ApplyDesiredState( ID3D11DeviceContext* pContext );

		const OutputMergerStageStateDX11& GetCurrentState() const;


		// The desired state is a public member that will allow the user of this
		// class to configure the state as desired before applying the state.

		OutputMergerStageStateDX11		DesiredState;







		//// Each of these set methods will buffer their input views for binding
		//// at a later time when the BindResources method is called.  Until it is,
		//// these views are cached for later use.
		//void BindRenderTarget( int index, ResourcePtr Target );
		//void BindDepthTarget( ResourcePtr DepthTarget );
		//void SetUnorderedAccessView( int index, ID3D11UnorderedAccessView* pUAV, unsigned int initial = -1 );

		//// Binding resources actually binds the currently 'set' views and makes 
		//// them 'bound' views to the API.
		//void BindResources( ID3D11DeviceContext* pContext );
		//
		//// Clearing resources wipes out the currently 'set' views from the buffer.
		//void ClearResources();
		//void UnbindResources( ID3D11DeviceContext* pContext );

		//// This method will set the current cached API state to the default value.
		//// This is commonly used when the context is reset for some reason.
		//void SetToDefaultState();

		//// The number of views 'set' indicates how many views will be bound after
		//// the next call to bind resources (i.e. the number of views planned to be
		//// bound).
		//unsigned int GetViewsSetCount();

		//// The number of views bound indicates how many views are bound to the pipeline
		//// in the API currently according to the tracked state of the context.
		//unsigned int GetViewsBoundCount();

	protected:

		D3D_FEATURE_LEVEL				m_FeatureLevel;

		// The current state of the API is used to allow for caching and elimination
		// of redundant API calls.  This should make it possible to minimize the number
		// of settings that need to be performed.

		OutputMergerStageStateDX11		CurrentState;

		friend PipelineManagerDX11;
	};
};
//--------------------------------------------------------------------------------
#endif // OutputMergerStageDX11_h
//--------------------------------------------------------------------------------

