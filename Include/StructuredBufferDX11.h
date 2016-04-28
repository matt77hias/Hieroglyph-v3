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
// StructuredBufferDX11 
//
//--------------------------------------------------------------------------------
#ifndef StructuredBufferDX11_h
#define StructuredBufferDX11_h
//--------------------------------------------------------------------------------
#include "BufferDX11.h"
//--------------------------------------------------------------------------------
namespace Glyph3
{
	class StructuredBufferDX11 : public BufferDX11
	{
	public:
		StructuredBufferDX11( Microsoft::WRL::ComPtr<ID3D11Buffer> pBuffer );
		virtual ~StructuredBufferDX11();

		virtual ResourceType				GetType();

	protected:

		friend RendererDX11;
	};
};
//--------------------------------------------------------------------------------
#endif // StructuredBufferDX11_h
