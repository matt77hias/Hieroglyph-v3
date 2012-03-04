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
// ConstantBufferDX11 
//
//--------------------------------------------------------------------------------
#ifndef ConstantBufferDX11_h
#define ConstantBufferDX11_h
//--------------------------------------------------------------------------------
#include "BufferDX11.h"
//--------------------------------------------------------------------------------
namespace Glyph3
{
	class ConstantBufferDX11 : public BufferDX11
	{
	public:
		ConstantBufferDX11( ID3D11Buffer* pBuffer );
		virtual ~ConstantBufferDX11();

		virtual ResourceType				GetType();

		void						SetConstantSize( int size );
		void						SetConstantCount( int count );
		void						SetAutoUpdate( bool enable );
		bool						GetAutoUpdate();

	protected:
		int							m_iConstantSize;
		int							m_iConstantCount;
		bool						m_bAutoUpdate;

		friend RendererDX11;
	};
};
//--------------------------------------------------------------------------------
#endif // ConstantBufferDX11_h
