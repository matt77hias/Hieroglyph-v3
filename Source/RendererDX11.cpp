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
#include "RendererDX11.h"

#include "Log.h"

#include "VertexBufferDX11.h"
#include "IndexBufferDX11.h"
#include "ConstantBufferDX11.h"
#include "StructuredBufferDX11.h"
#include "ByteAddressBufferDX11.h"
#include "IndirectArgsBufferDX11.h"
#include "Texture1dDX11.h"
#include "Texture2dDX11.h"
#include "Texture3dDX11.h"
#include "ResourceDX11.h"
#include "SwapChainDX11.h"
#include "ViewPortDX11.h"

#include "BufferConfigDX11.h"
#include "Texture1dConfigDX11.h"
#include "Texture2dConfigDX11.h"
#include "Texture3dConfigDX11.h"
#include "SwapChainConfigDX11.h"

#include "ShaderResourceViewDX11.h"
#include "RenderTargetViewDX11.h"
#include "DepthStencilViewDX11.h"
#include "UnorderedAccessViewDX11.h"

#include "BlendStateConfigDX11.h"
#include "DepthStencilStateConfigDX11.h"
#include "RasterizerStateConfigDX11.h"

#include "VertexShaderDX11.h"
#include "HullShaderDX11.h"
#include "DomainShaderDX11.h"
#include "GeometryShaderDX11.h"
#include "PixelShaderDX11.h"
#include "ComputeShaderDX11.h"
#include "ShaderFactoryDX11.h"
#include "ShaderReflectionDX11.h"
#include "ShaderReflectionFactoryDX11.h"

#include "VectorParameterDX11.h"
#include "MatrixParameterDX11.h"
#include "MatrixArrayParameterDX11.h"
#include "ShaderResourceParameterDX11.h"
#include "UnorderedAccessParameterDX11.h"
#include "ConstantBufferParameterDX11.h"
#include "SamplerParameterDX11.h"

#include "RenderEffectDX11.h"
#include "GeometryDX11.h"
#include "CommandListDX11.h"

#include "DXGIAdapter.h"
#include "DXGIOutput.h"

#include "ParameterManagerDX11.h"
#include "PipelineManagerDX11.h"

#include "Task.h"

#include "EventManager.h"
#include "EvtErrorMessage.h"

#include "FileSystem.h"
#include "Process.h"

#include "D3DEnumConversion.h"

#include "WICTextureLoader.h"
#include "DDSTextureLoader.h"

// Library imports
#pragma comment( lib, "d3d11.lib" )
#pragma comment( lib, "DXGI.lib" )

using Microsoft::WRL::ComPtr;

