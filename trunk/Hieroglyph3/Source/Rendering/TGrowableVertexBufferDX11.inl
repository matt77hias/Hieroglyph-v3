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
TGrowableVertexBufferDX11<T>::TGrowableVertexBufferDX11()
{
	m_uiMaxVertexCount = 0;
	m_uiVertexCount = 0;
	m_bUploadNeeded = false;

	m_pVertexArray = nullptr;

	// Initialize our buffer to a reasonable size
	SetMaxVertexCount( 128 );
}
//--------------------------------------------------------------------------------
template <class T>
TGrowableVertexBufferDX11<T>::~TGrowableVertexBufferDX11()
{
	SAFE_DELETE_ARRAY( m_pVertexArray );
}
//--------------------------------------------------------------------------------
template <class T>
void TGrowableVertexBufferDX11<T>::SetMaxVertexCount( unsigned int maxVertices )
{
	// if this is different than the current size, then release the existing
	// local array, plus resize the resource.

	if ( maxVertices != m_uiMaxVertexCount ) {

		// Create the new array, temporarily in a local variable.
		T* pNewArray = new T[maxVertices];

		// Truncate the vertex count if more than the new max are already loaded 
		// which could happen if the array is being shrunken.
		if ( m_uiVertexCount > maxVertices ) {
			m_uiVertexCount = maxVertices;
		}

		// Copy the existing vertex data over, if any has been added.
		if ( m_uiVertexCount > 0 ) {
			memcpy( pNewArray, m_pVertexArray, m_uiVertexCount * sizeof( T ) );
		}

		// Remember the maximum number of vertices to allow, and the 
		// current count of vertices is left as it is.
		m_uiMaxVertexCount = maxVertices;

		// Release system memory for the old array so that we can set a new one
		SAFE_DELETE_ARRAY( m_pVertexArray );
		m_pVertexArray = pNewArray;

		// Delete the existing resource if it already existed
		if ( nullptr != m_VB ) {
			RendererDX11::Get()->DeleteResource( m_VB );
			m_VB = nullptr;
		}

		// Create the new vertex buffer, with the dynamic flag set to true
		BufferConfigDX11 vbuffer;
		vbuffer.SetDefaultVertexBuffer( m_uiMaxVertexCount * sizeof( T ), true );
		m_VB = RendererDX11::Get()->CreateVertexBuffer( &vbuffer, nullptr );

		m_bUploadNeeded = true;
	}
}
//--------------------------------------------------------------------------------
template <class T>
unsigned int TGrowableVertexBufferDX11<T>::GetMaxVertexCount()
{
	return( m_uiMaxVertexCount );
}
//--------------------------------------------------------------------------------
template <class T>
unsigned int TGrowableVertexBufferDX11<T>::GetVertexCount()
{
	return( m_uiVertexCount );
}
//--------------------------------------------------------------------------------
template <class T>
void TGrowableVertexBufferDX11<T>::AddVertex( const T& vertex )
{
	EnsureVertexCapacity( );

	m_pVertexArray[m_uiVertexCount] = vertex;
	m_uiVertexCount++;

	m_bUploadNeeded = true;
}
//--------------------------------------------------------------------------------
template <class T>
void TGrowableVertexBufferDX11<T>::UploadVertexData( PipelineManagerDX11* pPipeline )
{
	if ( m_uiVertexCount > 0 && m_bUploadNeeded == true ) {

		m_bUploadNeeded = false;

		// Map the vertex buffer for writing

		D3D11_MAPPED_SUBRESOURCE resource = 
			pPipeline->MapResource( m_VB, 0, D3D11_MAP_WRITE_DISCARD, 0 );

		// Only copy as much of the data as you actually have filled up
	
		memcpy( resource.pData, m_pVertexArray, m_uiVertexCount * sizeof( T ) );

		// Unmap the vertex buffer

		pPipeline->UnMapResource( m_VB, 0 );
	}
}
//--------------------------------------------------------------------------------
template <class T>
void TGrowableVertexBufferDX11<T>::ResetVertices()
{
	// Reset the vertex count here to prepare for the next drawing pass.
	m_uiVertexCount = 0;

	m_bUploadNeeded = false;
}
//--------------------------------------------------------------------------------
template <class T>
void TGrowableVertexBufferDX11<T>::EnsureVertexCapacity( )
{
	// If the next vertex would put us over the limit, then resize the array.
	if ( m_uiVertexCount + 1 >= m_uiMaxVertexCount ) {
		SetMaxVertexCount( m_uiMaxVertexCount + 1024 );
	}
}
//--------------------------------------------------------------------------------
template <class T>
ResourcePtr TGrowableVertexBufferDX11<T>::GetVertexBuffer()
{
	return( m_VB );
}
//--------------------------------------------------------------------------------
