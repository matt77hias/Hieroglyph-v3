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
// InputAssemblerStateDX11
//
//--------------------------------------------------------------------------------
#ifndef InputAssemblerStateDX11_h
#define InputAssemblerStateDX11_h
//--------------------------------------------------------------------------------
#include "PCH.h"
#include "ResourceProxyDX11.h"
//--------------------------------------------------------------------------------
namespace Glyph3
{
	class InputAssemblerStageDX11;

	class InputAssemblerStateDX11
	{
	public:
		InputAssemblerStateDX11();
		virtual ~InputAssemblerStateDX11();

		void SetFeautureLevel( D3D_FEATURE_LEVEL level );

		void SetIndexBuffer( int buffer );
		void SetIndexBufferFormat( DXGI_FORMAT format );
		void SetVertexBuffer( unsigned int slot, int buffer, unsigned int offset, unsigned int stride );
		void SetInputLayout( int layout );
		void SetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY topology );

		
		D3D11_PRIMITIVE_TOPOLOGY GetPrimitiveTopology();
		UINT GetVertexStride( UINT slot );
		UINT GetVertexOffset( UINT slot );

		int CompareVertexBufferState( InputAssemblerStateDX11& desired );

		void ClearState( );

		void SetSisterState( InputAssemblerStateDX11* pState );
		void ResetUpdateFlags( );

	protected:

		D3D_FEATURE_LEVEL				m_FeatureLevel;

		int								IndexBuffer;
		DXGI_FORMAT						IndexBufferFormat;
		int								VertexBuffers[D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT];
		UINT							VertexStrides[D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT];
		UINT							VertexOffsets[D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT];
		int								InputLayout;
		D3D11_PRIMITIVE_TOPOLOGY		PrimitiveTopology;

		unsigned int					AvailableSlotCount;


		InputAssemblerStateDX11*		m_pSisterState;

		bool							m_bUpdateIndexBuffer;
		bool							m_bUpdateVertexBuffers;
		bool							m_bUpdateInputLayout;
		bool							m_bUpdateUpdatePrimitiveTopology;

		friend InputAssemblerStageDX11;
	};
};
//--------------------------------------------------------------------------------
#endif // InputAssemblerStateDX11_h
//--------------------------------------------------------------------------------

