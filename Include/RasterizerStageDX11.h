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
// RasterizerStageDX11
//
//--------------------------------------------------------------------------------
#ifndef RasterizerStageDX11_h
#define RasterizerStageDX11_h
//--------------------------------------------------------------------------------
#include "PCH.h"
#include "RasterizerStageStateDX11.h"
//--------------------------------------------------------------------------------
namespace Glyph3
{
	class RasterizerStageDX11
	{
	public:
		RasterizerStageDX11();
		virtual ~RasterizerStageDX11();

		void SetFeatureLevel( D3D_FEATURE_LEVEL level );

		void ClearDesiredState( );
		void ClearCurrentState( );
		void ApplyDesiredState( ID3D11DeviceContext* pContext );

		const RasterizerStageStateDX11& GetCurrentState() const;

		// The desired state is a public member that will allow the user of this
		// class to configure the state as desired before applying the state.

		RasterizerStageStateDX11		DesiredState;

	protected:

		D3D_FEATURE_LEVEL				m_FeatureLevel;

		// The current state of the API is used to allow for caching and elimination
		// of redundant API calls.  This should make it possible to minimize the number
		// of settings that need to be performed.

		RasterizerStageStateDX11		CurrentState;
	};
};
//--------------------------------------------------------------------------------
#endif // RasterizerStageDX11_h
//--------------------------------------------------------------------------------

