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
#include "PCH.h"
#include "BufferDX11.h"
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
BufferDX11::BufferDX11()
{
	ZeroMemory( &m_DesiredDesc, sizeof( D3D11_BUFFER_DESC ) );
	ZeroMemory( &m_ActualDesc, sizeof( D3D11_BUFFER_DESC ) );
}
//--------------------------------------------------------------------------------
BufferDX11::~BufferDX11()
{
}
//--------------------------------------------------------------------------------
D3D11_BUFFER_DESC BufferDX11::GetActualDescription()
{
	ZeroMemory( &m_ActualDesc, sizeof( D3D11_BUFFER_DESC ) );

	if ( m_pBuffer )
		m_pBuffer->GetDesc( &m_ActualDesc );

	return( m_ActualDesc );
}
//--------------------------------------------------------------------------------
D3D11_BUFFER_DESC BufferDX11::GetDesiredDescription()
{
	return( m_DesiredDesc );
}
//--------------------------------------------------------------------------------
void BufferDX11::SetDesiredDescription( D3D11_BUFFER_DESC desc )
{
	m_DesiredDesc = desc;
}
//--------------------------------------------------------------------------------
UINT BufferDX11::GetByteWidth()
{
	D3D11_BUFFER_DESC description = GetActualDescription();

	return( description.ByteWidth );
}
//--------------------------------------------------------------------------------
D3D11_USAGE BufferDX11::GetUsage()
{
	D3D11_BUFFER_DESC description = GetActualDescription();

	return( description.Usage );
}
//--------------------------------------------------------------------------------
UINT BufferDX11::GetBindFlags()
{
	D3D11_BUFFER_DESC description = GetActualDescription();

	return( description.BindFlags );
}
//--------------------------------------------------------------------------------
UINT BufferDX11::GetCPUAccessFlags()
{
	D3D11_BUFFER_DESC description = GetActualDescription();

	return( description.CPUAccessFlags );
}
//--------------------------------------------------------------------------------
UINT BufferDX11::GetMiscFlags()
{
	D3D11_BUFFER_DESC description = GetActualDescription();

	return( description.MiscFlags );
}
//--------------------------------------------------------------------------------
UINT BufferDX11::GetStructureByteStride()
{
	D3D11_BUFFER_DESC description = GetActualDescription();

	return( description.StructureByteStride );
}
//--------------------------------------------------------------------------------
void BufferDX11::SetByteWidth( UINT width )
{
	m_DesiredDesc.ByteWidth = width;
}
//--------------------------------------------------------------------------------
void BufferDX11::SetUsage( D3D11_USAGE usage )
{
	m_DesiredDesc.Usage = usage;
}
//--------------------------------------------------------------------------------
void BufferDX11::SetBindFlags( UINT flags )
{
	m_DesiredDesc.BindFlags = flags;
}
//--------------------------------------------------------------------------------
void BufferDX11::SetCPUAccessFlags( UINT flags )
{
	m_DesiredDesc.CPUAccessFlags = flags;
}
//--------------------------------------------------------------------------------
void BufferDX11::SetMiscFlags( UINT flags )
{
	m_DesiredDesc.MiscFlags = flags;
}
//--------------------------------------------------------------------------------
void BufferDX11::SetStructureByteStride( UINT stride )
{
	m_DesiredDesc.StructureByteStride = stride;
}
//--------------------------------------------------------------------------------
void* BufferDX11::Map()
{
	return( 0 );
}
//--------------------------------------------------------------------------------
void BufferDX11::UnMap()
{
}
//--------------------------------------------------------------------------------
ID3D11Resource*	BufferDX11::GetResource()
{
	return( m_pBuffer.Get() );
}
//--------------------------------------------------------------------------------
UINT BufferDX11::GetEvictionPriority()
{
	UINT priority = 0;

	if ( m_pBuffer )
		priority = m_pBuffer->GetEvictionPriority();

	return( priority );
}
//--------------------------------------------------------------------------------
void BufferDX11::SetEvictionPriority( UINT EvictionPriority )
{
	if ( m_pBuffer )
		m_pBuffer->SetEvictionPriority( EvictionPriority );
}
//--------------------------------------------------------------------------------