//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
RendererDX11* RendererDX11::m_spRenderer = 0;
//--------------------------------------------------------------------------------
RendererDX11::RendererDX11()
{
	if ( m_spRenderer == 0 )
		m_spRenderer = this;

//	m_pDevice = 0;
//	m_pDebugger = 0;
	m_driverType = D3D_DRIVER_TYPE_NULL;


	m_pParamMgr = 0;
	pImmPipeline = 0;

	// Initialize this to always use MT!
	MultiThreadingConfig.SetConfiguration( true );
	MultiThreadingConfig.ApplyConfiguration();

	m_FeatureLevel = D3D_FEATURE_LEVEL_9_1; // Initialize this to only support 9.1...
}
//--------------------------------------------------------------------------------
RendererDX11::~RendererDX11()
{
}
//--------------------------------------------------------------------------------
RendererDX11* RendererDX11::Get()
{
	return( m_spRenderer );
}
//--------------------------------------------------------------------------------
D3D_FEATURE_LEVEL RendererDX11::GetAvailableFeatureLevel( D3D_DRIVER_TYPE DriverType )
{
	D3D_FEATURE_LEVEL FeatureLevel;
	HRESULT hr;

	// If the device has already been created, simply return the feature level.
	// Otherwise perform a test with null inputs to get the returned feature level
	// without creating the device.  The application can then do whatever it needs
	// to for a given feature level.

	if ( m_pDevice ) {
		FeatureLevel = m_pDevice->GetFeatureLevel();
	} else {
		hr = D3D11CreateDevice(
			nullptr,
			DriverType,
			nullptr,
			0,
			nullptr,
			0,
			D3D11_SDK_VERSION,
			nullptr,
			&FeatureLevel,
			nullptr );

		if ( FAILED( hr ) ) {
			Log::Get().Write( L"Failed to determine the available hardware feature level!" );
		}

	}

	return( FeatureLevel );
}
//--------------------------------------------------------------------------------
D3D_FEATURE_LEVEL RendererDX11::GetCurrentFeatureLevel()
{
	return( m_FeatureLevel );
}
//--------------------------------------------------------------------------------
UINT64 RendererDX11::GetAvailableVideoMemory()
{
    // Acquire the DXGI device, then the adapter.
	// TODO: This method needs to be capable of checking on multiple adapters!

    ComPtr<IDXGIDevice> pDXGIDevice;
    ComPtr<IDXGIAdapter> pDXGIAdapter;
    
	HRESULT hr = m_pDevice.CopyTo( pDXGIDevice.GetAddressOf() );
	pDXGIDevice->GetAdapter( pDXGIAdapter.GetAddressOf() );
    
	// Use the adapter interface to get its description.  Then grab the available
	// video memory based on if there is dedicated or shared memory for the GPU.

    DXGI_ADAPTER_DESC AdapterDesc;
    pDXGIAdapter->GetDesc( &AdapterDesc );

	UINT64 availableVideoMem = 0;

    if ( AdapterDesc.DedicatedVideoMemory )
        availableVideoMem = AdapterDesc.DedicatedVideoMemory;
    else
        availableVideoMem = AdapterDesc.SharedSystemMemory;

    return( availableVideoMem );
}
//--------------------------------------------------------------------------------
bool RendererDX11::Initialize( D3D_DRIVER_TYPE DriverType, D3D_FEATURE_LEVEL FeatureLevel )
{

    HRESULT hr = S_OK;

	// Create a factory to enumerate all of the hardware in the system.

	ComPtr<IDXGIFactory1> pFactory;
	hr = CreateDXGIFactory1( __uuidof(IDXGIFactory), reinterpret_cast<void**>( pFactory.GetAddressOf() ) );


	// Enumerate all of the adapters in the current system.  This includes all
	// adapters, even the ones that don't support the ID3D11Device interface.

	ComPtr<IDXGIAdapter1> pCurrentAdapter;
	std::vector<DXGIAdapter> vAdapters;

	while( pFactory->EnumAdapters1( static_cast<UINT>(vAdapters.size()), pCurrentAdapter.ReleaseAndGetAddressOf() ) != DXGI_ERROR_NOT_FOUND )
	{
		vAdapters.push_back( pCurrentAdapter );

		DXGI_ADAPTER_DESC1 desc;
		pCurrentAdapter->GetDesc1( &desc );

		Log::Get().Write( desc.Description );
	}

	// Specify debug
	UINT CreateDeviceFlags = 0;
#ifdef _DEBUG
    CreateDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	DeviceContextComPtr pContext;

	D3D_FEATURE_LEVEL level[] = { FeatureLevel };
	D3D_FEATURE_LEVEL CreatedLevel;

	// If we are trying to get a hardware device, then loop through all available
	// adapters until we successfully create it.  This is useful for multi-adapter
	// systems, where the built in GPU may or may not be capable of the most recent
	// feature levels.
	//
	// If it isn't a hardware device, then we simply use nullptr for the adapter with
	// the appropriate driver type.  See the following page for more details on device 
	// creation: http://msdn.microsoft.com/en-us/library/windows/desktop/ff476082%28v=vs.85%29.aspx

    if ( DriverType == D3D_DRIVER_TYPE_HARDWARE )
    {
        for ( auto pAdapter : vAdapters )
        {
            hr = D3D11CreateDevice(
				pAdapter.m_pAdapter.Get(),
                D3D_DRIVER_TYPE_UNKNOWN,
                nullptr,
                CreateDeviceFlags,
                level,
                1,
                D3D11_SDK_VERSION,
				m_pDevice.GetAddressOf(),
                &CreatedLevel,
				pContext.GetAddressOf() );

            if (hr == S_OK)
                break;
        }
    }
    else
    {
        hr = D3D11CreateDevice(
            nullptr,
            DriverType,
            nullptr,
            CreateDeviceFlags,
            level,
            1,
            D3D11_SDK_VERSION,
            m_pDevice.GetAddressOf(),
            &CreatedLevel,
			pContext.GetAddressOf() );
    }

	if( FAILED( hr ) )
        return false;

	// Get the debugger interface from the device.

	hr = m_pDevice.CopyTo( m_pDebugger.GetAddressOf() );

	if ( FAILED( hr ) )
	{
		Log::Get().Write( L"Unable to acquire the ID3D11Debug interface from the device!" );
	}

	// Grab a copy of the feature level for use by the rest of the rendering system.

	m_FeatureLevel = m_pDevice->GetFeatureLevel();

	// Create the renderer components here, including the parameter manager, 
	// pipeline manager, and resource manager.

	m_pParamMgr = new ParameterManagerDX11( 0 );
	pImmPipeline = new PipelineManagerDX11();
	pImmPipeline->SetDeviceContext( pContext, m_FeatureLevel );

	// Rasterizer State (RS) - the first state will be index zero, so no need
	// to keep a copy of it here.

	RasterizerStateConfigDX11 RasterizerState;
	pImmPipeline->RasterizerStage.DesiredState.RasterizerState.SetState( CreateRasterizerState( &RasterizerState ) );

	// Depth Stencil State (DS) - the first state will be index zero, so no need
	// to keep a copy of it here.

	DepthStencilStateConfigDX11 DepthStencilState;
	pImmPipeline->OutputMergerStage.DesiredState.DepthStencilState.SetState( CreateDepthStencilState( &DepthStencilState ) );

	// Output Merger State (OM) - the first state will be index zero, so no need
	// to keep a copy of it here.

	BlendStateConfigDX11 BlendState;
	pImmPipeline->OutputMergerStage.DesiredState.BlendState.SetState( CreateBlendState( &BlendState ) );


	// Create the default resource views for each category.  This has the effect
	// of allowing the '0' index to be the default state.

	m_vShaderResourceViews.emplace_back( ShaderResourceViewComPtr() );
	m_vUnorderedAccessViews.emplace_back( UnorderedAccessViewComPtr() );
	m_vRenderTargetViews.emplace_back( RenderTargetViewComPtr() );
	m_vDepthStencilViews.emplace_back( DepthStencilViewComPtr() );


	// Create a query object to be used to gather statistics on the pipeline.

	D3D11_QUERY_DESC queryDesc;
	queryDesc.Query = D3D11_QUERY_PIPELINE_STATISTICS;
	queryDesc.MiscFlags = 0;

    for( int i = 0; i < PipelineManagerDX11::NumQueries; ++i)
    {
	    hr = m_pDevice->CreateQuery( &queryDesc, &pImmPipeline->m_Queries[i] );

	    if ( FAILED( hr ) )
	    {
		    Log::Get().Write( L"Unable to create a query object!" );
		    Shutdown();
		    return( false );
	    }
    }

    D3D11_FEATURE_DATA_D3D10_X_HARDWARE_OPTIONS Options;
    m_pDevice->CheckFeatureSupport(D3D11_FEATURE_D3D10_X_HARDWARE_OPTIONS, &Options, sizeof(Options));
	if ( Options.ComputeShaders_Plus_RawAndStructuredBuffers_Via_Shader_4_x )
		Log::Get().Write( L"Device supports compute shaders plus raw and structured buffers via shader 4.x" );


	D3D11_FEATURE_DATA_THREADING ThreadingOptions;
	m_pDevice->CheckFeatureSupport( D3D11_FEATURE_THREADING, &ThreadingOptions, sizeof( ThreadingOptions ) );

	// TODO: Enumerate all of the formats and quality levels available for the given format.
	//       It may be beneficial to allow this query from the user instead of enumerating
	//       all possible formats...
	UINT NumQuality;
	HRESULT hr1 = m_pDevice->CheckMultisampleQualityLevels( DXGI_FORMAT_R8G8B8A8_UNORM, 4, &NumQuality );


	// Initialize the multithreading portion of the renderer.  This includes
	// creating the threads themselves, initializing the thread payloads as well.

	for ( int i = 0; i < NUM_THREADS; i++ )
	{
		// Mark each payload so that the thread knows which synchronization primitives to use.
		g_aPayload[i].id = i;

		// Create a deferred context for each thread's pipeline.
		DeviceContextComPtr pDeferred;
		m_pDevice->CreateDeferredContext( 0, pDeferred.GetAddressOf() );

		// Create the pipeline and set the context.
		g_aPayload[i].pPipeline = new PipelineManagerDX11();
		g_aPayload[i].pPipeline->SetDeviceContext( pDeferred, m_FeatureLevel );
		g_aPayload[i].pPipeline->RasterizerStage.DesiredState.RasterizerState.SetState( 0 );
		g_aPayload[i].pPipeline->OutputMergerStage.DesiredState.DepthStencilState.SetState( 0 );
		g_aPayload[i].pPipeline->OutputMergerStage.DesiredState.BlendState.SetState( 0 );


		// Create the command list.
		g_aPayload[i].pList = new CommandListDX11();
		
		// Generate a new parameter manager for each thread.
		g_aPayload[i].pParamManager = new ParameterManagerDX11( i+1 );
		g_aPayload[i].pParamManager->AttachParent( m_pParamMgr );

		// Initialize the payload data variables.
		g_aPayload[i].bComplete = true;
		g_aPayload[i].pTask = nullptr;

		// Create the threads in a suspended state.
		g_aThreadHandles[i] = 0;
		g_aThreadHandles[i] = (HANDLE)_beginthreadex( 0, 0xfffff, _TaskThreadProc, &g_aPayload[i], CREATE_SUSPENDED, 0 );

		// Create the synchronization events.
		g_aBeginEventHandle[i] = CreateEvent( 0, FALSE, FALSE, 0 );
		g_aEndEventHandle[i] = CreateEvent( 0, FALSE, FALSE, 0 );

		// Start the thread up now that it has a synch object to use.
		ResumeThread( g_aThreadHandles[i] );
	}

	return( true );
}
//--------------------------------------------------------------------------------
void RendererDX11::Shutdown()
{
	// Print some details about the renderer's status at shutdown.
	LogObjectPtrVector<ShaderDX11*>( m_vShaders );

	// Shutdown all of the threads
	for ( int i = 0; i < NUM_THREADS; i++ )
	{
		g_aPayload[i].bComplete = true;
		g_aPayload[i].pTask = nullptr;
		
		SAFE_DELETE( g_aPayload[i].pParamManager );
		SAFE_DELETE( g_aPayload[i].pPipeline );
		SAFE_DELETE( g_aPayload[i].pList );

		CloseHandle( g_aThreadHandles[i] );
		CloseHandle( g_aBeginEventHandle[i] );
		CloseHandle( g_aEndEventHandle[i] );
	}


	SAFE_DELETE( m_pParamMgr );
	SAFE_DELETE( pImmPipeline );

	// Since these are all managed with smart pointers, we just empty the
	// container and the objects will automatically be deleted.

	m_vBlendStates.clear();
	m_vDepthStencilStates.clear();
	m_vRasterizerStates.clear();
	m_vSamplerStates.clear();
	m_vInputLayouts.clear();
	m_vViewPorts.clear();

	for ( auto pShader : m_vShaders )
		delete pShader;

	m_vShaderResourceViews.clear();
	m_vRenderTargetViews.clear();
	m_vDepthStencilViews.clear();
	m_vUnorderedAccessViews.clear();

	for ( auto pResource : m_vResources )
		delete pResource;

	for ( auto pSwapChain : m_vSwapChains ) {
		if ( pSwapChain->m_pSwapChain != nullptr ) {
			pSwapChain->m_pSwapChain->SetFullscreenState( false, NULL );
		}
		delete pSwapChain;
	}

	// Clear the context and the device

	//SAFE_RELEASE( m_pDevice );
	//SAFE_RELEASE( m_pDebugger );
}
//--------------------------------------------------------------------------------
void RendererDX11::Present(HWND hWnd, int SwapChain, UINT SyncInterval, UINT PresentFlags )
{
	// Present to the window

	unsigned int index = static_cast<unsigned int>( SwapChain );

	if ( index < m_vSwapChains.size() ) {
		SwapChainDX11* pSwapChain = m_vSwapChains[SwapChain];
		HRESULT hr = pSwapChain->m_pSwapChain->Present( SyncInterval, PresentFlags );
	}
	else {
		Log::Get().Write( L"Tried to present an invalid swap chain index!" );
	}
}
//--------------------------------------------------------------------------------
int RendererDX11::CreateSwapChain( SwapChainConfigDX11* pConfig )
{
	// Attempt to create the DXGI Factory.

	ComPtr<IDXGIDevice> pDXGIDevice;
	HRESULT hr = m_pDevice.CopyTo( pDXGIDevice.GetAddressOf() );

	ComPtr<IDXGIAdapter> pDXGIAdapter;
	hr = pDXGIDevice->GetParent(__uuidof(IDXGIAdapter), reinterpret_cast<void **>( pDXGIAdapter.GetAddressOf() ) );

	ComPtr<IDXGIFactory> pFactory;
	pDXGIAdapter->GetParent(__uuidof(IDXGIFactory), reinterpret_cast<void **>( pFactory.GetAddressOf() ) );


	// Attempt to create the swap chain.

	ComPtr<IDXGISwapChain> pSwapChain;
	hr = pFactory->CreateSwapChain( m_pDevice.Get(), &pConfig->m_State, pSwapChain.GetAddressOf() );


	// Release the factory regardless of pass or fail.

	if ( FAILED( hr ) )
	{
		Log::Get().Write( L"Failed to create swap chain!" );
		return( -1 );
	}


	// Acquire the texture interface from the swap chain.

	Texture2DComPtr pSwapChainBuffer;
	hr = pSwapChain->GetBuffer( 0, __uuidof( ID3D11Texture2D ), reinterpret_cast< void** >( pSwapChainBuffer.GetAddressOf() ) );

	if ( FAILED( hr ) )
	{
		Log::Get().Write( L"Failed to get swap chain texture resource!" );
		return( -1 );
	}


	// Add the swap chain's back buffer texture and render target views to the internal data
	// structures to allow setting them later on.

	int ResourceID = StoreNewResource( new Texture2dDX11( pSwapChainBuffer ) );
	

	// If we get here, then we succeeded in creating our swap chain and it's constituent parts.
	// Now we create the wrapper object and store the result in our container.

	Texture2dConfigDX11 TextureConfig;
	pSwapChainBuffer->GetDesc( &TextureConfig.m_State );

	ResourcePtr Proxy( new ResourceProxyDX11( ResourceID, &TextureConfig, this ) );
	// With the resource proxy created, create the swap chain wrapper and store it.
	// The resource proxy can then be used later on by the application to get the
	// RTV or texture ID if needed.

	m_vSwapChains.push_back( new SwapChainDX11( pSwapChain, Proxy ) );

	return( m_vSwapChains.size() - 1 );
}
//--------------------------------------------------------------------------------
ResourcePtr RendererDX11::CreateVertexBuffer( BufferConfigDX11* pConfig,  D3D11_SUBRESOURCE_DATA* pData )
{
	// Create the buffer with the specified configuration.
	BufferComPtr pBuffer;
	HRESULT hr = m_pDevice->CreateBuffer( &pConfig->m_State, pData, pBuffer.GetAddressOf() );

	if ( pBuffer )
	{
		VertexBufferDX11* pVertexBuffer = new VertexBufferDX11( pBuffer );
		pVertexBuffer->SetDesiredDescription( pConfig->m_State );

		// Return an index with the lower 16 bits of index, and the upper
		// 16 bits to identify the resource type.

		int ResourceID = StoreNewResource( pVertexBuffer );
		ResourcePtr Proxy( new ResourceProxyDX11( ResourceID, pConfig, this ) );

		return( Proxy );
	}

	return( ResourcePtr( new ResourceProxyDX11() ) );
}
//--------------------------------------------------------------------------------
ResourcePtr RendererDX11::CreateIndexBuffer( BufferConfigDX11* pConfig,  D3D11_SUBRESOURCE_DATA* pData )
{
	// Create the buffer with the specified configuration.

	BufferComPtr pBuffer;
	HRESULT hr = m_pDevice->CreateBuffer( &pConfig->m_State, pData, pBuffer.GetAddressOf() );

	if ( pBuffer )
	{
		IndexBufferDX11* pIndexBuffer = new IndexBufferDX11( pBuffer );
		pIndexBuffer->SetDesiredDescription( pConfig->m_State );

		// Return an index with the lower 16 bits of index, and the upper
		// 16 bits to identify the resource type.

		int ResourceID = StoreNewResource( pIndexBuffer );
		ResourcePtr Proxy( new ResourceProxyDX11( ResourceID, pConfig, this ) );

		return( Proxy );
	}

	return( ResourcePtr( new ResourceProxyDX11() ) );
}
//--------------------------------------------------------------------------------
ResourcePtr RendererDX11::CreateStructuredBuffer( BufferConfigDX11* pConfig,  D3D11_SUBRESOURCE_DATA* pData )
{
	// Create the buffer with the specified configuration.

	BufferComPtr pBuffer;
	HRESULT hr = m_pDevice->CreateBuffer( &pConfig->m_State, pData, pBuffer.GetAddressOf() );

	if ( pBuffer )
	{
		StructuredBufferDX11* pStructuredBuffer = new StructuredBufferDX11( pBuffer );
		pStructuredBuffer->SetDesiredDescription( pConfig->m_State );

		// Return an index with the lower 16 bits of index, and the upper
		// 16 bits to identify the resource type.

		int ResourceID = StoreNewResource( pStructuredBuffer );
		ResourcePtr Proxy( new ResourceProxyDX11( ResourceID, pConfig, this ) );

		return( Proxy );
	}

	return( ResourcePtr( new ResourceProxyDX11() ) );
}
//--------------------------------------------------------------------------------
ResourcePtr RendererDX11::CreateByteAddressBuffer( BufferConfigDX11* pConfig,  D3D11_SUBRESOURCE_DATA* pData )
{
	// Create the buffer with the specified configuration.

	BufferComPtr pBuffer;
	HRESULT hr = m_pDevice->CreateBuffer( &pConfig->m_State, pData, pBuffer.GetAddressOf() );

	if ( pBuffer )
	{
		ByteAddressBufferDX11* pByteAddressBuffer = new ByteAddressBufferDX11( pBuffer );
		pByteAddressBuffer->SetDesiredDescription( pConfig->m_State );
		
		// Return an index with the lower 16 bits of index, and the upper
		// 16 bits to identify the resource type.

		int ResourceID = StoreNewResource( pByteAddressBuffer );
		ResourcePtr Proxy( new ResourceProxyDX11( ResourceID, pConfig, this ) );

		return( Proxy );
	}

	return( ResourcePtr( new ResourceProxyDX11() ) );
}
//--------------------------------------------------------------------------------
ResourcePtr RendererDX11::CreateIndirectArgsBuffer( BufferConfigDX11* pConfig,  D3D11_SUBRESOURCE_DATA* pData )
{
	// Create the buffer with the specified configuration.

	BufferComPtr pBuffer;
	HRESULT hr = m_pDevice->CreateBuffer( &pConfig->m_State, pData, pBuffer.GetAddressOf() );

	if ( pBuffer )
	{
		IndirectArgsBufferDX11* pIndirectArgsBuffer = new IndirectArgsBufferDX11( pBuffer );
		pIndirectArgsBuffer->SetDesiredDescription( pConfig->m_State );

		// Return an index with the lower 16 bits of index, and the upper
		// 16 bits to identify the resource type.

		int ResourceID = StoreNewResource( pIndirectArgsBuffer );
		ResourcePtr Proxy( new ResourceProxyDX11( ResourceID, pConfig, this ) );

		return( Proxy );
	}

	return( ResourcePtr( new ResourceProxyDX11() ) );
}
//--------------------------------------------------------------------------------
ResourcePtr RendererDX11::CreateConstantBuffer( BufferConfigDX11* pConfig,  D3D11_SUBRESOURCE_DATA* pData, bool bAutoUpdate )
{
	// Set the constant buffer flag in addition to any other flags that
	// the user has set.

	BufferComPtr pBuffer;
	HRESULT hr = m_pDevice->CreateBuffer( &pConfig->m_State, pData, pBuffer.GetAddressOf() );

	if ( pBuffer )
	{
		ConstantBufferDX11* pConstantBuffer = new ConstantBufferDX11( pBuffer );
		pConstantBuffer->SetDesiredDescription( pConfig->m_State );
		pConstantBuffer->SetAutoUpdate( bAutoUpdate );

		// Return an index with the lower 16 bits of index, and the upper
		// 16 bits to identify the resource type.
		int ResourceID = StoreNewResource( pConstantBuffer );
		ResourcePtr Proxy( new ResourceProxyDX11( ResourceID, pConfig, this ) );

		return( Proxy );
	}

	return( ResourcePtr( new ResourceProxyDX11() ) );
}
//--------------------------------------------------------------------------------
ResourcePtr RendererDX11::CreateTexture1D( Texture1dConfigDX11* pConfig, D3D11_SUBRESOURCE_DATA* pData,
                                          ShaderResourceViewConfigDX11* pSRVConfig,
                                          RenderTargetViewConfigDX11* pRTVConfig,
                                          UnorderedAccessViewConfigDX11* pUAVConfig )
{
	Texture1DComPtr pTexture;
	HRESULT hr = m_pDevice->CreateTexture1D( &pConfig->m_State, pData, pTexture.GetAddressOf() );

	if ( pTexture )
	{
		Texture1dDX11* pTex = new Texture1dDX11( pTexture );
		pTex->SetDesiredDescription( pConfig->GetTextureDesc() );

		// Return an index with the lower 16 bits of index, and the upper
		// 16 bits to identify the resource type.
		int ResourceID = StoreNewResource( pTex );
		ResourcePtr Proxy( new ResourceProxyDX11( ResourceID, pConfig, this, pSRVConfig, pRTVConfig, pUAVConfig ) );

		return( Proxy );
	}

	return( ResourcePtr( new ResourceProxyDX11() ) );
}
//--------------------------------------------------------------------------------
ResourcePtr RendererDX11::CreateTexture2D( Texture2dConfigDX11* pConfig, D3D11_SUBRESOURCE_DATA* pData,
                                          ShaderResourceViewConfigDX11* pSRVConfig,
                                          RenderTargetViewConfigDX11* pRTVConfig,
                                          UnorderedAccessViewConfigDX11* pUAVConfig,
                                          DepthStencilViewConfigDX11* pDSVConfig )
{
	Texture2DComPtr pTexture;
	HRESULT hr = m_pDevice->CreateTexture2D( &pConfig->m_State, pData, pTexture.GetAddressOf() );

	if ( pTexture )
	{
		Texture2dDX11* pTex = new Texture2dDX11( pTexture );
		pTex->SetDesiredDescription( pConfig->GetTextureDesc() );

		// Return an index with the lower 16 bits of index, and the upper
		// 16 bits to identify the resource type.
		int ResourceID = StoreNewResource( pTex );
		ResourcePtr Proxy( new ResourceProxyDX11( ResourceID, pConfig, this, pSRVConfig, pRTVConfig, pUAVConfig, pDSVConfig ) );

		return( Proxy );
	}

	return( ResourcePtr( new ResourceProxyDX11() ) );
}
//--------------------------------------------------------------------------------
ResourcePtr RendererDX11::CreateTexture3D( Texture3dConfigDX11* pConfig, D3D11_SUBRESOURCE_DATA* pData,
                                          ShaderResourceViewConfigDX11* pSRVConfig,
                                          RenderTargetViewConfigDX11* pRTVConfig,
                                          UnorderedAccessViewConfigDX11* pUAVConfig )
{
	Texture3DComPtr pTexture;
	HRESULT hr = m_pDevice->CreateTexture3D( &pConfig->m_State, pData, pTexture.GetAddressOf() );

	if ( pTexture )
	{
		Texture3dDX11* pTex = new Texture3dDX11( pTexture );
		pTex->SetDesiredDescription( pConfig->GetTextureDesc() );

		// Return an index with the lower 16 bits of index, and the upper
		// 16 bits to identify the resource type.
		int ResourceID = StoreNewResource( pTex );
		ResourcePtr Proxy( new ResourceProxyDX11( ResourceID, pConfig, this, pSRVConfig, pRTVConfig, pUAVConfig ) );

		return( Proxy );
	}

	return( ResourcePtr( new ResourceProxyDX11() ) );
}
//--------------------------------------------------------------------------------
int RendererDX11::CreateShaderResourceView( int ResourceID, D3D11_SHADER_RESOURCE_VIEW_DESC* pDesc )
{
	ID3D11Resource* pRawResource = 0;
	ResourceDX11* pResource = GetResourceByIndex( ResourceID );
	
	if ( pResource ) {
		pRawResource = pResource->GetResource();

		if ( pRawResource ) {
			ShaderResourceViewComPtr pView;
			HRESULT hr = m_pDevice->CreateShaderResourceView( pRawResource, pDesc, pView.GetAddressOf() );

			if ( pView ) {
				m_vShaderResourceViews.push_back( pView );
				return( m_vShaderResourceViews.size() - 1 );
			}
		}
	}

	return( -1 );
}
//--------------------------------------------------------------------------------
int RendererDX11::CreateRenderTargetView( int ResourceID, D3D11_RENDER_TARGET_VIEW_DESC* pDesc )
{
	ID3D11Resource* pRawResource = 0;
	ResourceDX11* pResource = GetResourceByIndex( ResourceID );
	
	if ( pResource ) {
		pRawResource = pResource->GetResource();

		if ( pRawResource ) {
			RenderTargetViewComPtr pView;
			HRESULT hr = m_pDevice->CreateRenderTargetView( pRawResource, pDesc, pView.GetAddressOf() );

			if ( pView ) {
				m_vRenderTargetViews.push_back( pView );
				return( m_vRenderTargetViews.size() - 1 );
			}
		}
	}

	return( -1 );
}
//--------------------------------------------------------------------------------
int RendererDX11::CreateDepthStencilView( int ResourceID, D3D11_DEPTH_STENCIL_VIEW_DESC* pDesc )
{
	ID3D11Resource* pRawResource = 0;
	ResourceDX11* pResource = GetResourceByIndex( ResourceID );
	
	if ( pResource ) {
		pRawResource = pResource->GetResource();

		if ( pRawResource ) {

			DepthStencilViewComPtr pView;
			HRESULT hr = m_pDevice->CreateDepthStencilView( pRawResource, pDesc, pView.GetAddressOf() );

			if ( pView ) {
				m_vDepthStencilViews.push_back( pView );
				return( m_vDepthStencilViews.size() - 1 );
			}
		}
	}

	return( -1 );
}
//--------------------------------------------------------------------------------
int RendererDX11::CreateUnorderedAccessView( int ResourceID, D3D11_UNORDERED_ACCESS_VIEW_DESC* pDesc )
{
	ID3D11Resource* pRawResource = 0;
	ResourceDX11* pResource = GetResourceByIndex( ResourceID );
	
	if ( pResource ) {
		pRawResource = pResource->GetResource();

		if ( pRawResource ) {
			UnorderedAccessViewComPtr pView;
			HRESULT hr = m_pDevice->CreateUnorderedAccessView( pRawResource, pDesc, pView.GetAddressOf() );

			if ( pView ) {
				m_vUnorderedAccessViews.push_back( pView );
				return( m_vUnorderedAccessViews.size() - 1 );
			}
		}
	}

	return( -1 );
}
//--------------------------------------------------------------------------------
void RendererDX11::ResizeTexture( ResourcePtr texture, UINT width, UINT height )
{
	// For the texture, and then for each associated resource view create the new
	// sized versions.  Afterwards, release the old versions and replace them with
	// the new ones.
	int rid = texture->m_iResource;

	// Grab the old texture description and update it for the new size.
	Texture2dDX11* pTexture = GetTexture2DByIndex( rid );
	D3D11_TEXTURE2D_DESC TexDesc = pTexture->GetActualDescription();
	TexDesc.Width = width;
	TexDesc.Height = height;

	// Release the old texture, and replace it with the new one.
	if ( FAILED( m_pDevice->CreateTexture2D( &TexDesc, 0, pTexture->m_pTexture.ReleaseAndGetAddressOf() ) ) ) {
		Log::Get().Write( L"Error trying to resize texture..." );
	}
	
	// Update the description of the texture for future reference.
	pTexture->m_ActualDesc = TexDesc;
	pTexture->m_DesiredDesc = TexDesc;
	texture->m_pTexture2dConfig->m_State = TexDesc; 

	// Resize each of the resource views, if required.
	ResizeTextureSRV( rid, texture->m_iResourceSRV, width, height );
	ResizeTextureRTV( rid, texture->m_iResourceRTV, width, height );
	ResizeTextureDSV( rid, texture->m_iResourceDSV, width, height );
	ResizeTextureUAV( rid, texture->m_iResourceUAV, width, height );
}
//--------------------------------------------------------------------------------
void RendererDX11::ResizeTextureSRV( int RID, int SRVID, UINT width, UINT height )
{
	// Check to make sure we are supposed to do anything...
	if ( SRVID == 0 ) {
		return;
	}

	ResourceDX11* pResource = GetResourceByIndex( RID );

	// Check that the input resources / views are legit.
	unsigned int index = static_cast<unsigned int>( SRVID );

	if ( !pResource || !( index < m_vShaderResourceViews.size() ) || (pResource->GetType() != RT_TEXTURE2D ) ) {
		Log::Get().Write( L"Error trying to resize a SRV!!!!" );
		return;
	}

	// Get the existing UAV.
	ShaderResourceViewDX11& SRV = m_vShaderResourceViews[index];

	// Get its description.
	D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc;
	SRV.m_pShaderResourceView->GetDesc( &SRVDesc );
	
	// Create the new one.
	if ( FAILED( m_pDevice->CreateShaderResourceView( 
		pResource->GetResource(), 
		&SRVDesc, 
		SRV.m_pShaderResourceView.ReleaseAndGetAddressOf() ) ) )
	{
		Log::Get().Write( L"Error trying to resize a SRV!!!!" );
	}
}
//--------------------------------------------------------------------------------
void RendererDX11::ResizeTextureRTV( int RID, int RTVID, UINT width, UINT height )
{
	// Check to make sure we are supposed to do anything...
	if ( RTVID == 0 ) {
		return;
	}

	ResourceDX11* pResource = GetResourceByIndex( RID );

	// Check that the input resources / views are legit.
	unsigned int index = static_cast<unsigned int>( RTVID );

	if ( !pResource || !( index < m_vRenderTargetViews.size() ) || (pResource->GetType() != RT_TEXTURE2D ) ) {
		Log::Get().Write( L"Error trying to resize a RTV!!!!" );
		return;
	}

	// Get the existing UAV.
	RenderTargetViewDX11& RTV = m_vRenderTargetViews[index];

	// Get its description.
	D3D11_RENDER_TARGET_VIEW_DESC RTVDesc;
	RTV.m_pRenderTargetView->GetDesc( &RTVDesc );
	
	// Create the new one.
	if ( FAILED( m_pDevice->CreateRenderTargetView( 
		pResource->GetResource(), 
		&RTVDesc, 
		RTV.m_pRenderTargetView.ReleaseAndGetAddressOf() ) ) )
	{
		Log::Get().Write( L"Error trying to resize a RTV!!!!" );
	}
}
//--------------------------------------------------------------------------------
void RendererDX11::ResizeTextureDSV( int RID, int DSVID, UINT width, UINT height )
{
	// Check to make sure we are supposed to do anything...
	if ( DSVID == 0 ) {
		return;
	}

	ResourceDX11* pResource = GetResourceByIndex( RID );

	// Check that the input resources / views are legit.
	unsigned int index = static_cast<unsigned int>( DSVID );

	if ( !pResource || !( index < m_vDepthStencilViews.size() ) || (pResource->GetType() != RT_TEXTURE2D ) ) {
		Log::Get().Write( L"Error trying to resize a DSV!!!!" );
		return;
	}

	// Get the existing UAV.
	DepthStencilViewDX11& DSV = m_vDepthStencilViews[index];

	// Get its description.
	D3D11_DEPTH_STENCIL_VIEW_DESC DSVDesc;
	DSV.m_pDepthStencilView->GetDesc( &DSVDesc );
	
	// Create the new one.
	if ( FAILED(  m_pDevice->CreateDepthStencilView( 
		pResource->GetResource(), 
		&DSVDesc,
		DSV.m_pDepthStencilView.ReleaseAndGetAddressOf() ) ) )
	{
		Log::Get().Write( L"Error trying to resize a DSV!!!!" );
	}
}
//--------------------------------------------------------------------------------
void RendererDX11::ResizeTextureUAV( int RID, int UAVID, UINT width, UINT height )
{
	// Check to make sure we are supposed to do anything...
	if ( UAVID == 0 ) {
		return;
	}

	ResourceDX11* pResource = GetResourceByIndex( RID );

	// Check that the input resources / views are legit.
	unsigned int index = static_cast<unsigned int>( UAVID );

	if ( !pResource || !( index < m_vUnorderedAccessViews.size() ) || (pResource->GetType() != RT_TEXTURE2D ) ) {
		Log::Get().Write( L"Error trying to resize a UAV!!!!" );
		return;
	}

	// Get the existing UAV.
	UnorderedAccessViewDX11& UAV = m_vUnorderedAccessViews[index];

	// Get its description.
	D3D11_UNORDERED_ACCESS_VIEW_DESC UAVDesc;
	UAV.m_pUnorderedAccessView->GetDesc( &UAVDesc );
	
	// Create the new one.
	if ( FAILED( m_pDevice->CreateUnorderedAccessView( 
		pResource->GetResource(),
		&UAVDesc,
		UAV.m_pUnorderedAccessView.ReleaseAndGetAddressOf() ) ) )
	{
		Log::Get().Write( L"Error trying to resize a UAV!!!!" );
	}
}
//--------------------------------------------------------------------------------
void RendererDX11::ResizeSwapChain( int SID, UINT width, UINT height )
{
	unsigned int index = static_cast<unsigned int>( SID );

	if ( !( index < m_vSwapChains.size() ) ) {
		Log::Get().Write( L"Error trying to resize swap chain!" );
		return;
	}

	// In order to resize a swap chain, you first have to release all outstanding
	// references to it.  In our case, this means to release the texture and 
	// render target view that we maintain in the renderer.

	SwapChainDX11* pSwapChain = m_vSwapChains[index];

	Texture2dDX11* pBackBuffer = GetTexture2DByIndex( pSwapChain->m_Resource->m_iResource );
	pBackBuffer->m_pTexture.Reset();

	RenderTargetViewDX11& RTV = m_vRenderTargetViews[pSwapChain->m_Resource->m_iResourceRTV];
	
	// Get its description.
	DXGI_SWAP_CHAIN_DESC SwapDesc;
	pSwapChain->GetSwapChain()->GetDesc( &SwapDesc );

	D3D11_RENDER_TARGET_VIEW_DESC RTVDesc;
	RTV.m_pRenderTargetView->GetDesc( &RTVDesc );
	RTV.m_pRenderTargetView.Reset();


	this->pImmPipeline->ClearPipelineState();
	
	for ( int i = 0; i < NUM_THREADS; i++ ) {
		g_aPayload[i].pPipeline->ClearPipelineState();
	}

	// Resize the buffers.
	HRESULT hr = pSwapChain->m_pSwapChain->ResizeBuffers( 2, width, height, SwapDesc.BufferDesc.Format, SwapDesc.Flags );

	if ( FAILED(hr) ) {
		Log::Get().Write( L"Failed to resize buffers!" );
	}

	// Re-acquire the back buffer reference.
	hr = pSwapChain->m_pSwapChain->GetBuffer( 0, __uuidof( ID3D11Texture2D ), 
		reinterpret_cast< void** >( pBackBuffer->m_pTexture.GetAddressOf() ) );

	// Create the new one.
	hr = m_pDevice->CreateRenderTargetView( pBackBuffer->m_pTexture.Get(), &RTVDesc, RTV.m_pRenderTargetView.GetAddressOf() );
}
//--------------------------------------------------------------------------------
void RendererDX11::ResizeViewport( int ID, UINT width, UINT height )
{
	unsigned int index = static_cast<unsigned int>( ID );

	if ( !( index < m_vViewPorts.size() ) ) {
		Log::Get().Write( L"Error trying to resize viewport!" );
	}

	ViewPortDX11& pViewport = m_vViewPorts[index];
	pViewport.m_ViewPort.Width = static_cast<float>( width );
	pViewport.m_ViewPort.Height = static_cast<float>( height );
}
//--------------------------------------------------------------------------------
int RendererDX11::LoadShader( ShaderType type, std::wstring& filename, std::wstring& function, 
                             std::wstring& model, bool enablelogging )
{
    return LoadShader( type, filename, function, model, NULL, enablelogging );
}
//--------------------------------------------------------------------------------
int RendererDX11::LoadShader( ShaderType type, std::wstring& filename, std::wstring& function, 
                                std::wstring& model, const D3D_SHADER_MACRO* pDefines, bool enablelogging )
{

	// Check the existing list of shader files to see if there are any matches
	// before trying to load it up again.  This will reduce the load times,
	// and should speed up rendering in many cases since the shader object won't 
	// have to be bound again.  
	//
	// In the case that there are any defines passed in, we skip returning the 
	// cached shader - we assume that something is different about the shader due
	// to the defines, so we can't just reuse a previously loaded one.
	
	for ( unsigned int i = 0; i < m_vShaders.size(); i++ )
	{
		ShaderDX11* pShader = m_vShaders[i];

		if ( pShader->FileName.compare( filename ) == 0
			&& pShader->Function.compare( function ) == 0
			&& pShader->ShaderModel.compare( model ) == 0
			&& pDefines == nullptr )
		{
			return( i );
		}
	}

	HRESULT hr = S_OK;

	ID3DBlob* pCompiledShader = NULL;

	pCompiledShader = ShaderFactoryDX11::GenerateShader( type, filename, function, model, pDefines, enablelogging );
	//pCompiledShader = ShaderFactoryDX11::GeneratePrecompiledShader( filename, function, model );

	if ( pCompiledShader == nullptr ) {
		return( -1 );
	}

	// Create the shader wrapper to house all of the information about its interface.

	ShaderDX11* pShaderWrapper = 0;

	switch( type )
	{
	case VERTEX_SHADER:
		{
			ID3D11VertexShader* pShader = 0;

			hr = m_pDevice->CreateVertexShader(
				pCompiledShader->GetBufferPointer(),
				pCompiledShader->GetBufferSize(),
				0, &pShader );

			pShaderWrapper = new VertexShaderDX11( pShader );
			break;
		}

	case HULL_SHADER:
		{
			ID3D11HullShader* pShader = 0;

			hr = m_pDevice->CreateHullShader(
				pCompiledShader->GetBufferPointer(),
				pCompiledShader->GetBufferSize(),
				0, &pShader );

			pShaderWrapper = new HullShaderDX11( pShader );
			break;
		}

	case DOMAIN_SHADER:
		{
			ID3D11DomainShader* pShader = 0;

			hr = m_pDevice->CreateDomainShader(
				pCompiledShader->GetBufferPointer(),
				pCompiledShader->GetBufferSize(),
				0, &pShader );

			pShaderWrapper = new DomainShaderDX11( pShader );
			break;
		}

	case GEOMETRY_SHADER:
		{
			ID3D11GeometryShader* pShader = 0;

			hr = m_pDevice->CreateGeometryShader(
				pCompiledShader->GetBufferPointer(),
				pCompiledShader->GetBufferSize(),
				0, &pShader );

			pShaderWrapper = new GeometryShaderDX11( pShader );
			break;
		}

	case PIXEL_SHADER:
		{
			ID3D11PixelShader* pShader = 0;

			hr = m_pDevice->CreatePixelShader(
				pCompiledShader->GetBufferPointer(),
				pCompiledShader->GetBufferSize(),
				0, &pShader );

			pShaderWrapper = new PixelShaderDX11( pShader );
			break;
		}

	case COMPUTE_SHADER:
		{
			ID3D11ComputeShader* pShader = 0;

			hr = m_pDevice->CreateComputeShader(
				pCompiledShader->GetBufferPointer(),
				pCompiledShader->GetBufferSize(),
				0, &pShader );
			pShaderWrapper = new ComputeShaderDX11( pShader );
			break;
		}
	}

	if ( FAILED( hr ) )
	{
		Log::Get().Write( L"Failed to create shader!" );
		pCompiledShader->Release();
		delete pShaderWrapper;
		return( -1 );
	}

	pShaderWrapper->FileName = filename;
	pShaderWrapper->Function = function;
	pShaderWrapper->ShaderModel = model;

	m_vShaders.push_back( pShaderWrapper );




	// Store the compiled shader in the shader wrapper for use later on in creating
	// and checking input and output signatures.
	pShaderWrapper->m_pCompiledShader = pCompiledShader;


	ShaderReflectionDX11* pReflection = ShaderReflectionFactoryDX11::GenerateReflection( *pShaderWrapper );


	// Initialize the constant buffers of this shader, so that they aren't 
	// lazy created later on...

	pReflection->InitializeConstantBuffers( m_pParamMgr );

	pShaderWrapper->SetReflection( pReflection );

	//pReflection->PrintShaderDetails();


	// Return the index for future referencing.

	return( m_vShaders.size() - 1 );

}
//--------------------------------------------------------------------------------
int RendererDX11::CreateInputLayout( std::vector<D3D11_INPUT_ELEMENT_DESC>& elements, int ShaderID  )
{
	// Create array of elements here for the API call.
	D3D11_INPUT_ELEMENT_DESC* pElements = new D3D11_INPUT_ELEMENT_DESC[elements.size()];
	for ( unsigned int i = 0; i < elements.size(); i++ )
		pElements[i] = elements[i];
	
	// Attempt to create the input layout from the input information.
	ID3DBlob* pCompiledShader = m_vShaders[ShaderID]->m_pCompiledShader;
	InputLayoutComPtr pLayout;

	HRESULT hr = m_pDevice->CreateInputLayout( pElements, elements.size(), 
		pCompiledShader->GetBufferPointer(), pCompiledShader->GetBufferSize(), pLayout.GetAddressOf() );

	// Release the input elements array.
	delete[] pElements;

	// On failure, log the error and return an invalid index.
	if ( FAILED( hr ) )
	{
		Log::Get().Write( L"Failed to create input layout!" );
		return( -1 );
	}

	m_vInputLayouts.push_back( pLayout );
	
	// Return the index for referencing later on.
	return( m_vInputLayouts.size() - 1 );
}
//--------------------------------------------------------------------------------
ResourcePtr RendererDX11::LoadTexture( std::wstring filename, bool sRGB )
{
	ComPtr<ID3D11Resource> pResource;

	FileSystem fs;
	filename = fs.GetTextureFolder() + filename;

	// Test whether this is a DDS file or not.
	std::wstring extension = filename.substr( filename.size()-3, 3 );
	std::transform( extension.begin(), extension.end(), extension.begin(), ::tolower );

	HRESULT hr = S_OK;

	if ( extension == L"dds" ) 
	{
		hr = DirectX::CreateDDSTextureFromFile(
			m_pDevice.Get(),
			filename.c_str(),
			//0,
			//D3D11_USAGE_DEFAULT,
			//D3D11_BIND_SHADER_RESOURCE,
			//0,
			//0,
			//sRGB,
			pResource.GetAddressOf(),
			nullptr );
	}
	else
	{
		hr = DirectX::CreateWICTextureFromFileEx(
			m_pDevice.Get(),
			pImmPipeline->m_pContext.Get(),
			filename.c_str(),
			0,
			D3D11_USAGE_DEFAULT,
			D3D11_BIND_SHADER_RESOURCE,
			0,
			0,
			sRGB,
			pResource.GetAddressOf(),
			0 );
	}



	if ( FAILED( hr ) )
	{
		Log::Get().Write( L"Failed to load texture from file: " + filename );
		return( ResourcePtr( new ResourceProxyDX11() ) );
	}

	ComPtr<ID3D11Texture2D> pTexture;
	pResource.CopyTo( pTexture.GetAddressOf() );

	int ResourceID = StoreNewResource( new Texture2dDX11( pTexture ) );

	Texture2dConfigDX11 TextureConfig;
	pTexture->GetDesc( &TextureConfig.m_State );

	return( ResourcePtr( new ResourceProxyDX11( ResourceID, &TextureConfig, this ) ) );
}
//--------------------------------------------------------------------------------
ResourcePtr RendererDX11::LoadTexture( void* pData, SIZE_T sizeInBytes/*, D3DX11_IMAGE_LOAD_INFO* pLoadInfo*/ )
{
	ComPtr<ID3D11Resource> pResource;

	HRESULT hr = DirectX::CreateWICTextureFromMemory(
		m_pDevice.Get(),
		pImmPipeline->m_pContext.Get(),
		static_cast<uint8_t*>(pData),
		sizeInBytes,
		pResource.GetAddressOf(),
		0 );

	if ( FAILED( hr ) )
	{
		Log::Get().Write( L"Failed to load texture from memory!" );
		return( ResourcePtr( new ResourceProxyDX11() ) );
	}

	ComPtr<ID3D11Texture2D> pTexture;
	pResource.CopyTo( pTexture.GetAddressOf() );

	int ResourceID = StoreNewResource( new Texture2dDX11( pTexture ) );

	Texture2dConfigDX11 TextureConfig;
	pTexture->GetDesc( &TextureConfig.m_State );

	return( ResourcePtr( new ResourceProxyDX11( ResourceID, &TextureConfig, this ) ) );
}
//--------------------------------------------------------------------------------
ResourcePtr RendererDX11::LoadTexture( ID3D11Texture2D* pTexture )
{
	// Add a reference to the texture to ensure it doesn't get destroyed while we 
	// are using it.

	// TODO: This method has to be double checked to ensure that the reference 
	//       counting functions properly with the externally created texture!

	ComPtr<ID3D11Texture2D> pTexturePtr( pTexture );

    int ResourceID = StoreNewResource( new Texture2dDX11( pTexture ) );

    Texture2dConfigDX11 TextureConfig;
    pTexture->GetDesc( &TextureConfig.m_State );

    return( ResourcePtr( new ResourceProxyDX11( ResourceID, &TextureConfig, this ) ) );
}
//--------------------------------------------------------------------------------
int RendererDX11::CreateBlendState( BlendStateConfigDX11* pConfig )
{
	BlendStateComPtr pState;

	HRESULT hr = m_pDevice->CreateBlendState( dynamic_cast<D3D11_BLEND_DESC*>( pConfig ), pState.GetAddressOf() );

	if ( FAILED( hr ) )
	{
		Log::Get().Write( L"Failed to create blend state!" );
		return( -1 );
	}

	m_vBlendStates.push_back( pState );

	return( m_vBlendStates.size() - 1 );
}
//--------------------------------------------------------------------------------
int RendererDX11::CreateDepthStencilState( DepthStencilStateConfigDX11* pConfig )
{
	DepthStencilStateComPtr pState;

	HRESULT hr = m_pDevice->CreateDepthStencilState( dynamic_cast<D3D11_DEPTH_STENCIL_DESC*>( pConfig ), pState.GetAddressOf() );

	if ( FAILED( hr ) )
	{
		Log::Get().Write( L"Failed to create depth stencil state!" );
		return( -1 );
	}

	m_vDepthStencilStates.push_back( pState );

	return( m_vDepthStencilStates.size() - 1 );
}
//--------------------------------------------------------------------------------
int RendererDX11::CreateRasterizerState( RasterizerStateConfigDX11* pConfig )
{
	RasterizerStateComPtr pState;

	HRESULT hr = m_pDevice->CreateRasterizerState( dynamic_cast<D3D11_RASTERIZER_DESC*>( pConfig ), pState.GetAddressOf() );

	if ( FAILED( hr ) )
	{
		Log::Get().Write( L"Failed to create rasterizer state!" );
		return( -1 );
	}

	m_vRasterizerStates.push_back( pState );

	return( m_vRasterizerStates.size() - 1 );
}
//--------------------------------------------------------------------------------
int RendererDX11::CreateSamplerState( D3D11_SAMPLER_DESC* pDesc )
{
	SamplerStateComPtr pState;

	HRESULT hr = m_pDevice->CreateSamplerState( pDesc, pState.GetAddressOf() );

	if ( FAILED( hr ) )
	{
		Log::Get().Write( L"Failed to create sampler state!" );
		return( -1 );
	}

	m_vSamplerStates.push_back( pState );

	return( m_vSamplerStates.size() - 1 );
}
//--------------------------------------------------------------------------------
int RendererDX11::CreateViewPort( D3D11_VIEWPORT viewport )
{
	m_vViewPorts.emplace_back( viewport );

	return( m_vViewPorts.size() - 1 );
}
//--------------------------------------------------------------------------------
ResourcePtr RendererDX11::GetSwapChainResource( int ID )
{
	unsigned int index = static_cast<unsigned int>( ID );

	if ( index < m_vSwapChains.size() )
		return( m_vSwapChains[index]->m_Resource );

	Log::Get().Write( L"Tried to get an invalid swap buffer index texture ID!" );

	return( ResourcePtr( new ResourceProxyDX11() ) );
}
//--------------------------------------------------------------------------------
Vector2f RendererDX11::GetDesktopResolution()
{
	// Acquire the DXGI device, then the adapter, then the output...

	ComPtr<IDXGIDevice> pDXGIDevice;
	HRESULT hr = m_pDevice.CopyTo( pDXGIDevice.GetAddressOf() );
	ComPtr<IDXGIAdapter> pDXGIAdapter;
	hr = pDXGIDevice->GetParent(__uuidof(IDXGIAdapter), reinterpret_cast<void **>( pDXGIAdapter.GetAddressOf() ) );

	// Take the first output in the adapter.

	ComPtr<IDXGIOutput> pDXGIOutput;
	pDXGIAdapter->EnumOutputs( 0, pDXGIOutput.GetAddressOf() );

	// Use the output interface to get the output description.

	DXGI_OUTPUT_DESC desc;
	pDXGIOutput->GetDesc( &desc );

	// Return the current output's resolution from the description.
	return( Vector2f( static_cast<float>( desc.DesktopCoordinates.right - desc.DesktopCoordinates.left ),
						static_cast<float>( desc.DesktopCoordinates.bottom - desc.DesktopCoordinates.top ) ) );
}
//--------------------------------------------------------------------------------
BlendStateComPtr RendererDX11::GetBlendState( int index )
{
	// TODO: There should be a default blend state that can be returned which will
	//       put the blend state into the default D3D11 state...

	if ( index >= 0 )
		return( m_vBlendStates[index] ); 
	else
		return( m_vBlendStates[0] );
}
//--------------------------------------------------------------------------------
DepthStencilStateComPtr RendererDX11::GetDepthState( int index )
{
	// TODO: There should be a default blend state that can be returned which will
	//       put the blend state into the default D3D11 state...

	if ( index >= 0 )
		return( m_vDepthStencilStates[index] );
	else
		return( m_vDepthStencilStates[0] );
}
//--------------------------------------------------------------------------------
RasterizerStateComPtr RendererDX11::GetRasterizerState( int index )
{
	// TODO: There should be a default blend state that can be returned which will
	//       put the blend state into the default D3D11 state...

	if ( index >= 0 )
		return( m_vRasterizerStates[index] );
	else
		return( m_vRasterizerStates[0] );
}
//--------------------------------------------------------------------------------
const ViewPortDX11& RendererDX11::GetViewPort( int ID )
{
	unsigned int index = static_cast<unsigned int>( ID );
	
	assert( index < m_vViewPorts.size() );

	return( m_vViewPorts[index] );
}
//--------------------------------------------------------------------------------
ResourceDX11* RendererDX11::GetResourceByIndex( int ID )
{
	ResourceDX11* pResource = 0;

	unsigned int index = ID & 0xffff;
	int innerID = ( ID & 0xffff0000 ) >> 16;

	if ( index < m_vResources.size() ) {
		pResource = m_vResources[index];

		if ( pResource->GetInnerID() != innerID ) {
			Log::Get().Write( L"Inner ID doesn't match resource index!!!" );
		}
	}

	return( pResource );
}
//--------------------------------------------------------------------------------
InputLayoutComPtr RendererDX11::GetInputLayout( int index )
{
	return( m_vInputLayouts[index] );
}
//--------------------------------------------------------------------------------
SamplerStateComPtr RendererDX11::GetSamplerState( int index )
{
	return( m_vSamplerStates[index] );
}
//--------------------------------------------------------------------------------
ShaderDX11* RendererDX11::GetShader( int ID )
{
	unsigned int index = static_cast<unsigned int>( ID );

	if ( index < m_vShaders.size() )
		return( m_vShaders[index] );
	else
		return( nullptr );
}
//--------------------------------------------------------------------------------
void RendererDX11::QueueTask( Task* pTask )
{
	m_vQueuedTasks.push_back( pTask );
}
//--------------------------------------------------------------------------------
void RendererDX11::ProcessTaskQueue( )
{
	MultiThreadingConfig.ApplyConfiguration();

	if ( MultiThreadingConfig.GetConfiguration() == false )
	{
		// Single-threaded processing of the render view queue

		for ( int i = m_vQueuedTasks.size()-1; i >= 0; i-=NUM_THREADS )
        {
			for ( int j = 0; j < NUM_THREADS; j++ )
			{
				if ( (i-j) >= 0 )
				{
					//
					pImmPipeline->BeginEvent( std::wstring( L"View Draw: ") + m_vQueuedTasks[i-j]->GetName() );
					m_vQueuedTasks[i-j]->ExecuteTask( pImmPipeline, g_aPayload[j].pParamManager );
					pImmPipeline->EndEvent();
					//PIXEndEvent();
				}
			}
        }

		m_vQueuedTasks.clear();
	}
	else
	{
		// Multi-threaded processing of the render view queue

		for ( int i = m_vQueuedTasks.size()-1; i >= 0; i-=NUM_THREADS )
		{
			DWORD count = 0;

			for ( int j = 0; j < NUM_THREADS; j++ )
			{
				if ( (i-j) >= 0 )
				{
					count++;
					g_aPayload[j].pTask = m_vQueuedTasks[i-j];
					SetEvent( g_aBeginEventHandle[j] );
				}
			}

			WaitForMultipleObjects( count, g_aEndEventHandle, true, INFINITE );

			for ( int j = 0; count > 0; count-- )
			{
				pImmPipeline->ExecuteCommandList( g_aPayload[j].pList );
				g_aPayload[j].pList->ReleaseList();
				j++;
			}

		}

		m_vQueuedTasks.clear();
	}
}
//--------------------------------------------------------------------------------


