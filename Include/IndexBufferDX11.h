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
// IndexBufferDX11 
//
//--------------------------------------------------------------------------------
#ifndef IndexBufferDX11_h
#define IndexBufferDX11_h
//--------------------------------------------------------------------------------
#include "BufferDX11.h"
//--------------------------------------------------------------------------------
namespace Glyph3
{
	class IndexBufferDX11 : public BufferDX11
	{
	public:
		IndexBufferDX11( Microsoft::WRL::ComPtr<ID3D11Buffer> pBuffer );
		virtual ~IndexBufferDX11();

		virtual ResourceType				GetType();

		void						SetIndexSize( int size );
		void						SetIndexCount( int count );

	protected:
		int							m_iIndexSize;
		int							m_iIndexCount;

		friend RendererDX11;
	};
};
//--------------------------------------------------------------------------------
#endif // IndexBufferDX11_h
