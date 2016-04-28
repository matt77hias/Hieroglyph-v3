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
template <class T>
TGrowableIndexBufferDX11<T>::TGrowableIndexBufferDX11() :
    m_IB( nullptr )
{
	// Initialize our buffer to a reasonable size
	SetMaxElementCount( 128 );
}
//--------------------------------------------------------------------------------
template <class T>
TGrowableIndexBufferDX11<T>::~TGrowableIndexBufferDX11()
{
}
//--------------------------------------------------------------------------------
template <class T>
void TGrowableIndexBufferDX11<T>::UploadData( PipelineManagerDX11* pPipeline )
{
	if ( m_uiElementCount > 0 && m_bUploadNeeded == true ) {

		m_bUploadNeeded = false;

		// Map the index buffer for writing

		D3D11_MAPPED_SUBRESOURCE resource = 
			pPipeline->MapResource( m_IB, 0, D3D11_MAP_WRITE_DISCARD, 0 );

		// Only copy as much of the data as you actually have filled up
	
		memcpy( resource.pData, m_pDataArray, m_uiElementCount * sizeof( T ) );

		// Unmap the index buffer

		pPipeline->UnMapResource( m_IB, 0 );
	}
}
//--------------------------------------------------------------------------------
template <class T>
ResourcePtr TGrowableIndexBufferDX11<T>::GetBuffer()
{
    return( m_IB );
}
//--------------------------------------------------------------------------------
template <class T>
void TGrowableIndexBufferDX11<T>::CreateResource( unsigned int elements )
{
	// Create the new index buffer, with the dynamic flag set to true

	BufferConfigDX11 ibuffer;
	ibuffer.SetDefaultIndexBuffer( elements * sizeof( T ), true );
	m_IB = RendererDX11::Get()->CreateIndexBuffer( &ibuffer, nullptr );
}
//--------------------------------------------------------------------------------
template <class T>
void TGrowableIndexBufferDX11<T>::DeleteResource( )
{
	// Delete the existing resource if it already existed

	if ( nullptr != m_IB ) {
		RendererDX11::Get()->DeleteResource( m_IB );
		m_IB = nullptr;
	}
}
//--------------------------------------------------------------------------------
