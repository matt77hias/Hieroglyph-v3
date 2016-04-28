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
// StreamOutputStageDX11
//
//--------------------------------------------------------------------------------
#ifndef StreamOutputStageDX11_h
#define StreamOutputStageDX11_h
//--------------------------------------------------------------------------------
#include "PCH.h"
#include "ResourceProxyDX11.h"
#include "StreamOutputStageStateDX11.h"
//--------------------------------------------------------------------------------
namespace Glyph3
{
	class PipelineManagerDX11;

	class StreamOutputStageDX11
	{
	public:
		StreamOutputStageDX11();
		virtual ~StreamOutputStageDX11();

		void SetFeautureLevel( D3D_FEATURE_LEVEL level );

		void ClearDesiredState( );
		void ClearCurrentState( );
		void ApplyDesiredState( ID3D11DeviceContext* pContext );

		const StreamOutputStageStateDX11& GetCurrentState() const;

		// The desired state is a public member that will allow the user of this
		// class to configure the state as desired before applying the state.

		StreamOutputStageStateDX11		DesiredState;

	protected:

		D3D_FEATURE_LEVEL				m_FeatureLevel;

		// The current state of the API is used to allow for caching and elimination
		// of redundant API calls.  This should make it possible to minimize the number
		// of settings that need to be performed.

		StreamOutputStageStateDX11		CurrentState;

		friend PipelineManagerDX11;
	};
};
//--------------------------------------------------------------------------------
#endif // StreamOutputStageDX11_h
//--------------------------------------------------------------------------------

