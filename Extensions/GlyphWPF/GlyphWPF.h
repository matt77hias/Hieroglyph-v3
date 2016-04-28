
#pragma once
#include <windows.h>
//--------------------------------------------------------------------------------
// This file is a portion of the Hieroglyph 3 Rendering Engine.  It is distributed
// under the MIT License, available in the root of this distribution and 
// at the following URL:
//
// http://www.opensource.org/licenses/mit-license.php
//
// Copyright (c) Jason Zink 
//--------------------------------------------------------------------------------
// This implementation of D3DImageEx is based off of the code written by
// Jeremiah Morill: http://jmorrill.hjtcentral.com/Home/tabid/428/EntryId/437/Direct3D-10-11-Direct2D-in-WPF.aspx
//--------------------------------------------------------------------------------
#include <stdlib.h>
#include <stdio.h>

#include <d3d9.h>
#include <d3d11.h>

using namespace System;
using namespace System::Windows::Interop;
using namespace System::Windows;

namespace System { namespace Windows { namespace Interop
{
	public ref class D3DImageEx : D3DImage
	{
		static D3DImageEx();

	public:
		void SetBackBufferEx(IntPtr pResource);

	private:
		// The references to D3D9 and its device
		static IDirect3D9Ex*               m_D3D9;
		static IDirect3DDevice9Ex*         m_D3D9Device;
		
	private:
		static D3DFORMAT ConvertDXGIToD3D9Format(DXGI_FORMAT format);
		static HRESULT GetSharedSurface(HANDLE hSharedHandle, void** ppUnknown, UINT width, UINT height, DXGI_FORMAT format);
		static HRESULT GetSharedHandle(IUnknown *pUnknown, HANDLE * pHandle);
	};
}}}
