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
// TGrowableBufferDX11
//
// This template class provides 
//--------------------------------------------------------------------------------
#ifndef TGrowableBufferDX11_h
#define TGrowableBufferDX11_h
//--------------------------------------------------------------------------------
#include "PipelineManagerDX11.h"
#include "BufferConfigDX11.h"
//--------------------------------------------------------------------------------
namespace Glyph3
{
	template <class T>
	class TGrowableBufferDX11
	{
	public:
		TGrowableBufferDX11();
		virtual ~TGrowableBufferDX11();

		// Setting the size of the buffer will create a new array, and 
		// copy as much of the existing array as possible.

		void SetMaxElementCount( unsigned int max );
		
		unsigned int GetMaxElementCount();
		unsigned int GetElementCount();

		
		// Elements are added one at a time, with a template method.

		void AddElement( const T& element );
		

		// These methods allow the user to either upload the data to
		// the buffer resource, or alternatively they can 'reset' the 
		// data, which essentially just resets the element counters
		// (i.e. - no data is actually cleared from the system memory
		// array).  Each subclass is responsible for properly implementing
        // the upload method.

		virtual void UploadData( PipelineManagerDX11* pPipeline ) = 0;
		void ResetData();

		virtual ResourcePtr GetBuffer() = 0;

        // These methods are used internally by the growable buffer to allocate 
		// and deallocate the resources to be used.  This lets each subclass
		// determine the type and number of resources to use, giving very good 
		// future flexibility.  Because we leave this flexible for the subclasses
        // to implement, we also include the GetBuffer method into the required
        // methods to implement.

	protected:
        virtual void CreateResource( unsigned int elements ) = 0;
        virtual void DeleteResource( ) = 0;

		void EnsureCapacity( );

		// The sizes
		unsigned int m_uiMaxElementCount;
		unsigned int m_uiElementCount;

		bool m_bUploadNeeded;

		// The pointer to our array of vertex data
		T* m_pDataArray;
	};

#include "TGrowableBufferDX11.inl"
};
//--------------------------------------------------------------------------------
#endif // TGrowableBufferDX11_h
//--------------------------------------------------------------------------------