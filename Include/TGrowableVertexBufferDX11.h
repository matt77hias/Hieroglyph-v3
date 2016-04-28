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
// TGrowableVertexBufferDX11
//
//--------------------------------------------------------------------------------
#ifndef TGrowableVertexBufferDX11_h
#define TGrowableVertexBufferDX11_h
//--------------------------------------------------------------------------------
#include "PipelineManagerDX11.h"
#include "TGrowableBufferDX11.h"
//--------------------------------------------------------------------------------
namespace Glyph3
{
	template <class T>
	class TGrowableVertexBufferDX11 : public TGrowableBufferDX11<T>
	{
	public:
		TGrowableVertexBufferDX11();
		virtual ~TGrowableVertexBufferDX11();

		virtual void UploadData( PipelineManagerDX11* pPipeline );
		virtual ResourcePtr GetBuffer();

	protected:
        virtual void CreateResource( unsigned int elements );
        virtual void DeleteResource( );

	private:
		ResourcePtr m_VB;
	};

#include "TGrowableVertexBufferDX11.inl"
};
//--------------------------------------------------------------------------------
#endif // TGrowableVertexBufferDX11_h
//--------------------------------------------------------------------------------