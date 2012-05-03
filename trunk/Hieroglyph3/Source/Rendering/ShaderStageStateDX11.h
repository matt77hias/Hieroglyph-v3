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
// ShaderStageStateDX11
//
//--------------------------------------------------------------------------------
#ifndef ShaderStageStateDX11_h
#define ShaderStageStateDX11_h
//--------------------------------------------------------------------------------
#include "PCH.h"
#include "ResourceProxyDX11.h"
//--------------------------------------------------------------------------------
namespace Glyph3
{
	class VertexStageDX11;
	class HullStageDX11;
	class DomainStageDX11;
	class GeometryStageDX11;
	class PixelStageDX11;
	class ComputeStageDX11;
	class ShaderStageDX11;

	class ShaderStageStateDX11
	{
	public:
		ShaderStageStateDX11();
		virtual ~ShaderStageStateDX11();

		void SetFeautureLevel( D3D_FEATURE_LEVEL level );

		// These methods are provided to allow the user to 'set' particular states.  These states are
		// then tracked for minimizing API calls later on, as well as reducing the number of states 
		// which are compared on the CPU side.

		void SetShaderProgram( int index );
		void SetConstantBuffer( int index, ID3D11Buffer* pBuffer );
		void SetSamplerState( int index, ID3D11SamplerState* pState );
		void SetShaderResourceView( int index, ID3D11ShaderResourceView* pSRV );
		void SetUnorderedAccessView( int index, ID3D11UnorderedAccessView* pUAV, unsigned int initial = -1 );

		int GetShaderProgram();
		
		// These comparison methods are used to determine if two states are the same or not.  The result
		// can then be used to set the appropriate amount of states, or none at all if the result is 0.
		
		int CompareShaderProgramState( ShaderStageStateDX11& desired );
		int CompareConstantBufferState( ShaderStageStateDX11& desired );
		int CompareSamplerStateState( ShaderStageStateDX11& desired );
		int CompareShaderResourceViewState( ShaderStageStateDX11& desired );
		int CompareUnorderedAccessViewState( ShaderStageStateDX11& desired );
		
		void ClearState( );

		void SetSisterState( ShaderStageStateDX11* pState );
		void ResetUpdateFlags( );


	protected:

		D3D_FEATURE_LEVEL			m_FeatureLevel;

		int							m_ShaderIndex;
		ID3D11Buffer*				ConstantBuffers[D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT];
		ID3D11SamplerState*			SamplerStates[D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT];
		ID3D11ShaderResourceView*	ShaderResourceViews[D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT];
		ID3D11UnorderedAccessView*	UnorderedAccessViews[D3D11_PS_CS_UAV_REGISTER_COUNT];
		unsigned int				UAVInitCounts[D3D11_PS_CS_UAV_REGISTER_COUNT];

		ShaderStageStateDX11*		m_pSisterState;

		bool						m_bUpdateShaderIndex;
		bool						m_bUpdateConstantBuffers;
		bool						m_bUpdateSamplerStates;
		bool						m_bUpdateSRVs;
		bool						m_bUpdateUAVs;
		bool						m_bUpdateUAVCounts;

		friend VertexStageDX11;
		friend HullStageDX11;
		friend DomainStageDX11;
		friend GeometryStageDX11;
		friend PixelStageDX11;
		friend ComputeStageDX11;
		friend ShaderStageDX11;
	};
};
//--------------------------------------------------------------------------------
#endif // ShaderStageStateDX11_h
//--------------------------------------------------------------------------------