//--------------------------------------------------------------------------------
// Here is the render view process for each thread.  The intention here is to 
// have a thread perform a single render view's rendering commands to generate
// a command list.  This list is later used by the immediate context to execute
// the list.
//--------------------------------------------------------------------------------
HANDLE						g_aThreadHandles[NUM_THREADS];
Glyph3::ThreadPayLoad		g_aPayload[NUM_THREADS];
HANDLE						g_aBeginEventHandle[NUM_THREADS];
HANDLE						g_aEndEventHandle[NUM_THREADS];


unsigned int WINAPI _TaskThreadProc( void* lpParameter )
{
	ThreadPayLoad* pPayload = (ThreadPayLoad*) lpParameter;

	int id = pPayload->id;


	for ( ; ; )
	{
		// Wait for the main thread to signal that there is a payload available.
		WaitForSingleObject( g_aBeginEventHandle[id], INFINITE );

		pPayload->pPipeline->m_pContext->ClearState();

		// Execute the render view with the provided pipeline and parameter managers.
		pPayload->pTask->ExecuteTask( pPayload->pPipeline, pPayload->pParamManager );

		// Generate the command list.
		pPayload->pPipeline->GenerateCommandList( pPayload->pList );

		// Signal to the main thread that the view has been collected into a command list.
		SetEvent( g_aEndEventHandle[id] );
	}

	return( 0 );
}
//--------------------------------------------------------------------------------
Texture1dDX11* RendererDX11::GetTexture1DByIndex( int rid )
{
	Texture1dDX11* pResult = 0;

	ResourceDX11* pResource = GetResourceByIndex(rid);

	if ( pResource != NULL ) {
		if ( pResource->GetType() != RT_TEXTURE1D ) {
			Log::Get().Write( L"Trying to access a non-texture1D resource!!!!" );
		} else {
			pResult = reinterpret_cast<Texture1dDX11*>( pResource );
		}
	}

	return( pResult );
}
//--------------------------------------------------------------------------------
Texture2dDX11* RendererDX11::GetTexture2DByIndex( int rid )
{
	Texture2dDX11* pResult = 0;

	ResourceDX11* pResource = GetResourceByIndex(rid);

	if ( pResource != NULL ) {
		if ( pResource->GetType() != RT_TEXTURE2D ) {
			Log::Get().Write( L"Trying to access a non-texture2D resource!!!!" );
		} else {
			pResult = reinterpret_cast<Texture2dDX11*>( pResource );
		}
	}

	return( pResult );
}
//--------------------------------------------------------------------------------
Texture3dDX11* RendererDX11::GetTexture3DByIndex( int rid )
{
	Texture3dDX11* pResult = 0;

	ResourceDX11* pResource = GetResourceByIndex(rid);
	
	if ( pResource != NULL ) {
		if ( pResource->GetType() != RT_TEXTURE3D ) {
			Log::Get().Write( L"Trying to access a non-texture3D resource!!!!" );
		} else {
			pResult = reinterpret_cast<Texture3dDX11*>( pResource );
		}
	}

	return( pResult );
}
//--------------------------------------------------------------------------------
SwapChainDX11* RendererDX11::GetSwapChainByIndex( int sid )
{
	return( m_vSwapChains[sid] );
}
//--------------------------------------------------------------------------------
BufferDX11* RendererDX11::GetGenericBufferByIndex( int rid )
{
	// This method returns a BufferDX11 pointer, which is useful for methods that
	// can operate on more than one type of buffer.  As long as the underlying
	// resource is not a texture, then the pointer is returned, otherwise null
	// is returned.

	BufferDX11* pResult = 0;

	ResourceDX11* pResource = GetResourceByIndex(rid);
	
	if ( pResource != NULL ) {
		if ( pResource->GetType() == RT_TEXTURE1D || pResource->GetType() == RT_TEXTURE2D || pResource->GetType() == RT_TEXTURE3D ) {
			Log::Get().Write( L"Trying to access a non-buffer resource!!!!" );
		} else {
			pResult = reinterpret_cast<BufferDX11*>( pResource );
		}
	}

	return( pResult );
}
//--------------------------------------------------------------------------------
ConstantBufferDX11* RendererDX11::GetConstantBufferByIndex( int rid )
{
	ConstantBufferDX11* pResult = 0;

	ResourceDX11* pResource = GetResourceByIndex(rid);
	
	if ( pResource != NULL ) {
		if ( pResource->GetType() != RT_CONSTANTBUFFER ) {
			Log::Get().Write( L"Trying to access a non-vertex buffer resource!!!!" );
		} else {
			pResult = reinterpret_cast<ConstantBufferDX11*>( pResource );
		}
	}

	return( pResult );
}
//--------------------------------------------------------------------------------
VertexBufferDX11* RendererDX11::GetVertexBufferByIndex( int rid )
{
	VertexBufferDX11* pResult = 0;

	ResourceDX11* pResource = GetResourceByIndex(rid);
	
	if ( pResource != NULL ) {
		if ( pResource->GetType() != RT_VERTEXBUFFER ) {
			Log::Get().Write( L"Trying to access a non-vertex buffer resource!!!!" );
		} else {
			pResult = reinterpret_cast<VertexBufferDX11*>( pResource );
		}
	}

	return( pResult );
}
//--------------------------------------------------------------------------------
IndexBufferDX11* RendererDX11::GetIndexBufferByIndex( int rid )
{
	IndexBufferDX11* pResult = 0;

	ResourceDX11* pResource = GetResourceByIndex(rid);

	if ( pResource != NULL ) {
		if ( pResource->GetType() != RT_INDEXBUFFER ) {
			Log::Get().Write( L"Trying to access a non-index buffer resource!!!!" );
		} else {
			pResult = reinterpret_cast<IndexBufferDX11*>( pResource );
		}
	}

	return( pResult );
}
//--------------------------------------------------------------------------------
ByteAddressBufferDX11* RendererDX11::GetByteAddressBufferByIndex( int rid )
{
	ByteAddressBufferDX11* pResult = 0;

	ResourceDX11* pResource = GetResourceByIndex(rid);

	if ( pResource != NULL ) {
		if ( pResource->GetType() != RT_BYTEADDRESSBUFFER ) {
			Log::Get().Write( L"Trying to access a non-byte address buffer resource!!!!" );
		} else {
			pResult = reinterpret_cast<ByteAddressBufferDX11*>( pResource );
		}
	}

	return( pResult );
}
//--------------------------------------------------------------------------------
IndirectArgsBufferDX11*	RendererDX11::GetIndirectArgsBufferByIndex( int rid )
{
	IndirectArgsBufferDX11* pResult = 0;

	ResourceDX11* pResource = GetResourceByIndex(rid);			
			
	if ( pResource != NULL ) {
		if ( pResource->GetType() != RT_INDIRECTARGSBUFFER ) {
			Log::Get().Write( L"Trying to access a non-indirect args buffer resource!!!!" );
		} else {
			pResult = reinterpret_cast<IndirectArgsBufferDX11*>( pResource );
		}
	}

	return( pResult );
}
//--------------------------------------------------------------------------------
StructuredBufferDX11* RendererDX11::GetStructuredBufferByIndex( int rid )
{
	StructuredBufferDX11* pResult = 0;

	ResourceDX11* pResource = GetResourceByIndex(rid);

	if ( pResource != NULL ) {
		if ( pResource->GetType() != RT_STRUCTUREDBUFFER ) {
			Log::Get().Write( L"Trying to access a non-structured buffer resource!!!!" );
		} else {
			pResult = reinterpret_cast<StructuredBufferDX11*>( pResource );
		}
	}

	return( pResult );
}
//--------------------------------------------------------------------------------
RenderTargetViewDX11& RendererDX11::GetRenderTargetViewByIndex( int rid )
{
	assert( rid >= 0 );
	assert( rid < m_vRenderTargetViews.size() );

	return( m_vRenderTargetViews[rid] );
}
//--------------------------------------------------------------------------------
DepthStencilViewDX11& RendererDX11::GetDepthStencilViewByIndex( int rid )
{
	assert( rid >= 0 );
	assert( rid < m_vDepthStencilViews.size() );

	return( m_vDepthStencilViews[rid] );
}
//--------------------------------------------------------------------------------
ShaderResourceViewDX11& RendererDX11::GetShaderResourceViewByIndex( int rid )
{
	assert( rid >= 0 );
	assert( rid < m_vShaderResourceViews.size() );

	return( m_vShaderResourceViews[rid] );
}
//--------------------------------------------------------------------------------
UnorderedAccessViewDX11& RendererDX11::GetUnorderedAccessViewByIndex( int rid )
{
	assert( rid >= 0 );
	assert( rid < m_vUnorderedAccessViews.size() );

	return( m_vUnorderedAccessViews[rid] );
}
//--------------------------------------------------------------------------------
int	RendererDX11::GetUnusedResourceIndex()
{
	// Initialize return index to -1.
	int index = -1;
	
	// Search for a NULL index location.
	for ( unsigned int i = 0; i < m_vResources.size(); i++ ) {
		if ( m_vResources[i] == NULL ) {
			index = i;
			break;
		}
	}

	// Return either an empty location, or -1 if none exist.
	return( index );
}
//--------------------------------------------------------------------------------
int	RendererDX11::StoreNewResource( ResourceDX11* pResource )
{
	// This method either finds an empty spot in the list, or just appends the
	// resource to the end of it if none are available.

	int index = GetUnusedResourceIndex();

	if ( index == -1 ) {
		m_vResources.push_back( pResource );
		index = m_vResources.size() - 1;
	} else {
		m_vResources[index] = pResource;
	}

	// Shift the inner ID to the upper 16 bits.
	int innerID = (int)pResource->GetInnerID() << 16;
	index = index + innerID;

	return( index );
}
//--------------------------------------------------------------------------------
void RendererDX11::DeleteResource( ResourcePtr ptr )
{
	// This is a convenience method that just passes the resource index to
	// the delete function.

	DeleteResource( ptr->m_iResource );
}
//--------------------------------------------------------------------------------
void RendererDX11::DeleteResource( int index )
{
	// Here the resource is looked up, then deleted if it was found.  After 
	// being deleted, it is 
	ResourceDX11* pResource = GetResourceByIndex( index );

	if ( pResource != nullptr ) {
		delete pResource;
		m_vResources[index & 0xffff] = nullptr;
	}
}
//--------------------------------------------------------------------------------
ID3D11Device* RendererDX11::GetDevice()
{
    return( m_pDevice.Get() );
}
//--------------------------------------------------------------------------------
template <class T>
void LogObjectVector( std::vector<T> objects )
{
	for ( auto object : objects )
		Log::Get().Write( object.ToString() );
}
//--------------------------------------------------------------------------------
template <class T>
void LogObjectPtrVector( std::vector<T> objects )
{
	for ( auto object : objects )
		Log::Get().Write( object->ToString() );
}
//--------------------------------------------------------------------------------
