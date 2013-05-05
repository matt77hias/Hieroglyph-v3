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

//--------------------------------------------------------------------------------
template <class T>
TGrowableStructuredBufferDX11<T>::TGrowableStructuredBufferDX11()
{
	m_uiMaxStructCount = 0;
	m_uiStructCount = 0;
	m_bUploadNeeded = false;

	m_pStructArray = nullptr;

	// Initialize our buffer to a reasonable size
	SetMaxStructureCount( 128 );
}
//--------------------------------------------------------------------------------
template <class T>
TGrowableStructuredBufferDX11<T>::~TGrowableStructuredBufferDX11()
{
	SAFE_DELETE_ARRAY( m_pStructArray );
}
//--------------------------------------------------------------------------------
template <class T>
void TGrowableStructuredBufferDX11<T>::SetMaxStructureCount( unsigned int maxStructures )
{
	// if this is different than the current size, then release the existing
	// local array, plus resize the resource.

	if ( maxStructures != m_uiMaxStructCount ) {

		// Create the new array, temporarily in a local variable.
		T* pNewArray = new T[maxStructures];

		// Truncate the vertex count if more than the new max are already loaded 
		// which could happen if the array is being shrunken.
		if ( m_uiStructCount > maxStructures ) {
			m_uiStructCount = maxStructures;
		}

		// Copy the existing vertex data over, if any has been added.
		if ( m_uiStructCount > 0 ) {
			memcpy( pNewArray, m_pStructArray, m_uiStructCount * sizeof( T ) );
		}

		// Remember the maximum number of vertices to allow, and the 
		// current count of vertices is left as it is.
		m_uiMaxStructCount = maxStructures;

		// Release system memory for the old array so that we can set a new one
		SAFE_DELETE_ARRAY( m_pStructArray );
		m_pStructArray = pNewArray;

		// Delete the existing resource if it already existed
		if ( nullptr != m_CPUBuffer ) {
			RendererDX11::Get()->DeleteResource( m_CPUBuffer );
			m_CPUBuffer = nullptr;
			RendererDX11::Get()->DeleteResource( m_GPUBuffer );
			m_GPUBuffer = nullptr;
		}

		// Create the new structured buffers according to the new size.
		BufferConfigDX11 sbuffer;
		sbuffer.SetDefaultStructuredBuffer( m_uiMaxStructCount * sizeof( T ), true );
		sbuffer.SetUsage( D3D11_USAGE_STAGING );
		sbuffer.SetBindFlags( 0 );
		sbuffer.SetCPUAccessFlags( D3D11_CPU_ACCESS_WRITE );
		m_CPUBuffer = RendererDX11::Get()->CreateStructuredBuffer( &vbuffer, nullptr );

		sbuffer.SetDefaultStructuredBuffer( m_uiMaxStructCount * sizeof( T ), true );
		sbuffer.SetBindFlags( D3D11_BIND_SHADER_RESOURCE );
		m_GPUBuffer = RendererDX11::Get()->CreateStructuredBuffer( &vbuffer, nullptr );

		m_bUploadNeeded = true;
	}
}
//--------------------------------------------------------------------------------
template <class T>
unsigned int TGrowableStructuredBufferDX11<T>::GetMaxStructureCount()
{
	return( m_uiMaxStructCount );
}
//--------------------------------------------------------------------------------
template <class T>
unsigned int TGrowableStructuredBufferDX11<T>::GetStructureCount()
{
	return( m_uiStructCount );
}
//--------------------------------------------------------------------------------
template <class T>
void TGrowableStructuredBufferDX11<T>::AddStruct( const T& vertex )
{
	EnsureStructureCapacity( );

	m_pStructArray[m_uiStructCount] = vertex;
	m_uiStructCount++;

	m_bUploadNeeded = true;
}
//--------------------------------------------------------------------------------
template <class T>
void TGrowableStructuredBufferDX11<T>::UploadStructureData( PipelineManagerDX11* pPipeline )
{
	if ( m_uiStructCount > 0 && m_bUploadNeeded == true ) {

		m_bUploadNeeded = false;

		// Map the vertex buffer for writing

		D3D11_MAPPED_SUBRESOURCE resource = 
			pPipeline->MapResource( m_CPUBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0 );

		// Only copy as much of the data as you actually have filled up
	
		memcpy( resource.pData, m_pStructArray, m_uiStructCount * sizeof( T ) );

		// Unmap the vertex buffer

		pPipeline->UnMapResource( m_CPUBuffer, 0 );

		// Copy the data to the GPU buffer

		pPipeline->CopyResource( m_GPUBuffer, m_CPUBuffer );
	}
}
//--------------------------------------------------------------------------------
template <class T>
void TGrowableStructuredBufferDX11<T>::ResetStructures()
{
	// Reset the vertex count here to prepare for the next drawing pass.
	m_uiStructCount = 0;

	m_bUploadNeeded = false;
}
//--------------------------------------------------------------------------------
template <class T>
void TGrowableStructuredBufferDX11<T>::EnsureBufferCapacity( )
{
	// If the next vertex would put us over the limit, then resize the array.
	if ( m_uiStructCount + 1 >= m_uiMaxStructCount ) {
		SetMaxStructureCount( m_uiMaxStructCount + 1024 );
	}
}
//--------------------------------------------------------------------------------
template <class T>
ResourcePtr TGrowableStructuredBufferDX11<T>::GetStructuredBuffer()
{
	return( m_GPUBuffer );
}
//--------------------------------------------------------------------------------
