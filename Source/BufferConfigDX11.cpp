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
#include "BufferConfigDX11.h"
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
BufferConfigDX11::BufferConfigDX11()
{
	SetDefaults();
}
//--------------------------------------------------------------------------------
BufferConfigDX11::~BufferConfigDX11()
{
}
//--------------------------------------------------------------------------------
void BufferConfigDX11::SetDefaults()
{
	// Set the state to the default configuration.  These are the D3D11 default
	// values as well.

	m_State.ByteWidth = 1;
    m_State.Usage = D3D11_USAGE_DEFAULT;
    m_State.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    m_State.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    m_State.MiscFlags = 0;
    m_State.StructureByteStride = 0;
}
//--------------------------------------------------------------------------------
void BufferConfigDX11::SetDefaultConstantBuffer( UINT size, bool dynamic )
{
	// Create the settings for a constant buffer.  This includes setting the 
	// constant buffer flag, allowing the CPU write access, and a dynamic usage.
	// Additional flags may be set as needed.

	m_State.ByteWidth = size;
    m_State.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    m_State.MiscFlags = 0;
    m_State.StructureByteStride = 0;

	if ( dynamic )
	{
		m_State.Usage = D3D11_USAGE_DYNAMIC;
		m_State.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	}
	else
	{
		m_State.Usage = D3D11_USAGE_IMMUTABLE;
		m_State.CPUAccessFlags = 0;
	}
}
//--------------------------------------------------------------------------------
void BufferConfigDX11::SetDefaultVertexBuffer( UINT size, bool dynamic )
{
	// Create the settings for a vertex buffer.  This includes the setting the
	// vertex buffer flag, 

	m_State.ByteWidth = size;
    m_State.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    m_State.MiscFlags = 0;
    m_State.StructureByteStride = 0;

	if ( dynamic )
	{
		m_State.Usage = D3D11_USAGE_DYNAMIC;
		m_State.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	}
	else
	{
		m_State.Usage = D3D11_USAGE_IMMUTABLE;
		m_State.CPUAccessFlags = 0;
	}
}
//--------------------------------------------------------------------------------
void BufferConfigDX11::SetDefaultIndexBuffer( UINT size, bool dynamic )
{
	m_State.ByteWidth = size;
    m_State.BindFlags = D3D11_BIND_INDEX_BUFFER;
    m_State.MiscFlags = 0;
    m_State.StructureByteStride = 0;

	if ( dynamic )
	{
		m_State.Usage = D3D11_USAGE_DYNAMIC;
		m_State.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	}
	else
	{
		m_State.Usage = D3D11_USAGE_IMMUTABLE;
		m_State.CPUAccessFlags = 0;
	}
}
//--------------------------------------------------------------------------------
void BufferConfigDX11::SetDefaultStructuredBuffer( UINT size, UINT structsize )
{
	m_State.ByteWidth = size * structsize;
    m_State.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
    m_State.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
    m_State.StructureByteStride = structsize;

	m_State.Usage = D3D11_USAGE_DEFAULT;
	m_State.CPUAccessFlags = 0;
}
//--------------------------------------------------------------------------------
void BufferConfigDX11::SetDefaultByteAddressBuffer( UINT size )
{
	// Set the state to the default configuration.  These are the D3D11 default
	// values as well.

	// TODO: These parameters need to be updated to reflect the byte address buffer!
	m_State.ByteWidth = size;
    m_State.Usage = D3D11_USAGE_DEFAULT;
    m_State.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
    m_State.CPUAccessFlags = 0;
    m_State.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS;
    m_State.StructureByteStride = 0;
}
//--------------------------------------------------------------------------------
void BufferConfigDX11::SetDefaultIndirectArgsBuffer( UINT size )
{
	// Set the state to the default configuration.  These are the D3D11 default
	// values as well.

	m_State.ByteWidth = size;
    m_State.Usage = D3D11_USAGE_DEFAULT;
    m_State.BindFlags = 0;
    m_State.CPUAccessFlags = 0;
    m_State.MiscFlags = D3D11_RESOURCE_MISC_DRAWINDIRECT_ARGS;
    m_State.StructureByteStride = 0;
}
//--------------------------------------------------------------------------------
void BufferConfigDX11::SetDefaultStagingBuffer( UINT size )
{
	// Set the state to the default configuration.  These are the D3D11 default
	// values as well.

	m_State.ByteWidth = size;
    m_State.Usage = D3D11_USAGE_STAGING;
    m_State.BindFlags = 0;
    m_State.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
    m_State.MiscFlags = 0;
    m_State.StructureByteStride = 0;
}
//--------------------------------------------------------------------------------
void BufferConfigDX11::SetByteWidth( UINT state )
{
	m_State.ByteWidth = state;
}
//--------------------------------------------------------------------------------
void BufferConfigDX11::SetUsage( D3D11_USAGE state )
{
	m_State.Usage = state;
}
//--------------------------------------------------------------------------------
void BufferConfigDX11::SetBindFlags( UINT state )
{
	m_State.BindFlags = state;
}
//--------------------------------------------------------------------------------
void BufferConfigDX11::SetCPUAccessFlags( UINT state )
{
	m_State.CPUAccessFlags = state;
}
//--------------------------------------------------------------------------------
void BufferConfigDX11::SetMiscFlags( UINT state )
{
	m_State.MiscFlags = state;
}
//--------------------------------------------------------------------------------
void BufferConfigDX11::SetStructureByteStride( UINT state )
{
	m_State.StructureByteStride = state;
}
//--------------------------------------------------------------------------------
D3D11_BUFFER_DESC BufferConfigDX11::GetBufferDesc()
{
	return( m_State );
}
//--------------------------------------------------------------------------------
