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
// ByteAddressBufferDX11 
//
//--------------------------------------------------------------------------------
#ifndef ByteAddressBufferDX11_h
#define ByteAddressBufferDX11_h
//--------------------------------------------------------------------------------
#include "BufferDX11.h"
//--------------------------------------------------------------------------------
namespace Glyph3
{
	class ByteAddressBufferDX11 : public BufferDX11
	{
	public:
		ByteAddressBufferDX11( ID3D11Buffer* pBuffer );
		virtual ~ByteAddressBufferDX11();

		//void						SetVertexSize( int size );
		//void						SetVertexCount( int count );

	protected:
		//int							m_iVertexSize;
		//int							m_iVertexCount;

		friend RendererDX11;
	};
};
//--------------------------------------------------------------------------------
#endif // ByteAddressBufferDX11_h
