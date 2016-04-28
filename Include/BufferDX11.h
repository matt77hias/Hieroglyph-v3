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
// BufferDX11
//
// This interface provides the function interface to the information 
// contained in the D3D11 buffer class.  
//--------------------------------------------------------------------------------
#ifndef BufferDX11_h
#define BufferDX11_h
//--------------------------------------------------------------------------------
#include "ResourceDX11.h"
//--------------------------------------------------------------------------------
namespace Glyph3
{
	class BufferDX11 : public ResourceDX11
	{
	public:
		BufferDX11();
		virtual ~BufferDX11();

		D3D11_BUFFER_DESC			GetActualDescription();
		D3D11_BUFFER_DESC			GetDesiredDescription();
		void						SetDesiredDescription( D3D11_BUFFER_DESC desc );

		UINT						GetByteWidth();
		void						SetByteWidth( UINT width );

		D3D11_USAGE					GetUsage();
		void						SetUsage( D3D11_USAGE usage );

		UINT						GetBindFlags();
		void						SetBindFlags( UINT flags );

		UINT						GetCPUAccessFlags();
		void						SetCPUAccessFlags( UINT flags );

		UINT						GetMiscFlags();
		void						SetMiscFlags( UINT flags );

		UINT						GetStructureByteStride();
		void						SetStructureByteStride( UINT stride );

		void*						Map();
		void						UnMap();

		virtual ID3D11Resource*				GetResource();

		virtual UINT						GetEvictionPriority();
		virtual void						SetEvictionPriority( UINT EvictionPriority );

	protected:
		Microsoft::WRL::ComPtr<ID3D11Buffer>		m_pBuffer;
		D3D11_BUFFER_DESC							m_DesiredDesc;
		D3D11_BUFFER_DESC							m_ActualDesc;

		friend RendererDX11;
	};
};
//--------------------------------------------------------------------------------
#endif // BufferDX11_h
//--------------------------------------------------------------------------------

