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
// VertexBufferDX11 
//
//--------------------------------------------------------------------------------
#ifndef VertexBufferDX11_h
#define VertexBufferDX11_h
//--------------------------------------------------------------------------------
#include "BufferDX11.h"
//--------------------------------------------------------------------------------
namespace Glyph3
{
	class VertexBufferDX11 : public BufferDX11
	{
	public:
		VertexBufferDX11( Microsoft::WRL::ComPtr<ID3D11Buffer> pBuffer );
		virtual ~VertexBufferDX11();

		virtual ResourceType				GetType();

		void						SetVertexSize( int size );
		void						SetVertexCount( int count );

	protected:
		int							m_iVertexSize;
		int							m_iVertexCount;

		friend RendererDX11;
	};
};
//--------------------------------------------------------------------------------
#endif // VertexBufferDX11_h
