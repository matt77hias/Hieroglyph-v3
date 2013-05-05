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
// TGrowableStructuredBufferDX11
//
// This template class provides 
//--------------------------------------------------------------------------------
#ifndef TGrowableStructuredBufferDX11_h
#define TGrowableStructuredBufferDX11_h
//--------------------------------------------------------------------------------
#include "PipelineManagerDX11.h"
#include "BufferConfigDX11.h"
//--------------------------------------------------------------------------------
namespace Glyph3
{
	template <class T>
	class TGrowableStructuredBufferDX11
	{
	public:
		TGrowableStructuredBufferDX11();
		~TGrowableStructuredBufferDX11();


		// Setting the size of the structured buffer will create a new array, and 
		// copy as much of the existing array as possible.

		void SetMaxStructureCount( unsigned int maxStructures );
		
		unsigned int GetMaxStructureCount();
		unsigned int GetStructureCount();

		
		// Structures are added one at a time, with a template method.

		void AddStruct( const T& struct );
		

		// These methods allow the user to either upload the struct data to
		// the buffer resource, or alternatively they can 'reset' the 
		// vertices, which essentially just resets the vertex counters
		// (i.e. - no data is actually cleared from the system memory
		// array).

		void UploadStructureData( PipelineManagerDX11* pPipeline );
		void ResetStructures();


		ResourcePtr GetStructuredBuffer();

	private:

		void EnsureBufferCapacity( );

		ResourcePtr m_CPUBuffer;
		ResourcePtr m_GPUBuffer;

		// The sizes
		unsigned int m_uiMaxStructCount;
		unsigned int m_uiStructCount;

		bool m_bUploadNeeded;

		// The pointer to our array of vertex data
		T* m_pStructArray;

	};

#include "TGrowableStructuredBufferDX11.inl"
};
//--------------------------------------------------------------------------------
#endif // TGrowableStructuredBufferDX11_h
