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
// TGrowableIndexBufferDX11
//
// This template class provides 
//--------------------------------------------------------------------------------
#ifndef TGrowableIndexBufferDX11_h
#define TGrowableIndexBufferDX11_h
//--------------------------------------------------------------------------------
#include "PipelineManagerDX11.h"
//--------------------------------------------------------------------------------
namespace Glyph3
{
	template <class T>
	class TGrowableIndexBufferDX11
	{
	public:
		TGrowableIndexBufferDX11();
		~TGrowableIndexBufferDX11();


		// Setting the size of the index buffer will create a new array, and 
		// copy as much of the existing array as possible.

		void SetMaxIndexCount( unsigned int maxIndices );
		
		unsigned int GetMaxIndexCount();
		unsigned int GetIndexCount();

		
		// Indices are added one at a time, with a template method.

		void AddIndex( const T& index );
		

		// These methods allow the user to either upload the index data to
		// the buffer resource, or alternatively they can 'reset' the 
		// indices, which essentially just resets the index counters
		// (i.e. - no data is actually cleared from the system memory
		// array).

		void UploadIndexData( PipelineManagerDX11* pPipeline );
		void ResetIndices();


		ResourcePtr GetIndexBuffer();

	private:

		void EnsureIndexCapacity( );

		ResourcePtr m_IB;

		// The sizes
		unsigned int m_uiMaxIndexCount;
		unsigned int m_uiIndexCount;

		bool m_bUploadNeeded;

		// The pointer to our array of index data
		T* m_pIndexArray;

	};

#include "TGrowableIndexBufferDX11.inl"
};
//--------------------------------------------------------------------------------
#endif // TGrowableIndexBufferDX11_h
