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
// ShaderStageDX11
//
//--------------------------------------------------------------------------------
#ifndef ShaderStageDX11_h
#define ShaderStageDX11_h
//--------------------------------------------------------------------------------
#include "ShaderDX11.h"
//--------------------------------------------------------------------------------
namespace Glyph3
{
	class ShaderStageDX11
	{
	public:
		ShaderStageDX11();
		virtual ~ShaderStageDX11();

		void SetFeatureLevel( D3D_FEATURE_LEVEL level );

		void SetConstantBuffer( int index, ID3D11Buffer* pBuffer );
		void SetSamplerState( int index, ID3D11SamplerState* pState );
		void SetShaderResourceView( int index, ID3D11ShaderResourceView* pSRV );
		void SetUnorderedAccessView( int index, ID3D11UnorderedAccessView* pUAV, unsigned int initial = -1 );

		virtual ShaderType GetType() = 0;
		void BindResources( ID3D11DeviceContext* pContext );
		void UnbindResources( ID3D11DeviceContext* pContext );

		virtual void BindConstantBuffers( ID3D11DeviceContext* pContext, int count ) = 0;
		virtual void BindSamplerStates( ID3D11DeviceContext* pContext, int count ) = 0;
		virtual void BindShaderResourceViews( ID3D11DeviceContext* pContext, int count ) = 0;
		virtual void BindUnorderedAccessViews( ID3D11DeviceContext* pContext, int count ) = 0;


	protected:

		D3D_FEATURE_LEVEL			m_FeatureLevel;

		// TODO: Set up some way to selectively record the current state of the 
		//       pipeline, then only set the needed shader slots instead of always
		//       setting all of them.

		ID3D11Buffer*				ConstantBuffers[D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT];
		ID3D11SamplerState*			SamplerStates[D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT];
		ID3D11ShaderResourceView*	ShaderResourceViews[D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT];
		ID3D11UnorderedAccessView*	UnorderedAccessViews[D3D11_PS_CS_UAV_REGISTER_COUNT];
		unsigned int				UAVInitCounts[D3D11_PS_CS_UAV_REGISTER_COUNT];

		int iCurrMaxCB, iNextMaxCB;
		int iCurrMaxSS, iNextMaxSS;
		int iCurrMaxSRV, iNextMaxSRV;
		int iCurrMaxUAV, iNextMaxUAV;

		bool bBuffersDirty;
		bool bSamplersDirty;
		bool bShaderResourceViewsDirty;
		bool bUnorderedAccessViewsDirty;

		friend RendererDX11;
	};
};
//--------------------------------------------------------------------------------
#endif // ShaderStageDX11_h
//--------------------------------------------------------------------------------

