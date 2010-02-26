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
#include "ResourceProxyDX11.h"
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
ResourceProxyDX11::ResourceProxyDX11( int ResourceID, BufferConfigDX11* pConfig, RendererDX11* pRenderer )
{
	// Initialize all indices and pointers to a neutral state.

	m_iResource = m_iResourceSRV = m_iResourceRTV = m_iResourceDSV = m_iResourceUAV = -1;
	m_pBufferConfig = 0;
	m_pTexture1dConfig = 0;
	m_pTexture2dConfig = 0;
	m_pTexture3dConfig = 0;


	// Retain the renderer's resource ID and configuration.  
	
	m_iResource = ResourceID;
	m_pBufferConfig = new BufferConfigDX11();
	*m_pBufferConfig = *pConfig;


	// Depending on the bind flags used to create the resource, we create a set
	// of default views to be used.  These can be modified later on if a special
	// view type is needed, but in most cases these will work fine.  This makes
	// using the resources easier, while still allowing fancy uses if needed.

	D3D11_BUFFER_DESC desc = pConfig->GetBufferDesc();

	if ( ( desc.BindFlags & D3D11_BIND_SHADER_RESOURCE ) == D3D11_BIND_SHADER_RESOURCE )
		m_iResourceSRV = pRenderer->CreateShaderResourceView( m_iResource, 0 );

	if ( ( desc.BindFlags & D3D11_BIND_RENDER_TARGET ) == D3D11_BIND_RENDER_TARGET )
		m_iResourceRTV = pRenderer->CreateRenderTargetView( m_iResource, 0 );

	if ( ( desc.BindFlags & D3D11_BIND_DEPTH_STENCIL ) == D3D11_BIND_DEPTH_STENCIL )
		m_iResourceDSV = pRenderer->CreateDepthStencilView( m_iResource, 0 );

	if ( ( desc.BindFlags & D3D11_BIND_UNORDERED_ACCESS ) == D3D11_BIND_UNORDERED_ACCESS )
		m_iResourceUAV = pRenderer->CreateUnorderedAccessView( m_iResource, 0 );

}
//--------------------------------------------------------------------------------
ResourceProxyDX11::ResourceProxyDX11( int ResourceID, Texture1dConfigDX11* pConfig, RendererDX11* pRenderer )
{
	// Initialize all indices and pointers to a neutral state.

	m_iResource = m_iResourceSRV = m_iResourceRTV = m_iResourceDSV = m_iResourceUAV = -1;
	m_pBufferConfig = 0;
	m_pTexture1dConfig = 0;
	m_pTexture2dConfig = 0;
	m_pTexture3dConfig = 0;


	// Retain the renderer's resource ID and configuration.  
	
	m_iResource = ResourceID;
	m_pTexture1dConfig = new Texture1dConfigDX11();
	*m_pTexture1dConfig = *pConfig;


	// Depending on the bind flags used to create the resource, we create a set
	// of default views to be used.  These can be modified later on if a special
	// view type is needed, but in most cases these will work fine.  This makes
	// using the resources easier, while still allowing fancy uses if needed.

	D3D11_TEXTURE1D_DESC desc = pConfig->GetTextureDesc();

	if ( ( desc.BindFlags & D3D11_BIND_SHADER_RESOURCE ) == D3D11_BIND_SHADER_RESOURCE )
		m_iResourceSRV = pRenderer->CreateShaderResourceView( m_iResource, 0 );

	if ( ( desc.BindFlags & D3D11_BIND_RENDER_TARGET ) == D3D11_BIND_RENDER_TARGET )
		m_iResourceRTV = pRenderer->CreateRenderTargetView( m_iResource, 0 );

	if ( ( desc.BindFlags & D3D11_BIND_DEPTH_STENCIL ) == D3D11_BIND_DEPTH_STENCIL )
		m_iResourceDSV = pRenderer->CreateDepthStencilView( m_iResource, 0 );

	if ( ( desc.BindFlags & D3D11_BIND_UNORDERED_ACCESS ) == D3D11_BIND_UNORDERED_ACCESS )
		m_iResourceUAV = pRenderer->CreateUnorderedAccessView( m_iResource, 0 );

}
//--------------------------------------------------------------------------------
ResourceProxyDX11::ResourceProxyDX11( int ResourceID, Texture2dConfigDX11* pConfig, RendererDX11* pRenderer )
{
	// Initialize all indices and pointers to a neutral state.

	m_iResource = m_iResourceSRV = m_iResourceRTV = m_iResourceDSV = m_iResourceUAV = -1;
	m_pBufferConfig = 0;
	m_pTexture1dConfig = 0;
	m_pTexture2dConfig = 0;
	m_pTexture3dConfig = 0;


	// Retain the renderer's resource ID and configuration.  
	
	m_iResource = ResourceID;
	m_pTexture2dConfig = new Texture2dConfigDX11();
	*m_pTexture2dConfig = *pConfig;


	// Depending on the bind flags used to create the resource, we create a set
	// of default views to be used.  These can be modified later on if a special
	// view type is needed, but in most cases these will work fine.  This makes
	// using the resources easier, while still allowing fancy uses if needed.

	D3D11_TEXTURE2D_DESC desc = pConfig->GetTextureDesc();

	if ( ( desc.BindFlags & D3D11_BIND_SHADER_RESOURCE ) == D3D11_BIND_SHADER_RESOURCE )
		m_iResourceSRV = pRenderer->CreateShaderResourceView( m_iResource, 0 );

	if ( ( desc.BindFlags & D3D11_BIND_RENDER_TARGET ) == D3D11_BIND_RENDER_TARGET )
		m_iResourceRTV = pRenderer->CreateRenderTargetView( m_iResource, 0 );

	if ( ( desc.BindFlags & D3D11_BIND_DEPTH_STENCIL ) == D3D11_BIND_DEPTH_STENCIL )
		m_iResourceDSV = pRenderer->CreateDepthStencilView( m_iResource, 0 );

	if ( ( desc.BindFlags & D3D11_BIND_UNORDERED_ACCESS ) == D3D11_BIND_UNORDERED_ACCESS )
		m_iResourceUAV = pRenderer->CreateUnorderedAccessView( m_iResource, 0 );

}
//--------------------------------------------------------------------------------
ResourceProxyDX11::ResourceProxyDX11( int ResourceID, Texture3dConfigDX11* pConfig, RendererDX11* pRenderer )
{
	// Initialize all indices and pointers to a neutral state.

	m_iResource = m_iResourceSRV = m_iResourceRTV = m_iResourceDSV = m_iResourceUAV = -1;
	m_pBufferConfig = 0;
	m_pTexture1dConfig = 0;
	m_pTexture2dConfig = 0;
	m_pTexture3dConfig = 0;


	// Retain the renderer's resource ID and configuration.  
	
	m_iResource = ResourceID;
	m_pTexture3dConfig = new Texture3dConfigDX11();
	*m_pTexture3dConfig = *pConfig;


	// Depending on the bind flags used to create the resource, we create a set
	// of default views to be used.  These can be modified later on if a special
	// view type is needed, but in most cases these will work fine.  This makes
	// using the resources easier, while still allowing fancy uses if needed.

	D3D11_TEXTURE3D_DESC desc = pConfig->GetTextureDesc();

	if ( ( desc.BindFlags & D3D11_BIND_SHADER_RESOURCE ) == D3D11_BIND_SHADER_RESOURCE )
		m_iResourceSRV = pRenderer->CreateShaderResourceView( m_iResource, 0 );

	if ( ( desc.BindFlags & D3D11_BIND_RENDER_TARGET ) == D3D11_BIND_RENDER_TARGET )
		m_iResourceRTV = pRenderer->CreateRenderTargetView( m_iResource, 0 );

	if ( ( desc.BindFlags & D3D11_BIND_DEPTH_STENCIL ) == D3D11_BIND_DEPTH_STENCIL )
		m_iResourceDSV = pRenderer->CreateDepthStencilView( m_iResource, 0 );

	if ( ( desc.BindFlags & D3D11_BIND_UNORDERED_ACCESS ) == D3D11_BIND_UNORDERED_ACCESS )
		m_iResourceUAV = pRenderer->CreateUnorderedAccessView( m_iResource, 0 );

}
//--------------------------------------------------------------------------------
ResourceProxyDX11::ResourceProxyDX11()
{
	// Initialize all indices and pointers to a neutral state.

	m_iResource = m_iResourceSRV = m_iResourceRTV = m_iResourceDSV = m_iResourceUAV = -1;
	m_pBufferConfig = 0;
	m_pTexture1dConfig = 0;
	m_pTexture2dConfig = 0;
	m_pTexture3dConfig = 0;
}
//--------------------------------------------------------------------------------
ResourceProxyDX11::~ResourceProxyDX11()
{
	SAFE_DELETE( m_pBufferConfig );
	SAFE_DELETE( m_pTexture1dConfig );
	SAFE_DELETE( m_pTexture2dConfig );
	SAFE_DELETE( m_pTexture3dConfig );
}
//--------------------------------------------------------------------------------
