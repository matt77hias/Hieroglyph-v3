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
using namespace Glyph3;
//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
template <class T>
TGrowableIndexBufferDX11<T>::TGrowableIndexBufferDX11()
{
	m_uiMaxIndexCount = 0;
	m_uiIndexCount = 0;
	m_bUploadNeeded = false;

	m_pIndexArray = nullptr;

	// Initialize our buffer to a reasonable size
	SetMaxIndexCount( 128 );
}
//--------------------------------------------------------------------------------
template <class T>
TGrowableIndexBufferDX11<T>::~TGrowableIndexBufferDX11()
{
	SAFE_DELETE_ARRAY( m_pIndexArray );
}
//--------------------------------------------------------------------------------
template <class T>
void TGrowableIndexBufferDX11<T>::SetMaxIndexCount( unsigned int maxIndices )
{
	// if this is different than the current size, then release the existing
	// local array, plus resize the resource.

	if ( maxIndices != m_uiMaxIndexCount ) {

		// Create the new array, temporarily in a local variable.
		T* pNewArray = new T[maxIndices];

		// Truncate the index count if more than the new max are already loaded 
		// which could happen if the array is being shrunken.
		if ( m_uiIndexCount > maxIndices ) {
			m_uiIndexCount = maxIndices;
		}

		// Copy the existing index data over, if any has been added.
		if ( m_uiIndexCount > 0 ) {
			memcpy( pNewArray, m_pIndexArray, m_uiIndexCount * sizeof( T ) );
		}

		// Remember the maximum number of indices to allow, and the 
		// current count of indices is left as it is.
		m_uiMaxIndexCount = maxIndices;

		// Release system memory for the old array so that we can set a new one
		SAFE_DELETE_ARRAY( m_pIndexArray );
		m_pIndexArray = pNewArray;

		// Delete the existing resource if it already existed
		if ( nullptr != m_IB ) {
			RendererDX11::Get()->DeleteResource( m_IB );
			m_IB = nullptr;
		}

		// Create the new index buffer, with the dynamic flag set to true
		BufferConfigDX11 ibuffer;
		ibuffer.SetDefaultIndexBuffer( m_uiMaxIndexCount * sizeof( T ), true );
		m_IB = RendererDX11::Get()->CreateIndexBuffer( &ibuffer, nullptr );
		
		m_bUploadNeeded = true;
	}
}
//--------------------------------------------------------------------------------
template <class T>
unsigned int TGrowableIndexBufferDX11<T>::GetMaxIndexCount()
{
	return( m_uiMaxIndexCount );
}
//--------------------------------------------------------------------------------
template <class T>
unsigned int TGrowableIndexBufferDX11<T>::GetIndexCount()
{
	return( m_uiIndexCount );
}
//--------------------------------------------------------------------------------
template <class T>
void TGrowableIndexBufferDX11<T>::AddIndex( const T& index )
{
	EnsureIndexCapacity( );

	m_pIndexArray[m_uiIndexCount] = index;
	m_uiIndexCount++;

	m_bUploadNeeded = true;
}
//--------------------------------------------------------------------------------
template <class T>
void TGrowableIndexBufferDX11<T>::UploadIndexData( PipelineManagerDX11* pPipeline )
{
	if ( m_uiIndexCount > 0 && m_bUploadNeeded == true ) {

		m_bUploadNeeded = false;

		// Map the index buffer for writing

		D3D11_MAPPED_SUBRESOURCE resource = 
			pPipeline->MapResource( m_IB, 0, D3D11_MAP_WRITE_DISCARD, 0 );

		// Only copy as much of the data as you actually have filled up
	
		memcpy( resource.pData, m_pIndexArray, m_uiIndexCount * sizeof( T ) );

		// Unmap the index buffer

		pPipeline->UnMapResource( m_IB, 0 );
	}
}
//--------------------------------------------------------------------------------
template <class T>
void TGrowableIndexBufferDX11<T>::ResetIndices()
{
	// Reset the index count here to prepare for the next drawing pass.
	m_uiIndexCount = 0;

	m_bUploadNeeded = false;
}
//--------------------------------------------------------------------------------
template <class T>
void TGrowableIndexBufferDX11<T>::EnsureIndexCapacity( )
{
	// If the next index would put us over the limit, then resize the array.
	if ( m_uiIndexCount + 1 >= m_uiMaxIndexCount ) {
		SetMaxIndexCount( m_uiMaxIndexCount + 1024 );
	}
}
//--------------------------------------------------------------------------------
template <class T>
ResourcePtr TGrowableIndexBufferDX11<T>::GetIndexBuffer()
{
	return( m_IB );
}
//--------------------------------------------------------------------------------
