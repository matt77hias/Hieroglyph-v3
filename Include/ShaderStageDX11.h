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
// ShaderStageDX11
//
//--------------------------------------------------------------------------------
#ifndef ShaderStageDX11_h
#define ShaderStageDX11_h
//--------------------------------------------------------------------------------
#include "ShaderDX11.h"
#include "ShaderStageStateDX11.h"
//--------------------------------------------------------------------------------
namespace Glyph3
{
	class ShaderStageDX11
	{
	public:
		ShaderStageDX11();
		virtual ~ShaderStageDX11();

		void SetFeatureLevel( D3D_FEATURE_LEVEL level );

		void ClearDesiredState( );
		void ClearCurrentState( );
		void ApplyDesiredState( ID3D11DeviceContext* pContext );

		// This method is used to allow each sub-class to identify what type of stage
		// it is.
		
		virtual ShaderType GetType() = 0;

		// These methods are stubs for the individual stages to implement.  This allows
		// each stage to use the appropriate ID3D11DeviceContext::XSSetYYY() methods.

		virtual void BindShaderProgram( ID3D11DeviceContext* ) = 0;
		virtual void BindConstantBuffers( ID3D11DeviceContext* pContext, int count ) = 0;
		virtual void BindSamplerStates( ID3D11DeviceContext* pContext, int count ) = 0;
		virtual void BindShaderResourceViews( ID3D11DeviceContext* pContext, int count ) = 0;
		virtual void BindUnorderedAccessViews( ID3D11DeviceContext* pContext, int count ) = 0;

		// The desired state is a public member that will allow the user of this
		// class to configure the state as desired before applying the state.

		ShaderStageStateDX11		DesiredState;

	protected:

		D3D_FEATURE_LEVEL			m_FeatureLevel;

		// The current state of the API is used to allow for caching and elimination
		// of redundant API calls.  This should make it possible to minimize the number
		// of settings that need to be performed.

		ShaderStageStateDX11		CurrentState;
	};
};
//--------------------------------------------------------------------------------
#endif // ShaderStageDX11_h
//--------------------------------------------------------------------------------

