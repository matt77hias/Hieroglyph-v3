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
// InputAssemblerStageDX11
//
//--------------------------------------------------------------------------------
#ifndef InputAssemblerStageDX11_h
#define InputAssemblerStageDX11_h
//--------------------------------------------------------------------------------
#include "PCH.h"
#include "InputAssemblerStateDX11.h"
//--------------------------------------------------------------------------------
namespace Glyph3
{
	class InputAssemblerStageDX11
	{
	public:
		InputAssemblerStageDX11();
		virtual ~InputAssemblerStageDX11();

		void SetFeautureLevel( D3D_FEATURE_LEVEL level );

		void ClearDesiredState( );
		void ClearCurrentState( );
		void ApplyDesiredState( ID3D11DeviceContext* pContext );

		const InputAssemblerStateDX11& GetCurrentState() const;

		// The desired state is a public member that will allow the user of this
		// class to configure the state as desired before applying the state.

		InputAssemblerStateDX11			DesiredState;

	protected:

		D3D_FEATURE_LEVEL				m_FeatureLevel;

		// The current state of the API is used to allow for caching and elimination
		// of redundant API calls.  This should make it possible to minimize the number
		// of settings that need to be performed.

		InputAssemblerStateDX11			CurrentState;
	};
};
//--------------------------------------------------------------------------------
#endif // InputAssemblerStageDX11_h
//--------------------------------------------------------------------------------

