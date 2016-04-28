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
TGrowableStructuredBufferDX11<T>::TGrowableStructuredBufferDX11() :
    m_CPUBuffer( nullptr ),
    m_GPUBuffer( nullptr )
{
	// Initialize our buffer to a reasonable size
	SetMaxElementCount( 128 );
}
//--------------------------------------------------------------------------------
template <class T>
TGrowableStructuredBufferDX11<T>::~TGrowableStructuredBufferDX11()
{
}
//--------------------------------------------------------------------------------
template <class T>
void TGrowableStructuredBufferDX11<T>::UploadData( PipelineManagerDX11* pPipeline )
{
	if ( m_uiElementCount > 0 && m_bUploadNeeded == true ) {

		m_bUploadNeeded = false;

		// Map the vertex buffer for writing

		D3D11_MAPPED_SUBRESOURCE resource = 
			pPipeline->MapResource( m_CPUBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0 );

		// Only copy as much of the data as you actually have filled up
	
		memcpy( resource.pData, m_pDataArray, m_uiElementCount * sizeof( T ) );

		// Unmap the vertex buffer

		pPipeline->UnMapResource( m_CPUBuffer, 0 );

		// Copy the data to the GPU buffer

		pPipeline->CopyResource( m_GPUBuffer, m_CPUBuffer );
	}
}
//--------------------------------------------------------------------------------
template <class T>
ResourcePtr TGrowableStructuredBufferDX11<T>::GetBuffer()
{
	return( m_GPUBuffer );
}
//--------------------------------------------------------------------------------
template <class T>
void TGrowableStructuredBufferDX11<T>::CreateResource( unsigned int elements )
{
	// Create the new structured buffers according to the new size.

	BufferConfigDX11 sbuffer;
	sbuffer.SetDefaultStructuredBuffer( m_uiMaxElementCount * sizeof( T ), true );
	sbuffer.SetUsage( D3D11_USAGE_STAGING );
	sbuffer.SetBindFlags( 0 );
	sbuffer.SetCPUAccessFlags( D3D11_CPU_ACCESS_WRITE );
	m_CPUBuffer = RendererDX11::Get()->CreateStructuredBuffer( &sbuffer, nullptr );

	sbuffer.SetDefaultStructuredBuffer( m_uiMaxElementCount * sizeof( T ), true );
	sbuffer.SetBindFlags( D3D11_BIND_SHADER_RESOURCE );
	m_GPUBuffer = RendererDX11::Get()->CreateStructuredBuffer( &sbuffer, nullptr );
}
//--------------------------------------------------------------------------------
template <class T>
void TGrowableStructuredBufferDX11<T>::DeleteResource( )
{
	// Delete the existing resource if it already existed

	if ( nullptr != m_CPUBuffer ) {
		RendererDX11::Get()->DeleteResource( m_CPUBuffer );
		m_CPUBuffer = nullptr;
		RendererDX11::Get()->DeleteResource( m_GPUBuffer );
		m_GPUBuffer = nullptr;
	}
}
//--------------------------------------------------------------------------------
