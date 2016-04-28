//--------------------------------------------------------------------------------
// This file is a portion of the Hieroglyph 3 Rendering Engine.  It is distributed
// under the MIT License, available in the root of this distribution and 
// at the following URL:
//
// http://www.opensource.org/licenses/mit-license.php
//
// Copyright (c) Jason Zink 
//--------------------------------------------------------------------------------

#include "GlyphWPF.h"

namespace System { namespace Windows { namespace Interop
{
	static D3DImageEx::D3DImageEx()
	{
		HRESULT hr;
		HWND hWnd = GetDesktopWindow();

		IDirect3D9Ex* pD3D9;
		Direct3DCreate9Ex( D3D_SDK_VERSION, &pD3D9 );

		m_D3D9 = pD3D9;

		if (!m_D3D9)
			throw gcnew Exception("Could not acquire D3D9 interface.");

		m_D3D9 = pD3D9;

		D3DPRESENT_PARAMETERS		d3dpp;
		ZeroMemory(&d3dpp, sizeof(d3dpp));
		d3dpp.Windowed				= TRUE;
		d3dpp.SwapEffect			= D3DSWAPEFFECT_DISCARD;
		d3dpp.hDeviceWindow			= hWnd;
		d3dpp.PresentationInterval	= D3DPRESENT_INTERVAL_IMMEDIATE;

		IDirect3DDevice9Ex *d3D9Device;

		hr = m_D3D9->CreateDeviceEx(D3DADAPTER_DEFAULT,
									D3DDEVTYPE_HAL,
									hWnd,
									D3DCREATE_HARDWARE_VERTEXPROCESSING | D3DCREATE_MULTITHREADED | D3DCREATE_FPU_PRESERVE,
									&d3dpp,
									NULL,
									&d3D9Device);

		if (FAILED(hr))
		{
			m_D3D9->Release();
			m_D3D9 = nullptr;
			throw gcnew Exception("Could not acquire D3D9 device interface.");
		}

		m_D3D9Device = d3D9Device;
	}



	void D3DImageEx::SetBackBufferEx(IntPtr pResource)
	{
		/* Check if the user just wants to clear the D3DImage backbuffer */
		if(pResource == IntPtr::Zero)
		{
			SetBackBuffer(D3DResourceType::IDirect3DSurface9, IntPtr::Zero);
			return;
		}

		IUnknown *pUnk = (IUnknown*)pResource.ToPointer();
		IUnknown *pUnkResource = NULL;

		UINT width = 0; 
		UINT height = 0;
		DXGI_FORMAT format = DXGI_FORMAT_UNKNOWN;
		

		::ID3D11Texture2D *pTexture2D = NULL;

		/* Query for IUNKNOWN for ID3D11Texture2D */
		if(FAILED(pUnk->QueryInterface(__uuidof(::ID3D11Texture2D), (void**)&pTexture2D)))
			throw gcnew ArgumentException("Must pass a valid resource","pResource");

		/* Get the texture description.  This is needed to recreate
			* the surface in the 9Ex device */
		D3D11_TEXTURE2D_DESC textureDesc;
		pTexture2D->GetDesc(&textureDesc);

		width = textureDesc.Width;
		height = textureDesc.Height;
		format = textureDesc.Format;
					
		pUnkResource = pTexture2D;
		pTexture2D->Release();




		/* The shared handle of the D3D resource */
		HANDLE hSharedHandle = NULL;

		/* Shared texture pulled through the 9Ex device */
		IDirect3DTexture9* pTexture = NULL;
		
		/* Shared surface, pulled through the shared texture */
		IDirect3DSurface9* pSurface;
		
		/* Get the shared handle for the given resource */
		if(FAILED(GetSharedHandle(pUnkResource, &hSharedHandle)))
			throw gcnew Exception("Could not aquire shared resource handle");

		/* Get the shared surface.  In this case its really a texture =X */
		if(FAILED(GetSharedSurface(hSharedHandle, (void**)&pTexture, width, height, format)))
			throw gcnew Exception("Could not create shared resource");

		/* Get surface level 0, which we need for the D3DImage */
		if (FAILED(pTexture->GetSurfaceLevel(0, &pSurface)))
			throw gcnew Exception("Could not get surface level");

		/* Done with the texture */
		pTexture->Release();
		
		Lock();
		/* Set the backbuffer of the D3DImage */
		SetBackBuffer(D3DResourceType::IDirect3DSurface9, IntPtr(pSurface));
		Unlock();

		pSurface->Release();
	}

	D3DFORMAT D3DImageEx::ConvertDXGIToD3D9Format(DXGI_FORMAT format)
	{
		switch (format)
		{
			case DXGI_FORMAT_B8G8R8A8_UNORM:
				return D3DFMT_A8R8G8B8;
			case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB: 
				return D3DFMT_A8R8G8B8;
			case DXGI_FORMAT_B8G8R8X8_UNORM:
				return D3DFMT_X8R8G8B8;
			case DXGI_FORMAT_R8G8B8A8_UNORM: 
				return D3DFMT_A8B8G8R8;
			case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB: 
				return D3DFMT_A8B8G8R8;
			default:
				return D3DFMT_UNKNOWN;
		};
	}

	HRESULT D3DImageEx::GetSharedSurface(HANDLE hSharedHandle, void** ppUnknown, UINT width, UINT height, DXGI_FORMAT format)
	{
		D3DFORMAT D3D9Format;

		/* Convert the DXGI format to a D3D9 format */
		if ((D3D9Format = ConvertDXGIToD3D9Format(format)) == D3DFMT_UNKNOWN)
			/* We're boned =X */
			return E_INVALIDARG;

		HRESULT hr = S_OK;
		IDirect3DTexture9** ppTexture = (IDirect3DTexture9**)ppUnknown;

		/* Create the texture locally, but provide the shared handle.
		 * This doesn't really create a new texture, but simply
		 * pulls the D3D10/11 resource in the 9Ex device */
		hr = m_D3D9Device->CreateTexture( width, height, 1,
										  D3DUSAGE_RENDERTARGET,
										  D3D9Format,
										  D3DPOOL_DEFAULT,
										  ppTexture,
										  &hSharedHandle );

		return hr;
	}

	HRESULT D3DImageEx::GetSharedHandle(IUnknown *pUnknown, HANDLE * pHandle)
	{
		HRESULT hr = S_OK;
		    
		*pHandle = NULL;
		IDXGIResource* pSurface;

		if (FAILED(hr = pUnknown->QueryInterface(__uuidof(IDXGIResource), (void**)&pSurface)))
			return hr;

		hr = pSurface->GetSharedHandle(pHandle);
		pSurface->Release();

		return hr;
	}


}}}