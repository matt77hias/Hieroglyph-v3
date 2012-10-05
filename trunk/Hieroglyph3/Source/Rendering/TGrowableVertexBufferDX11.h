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
// TGrowableVertexBufferDX11
//
// This template class provides 
//--------------------------------------------------------------------------------
#ifndef TGrowableVertexBufferDX11_h
#define TGrowableVertexBufferDX11_h
//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
namespace Glyph3
{
	template <class T>
	class TGrowableVertexBufferDX11
	{
	public:
		TGrowableVertexBufferDX11();
		~TGrowableVertexBufferDX11();


		// Setting the size of the vertex buffer will create a new array, and 
		// copy as much of the existing array as possible.

		void SetMaxVertexCount( unsigned int maxVertices );
		
		unsigned int GetMaxVertexCount();
		unsigned int GetVertexCount();

		
		// Vertices are added one at a time, with a template method.

		void AddVertex( const T& vertex );
		

		// These methods allow the user to either upload the vertex data to
		// the buffer resource, or alternatively they can 'reset' the 
		// vertices, which essentially just resets the vertex counters
		// (i.e. - no data is actually cleared from the system memory
		// array).

		void UploadVertexData( PipelineManagerDX11* pPipeline );
		void ResetVertices();


		ResourcePtr GetVertexBuffer();

	private:

		void EnsureVertexCapacity( );

		ResourcePtr m_VB;

		// The sizes
		unsigned int m_uiMaxVertexCount;
		unsigned int m_uiVertexCount;

		bool m_bUploadNeeded;

		// The pointer to our array of vertex data
		T* m_pVertexArray;

	};

#include "TGrowableVertexBufferDX11.inl"
};
//--------------------------------------------------------------------------------
#endif // TGrowableVertexBufferDX11_h
