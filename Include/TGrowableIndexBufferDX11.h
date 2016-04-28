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
// TGrowableIndexBufferDX11
//
//--------------------------------------------------------------------------------
#ifndef TGrowableIndexBufferDX11_h
#define TGrowableIndexBufferDX11_h
//--------------------------------------------------------------------------------
#include "PipelineManagerDX11.h"
#include "TGrowableBufferDX11.h"
//--------------------------------------------------------------------------------
namespace Glyph3
{
	template <class T>
	class TGrowableIndexBufferDX11 : public TGrowableBufferDX11<T>
	{
	public:
		TGrowableIndexBufferDX11();
		virtual ~TGrowableIndexBufferDX11();

		virtual void UploadData( PipelineManagerDX11* pPipeline );
		virtual ResourcePtr GetBuffer();

	protected:
        virtual void CreateResource( unsigned int elements );
        virtual void DeleteResource( );

	private:
    	ResourcePtr m_IB;
	};

#include "TGrowableIndexBufferDX11.inl"
};
//--------------------------------------------------------------------------------
#endif // TGrowableIndexBufferDX11_h
//--------------------------------------------------------------------------------