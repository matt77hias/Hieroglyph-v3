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

#include "InputLayoutDX11.h"
#include "BlendStateDX11.h"
#include "DepthStencilStateDX11.h"
#include "RasterizerStateDX11.h"
#include "SamplerStateDX11.h"

#include "BlendStateConfigDX11.h"
#include "DepthStencilStateConfigDX11.h"
#include "RasterizerStateConfigDX11.h"

#include "VertexShaderDX11.h"
#include "HullShaderDX11.h"
#include "DomainShaderDX11.h"
#include "GeometryShaderDX11.h"
#include "PixelShaderDX11.h"
#include "ComputeShaderDX11.h"

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

#include "IRenderView.h"

#include "EventManager.h"
#include "EvtErrorMessage.h"

#include "FileSystem.h"
#include "Process.h"
#include <sstream>
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
RendererDX11* RendererDX11::m_spRenderer = 0;
//--------------------------------------------------------------------------------
RendererDX11::RendererDX11()
{
	if ( m_spRenderer == 0 )
		m_spRenderer = this;

	m_pDevice = 0;
	m_pDebugger = 0;
	m_driverType = D3D_DRIVER_TYPE_NULL;


	m_pParamMgr = 0;
	pImmPipeline = 0;

	m_bMultiThreadActive = true;			// Initialize this to always use MT!
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
			NULL,
			DriverType,
			NULL,
			0,
			NULL,
			0,
			D3D11_SDK_VERSION,
			NULL,
			&FeatureLevel,
			NULL );

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
bool RendererDX11::Initialize( D3D_DRIVER_TYPE DriverType, D3D_FEATURE_LEVEL FeatureLevel )
{

    HRESULT hr = S_OK;

	// Create a factory to enumerate all of the hardware in the system.

	IDXGIFactory1* pFactory = 0;
	hr = CreateDXGIFactory( __uuidof(IDXGIFactory), (void**)(&pFactory) );


	// Enumerate all of the adapters in the current system.  This includes all
	// adapters, even the ones that don't support the ID3D11Device interface.

	IDXGIAdapter1* pAdapter;
	TArray<DXGIAdapter*> vAdapters;
	TArray<DXGIOutput*> vOutputs;

	while( pFactory->EnumAdapters1( vAdapters.count(), &pAdapter ) != DXGI_ERROR_NOT_FOUND )
	{
		vAdapters.add( new DXGIAdapter( pAdapter ) );

		DXGI_ADAPTER_DESC1 desc;
		pAdapter->GetDesc1( &desc );

		Log::Get().Write( desc.Description );
	}

	// Specify debug
    //UINT CreateDeviceFlags = D3D11_CREATE_DEVICE_SINGLETHREADED; // not interested in multi-threading for now
	UINT CreateDeviceFlags = 0;
#ifdef _DEBUG
    CreateDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	ID3D11DeviceContext* pContext = 0;

	D3D_FEATURE_LEVEL level[] = { FeatureLevel };
	hr = D3D11CreateDevice(
				0, //vAdapters[0]->m_pAdapter,
				DriverType,
				0,
				CreateDeviceFlags,
				level,
				1,
				D3D11_SDK_VERSION,
				&m_pDevice,
				0,
				&pContext
			);

	// Release the adapters and the factory for now...

	for ( int i = 0; i < vAdapters.count(); i++ )
		delete vAdapters[i];

	SAFE_RELEASE( pFactory );

	if( FAILED( hr ) )
        return false;

	// Get the debugger interface from the device.

	m_pDebugger = 0;
	hr = m_pDevice->QueryInterface( __uuidof(ID3D11Debug), (void **)&m_pDebugger );

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
	pImmPipeline->RasterizerStage.DesiredState.SetRasterizerState( CreateRasterizerState( &RasterizerState ) );

	// Depth Stencil State (DS) - the first state will be index zero, so no need
	// to keep a copy of it here.

	DepthStencilStateConfigDX11 DepthStencilState;
	pImmPipeline->OutputMergerStage.DesiredState.SetDepthStencilState( CreateDepthStencilState( &DepthStencilState ) );

	// Output Merger State (OM) - the first state will be index zero, so no need
	// to keep a copy of it here.

	BlendStateConfigDX11 BlendState;
	pImmPipeline->OutputMergerStage.DesiredState.SetBlendState( CreateBlendState( &BlendState ) );

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
		ID3D11DeviceContext* pDeferred = 0;
		m_pDevice->CreateDeferredContext( 0, &pDeferred );

		// Create the pipeline and set the context.
		g_aPayload[i].pPipeline = new PipelineManagerDX11();
		g_aPayload[i].pPipeline->SetDeviceContext( pDeferred, m_FeatureLevel );
		g_aPayload[i].pPipeline->RasterizerStage.DesiredState.SetRasterizerState( 0 );
		g_aPayload[i].pPipeline->OutputMergerStage.DesiredState.SetDepthStencilState( 0 );
		g_aPayload[i].pPipeline->OutputMergerStage.DesiredState.SetBlendState( 0 );


		// Create the command list.
		g_aPayload[i].pList = new CommandListDX11();
		
		// Generate a new parameter manager for each thread.
		g_aPayload[i].pParamManager = new ParameterManagerDX11( i+1 );
		g_aPayload[i].pParamManager->AttachParent( m_pParamMgr );

		// Initialize the payload data variables.
		g_aPayload[i].bComplete = true;
		g_aPayload[i].pRenderView = 0;

		// Create the threads in a suspended state.
		g_aThreadHandles[i] = 0;
		g_aThreadHandles[i] = (HANDLE)_beginthreadex( 0, 0xfffff, _RenderViewThreadProc, &g_aPayload[i], CREATE_SUSPENDED, 0 );

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
	// Shutdown all of the threads
	for ( int i = 0; i < NUM_THREADS; i++ )
	{
		g_aPayload[i].bComplete = true;
		g_aPayload[i].pRenderView = 0;
		
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

	m_vBlendStates.empty();
	m_vDepthStencilStates.empty();
	m_vRasterizerStates.empty();

	for ( int i = 0; i < m_vSamplerStates.count(); i++ )
		delete m_vSamplerStates[i];

	for ( int i = 0; i < m_vInputLayouts.count(); i++ )
		delete m_vInputLayouts[i];

	for ( int i = 0; i < m_vShaders.count(); i++ )
		delete m_vShaders[i];

	for ( int i = 0; i < m_vShaderResourceViews.count(); i++ )
		delete m_vShaderResourceViews[i];

	for ( int i = 0; i < m_vRenderTargetViews.count(); i++ )
		delete m_vRenderTargetViews[i];

	for ( int i = 0; i < m_vDepthStencilViews.count(); i++ )
		delete m_vDepthStencilViews[i];

	for ( int i = 0; i < m_vUnorderedAccessViews.count(); i++ )
		delete m_vUnorderedAccessViews[i];

	for ( int i = 0; i < m_vResources.count(); i++ )
		delete m_vResources[i];

	for ( int i = 0; i < m_vSwapChains.count(); i++ )
		delete m_vSwapChains[i];

	// Clear the context and the device

	SAFE_RELEASE( m_pDevice );
	SAFE_RELEASE( m_pDebugger );
}
//--------------------------------------------------------------------------------
void RendererDX11::Present( HWND hWnd, int SwapChain )
{
	// Present to the window

	if ( ( SwapChain >= 0 ) && ( SwapChain < m_vSwapChains.count() ) ) {
		SwapChainDX11* pSwapChain = m_vSwapChains[SwapChain];
		pSwapChain->m_pSwapChain->Present( 0, 0 );
	}
	else {
		Log::Get().Write( L"Tried to present an invalid swap chain index!" );
	}
}
//--------------------------------------------------------------------------------
int RendererDX11::CreateSwapChain( SwapChainConfigDX11* pConfig )
{
	// Attempt to create the DXGI Factory.

	IDXGIDevice * pDXGIDevice;
	HRESULT hr = m_pDevice->QueryInterface(__uuidof(IDXGIDevice), (void **)&pDXGIDevice);

	IDXGIAdapter * pDXGIAdapter;
	hr = pDXGIDevice->GetParent(__uuidof(IDXGIAdapter), (void **)&pDXGIAdapter);

	IDXGIFactory * pFactory;
	pDXGIAdapter->GetParent(__uuidof(IDXGIFactory), (void **)&pFactory);


	// Attempt to create the swap chain.

	IDXGISwapChain* pSwapChain = 0;
	hr = pFactory->CreateSwapChain( m_pDevice, &pConfig->m_State, &pSwapChain );


	// Release the factory regardless of pass or fail.

	pDXGIDevice->Release();
	pDXGIAdapter->Release();
	pFactory->Release();

	if ( FAILED( hr ) )
	{
		Log::Get().Write( L"Failed to create swap chain!" );
		return( -1 );
	}


	// Acquire the texture interface from the swap chain.

	ID3D11Texture2D* pSwapChainBuffer = 0;
	hr = pSwapChain->GetBuffer( 0, __uuidof( ID3D11Texture2D ), reinterpret_cast< void** >( &pSwapChainBuffer ) );

	if ( FAILED( hr ) )
	{
		Log::Get().Write( L"Failed to get swap chain texture resource!" );
		return( -1 );
	}


	// Add the swap chain's back buffer texture and render target views to the internal data
	// structures to allow setting them later on.

	m_vResources.add( new Texture2dDX11( pSwapChainBuffer ) );


	// If we get here, then we succeeded in creating our swap chain and it's constituent parts.
	// Now we create the wrapper object and store the result in our container.

	int ResourceID = ( m_vResources.count() - 1 ) + RT_TEXTURE2D;
	Texture2dConfigDX11 TextureConfig;
	pSwapChainBuffer->GetDesc( &TextureConfig.m_State );

	ResourcePtr Proxy( new ResourceProxyDX11( ResourceID, &TextureConfig, this ) );
	// With the resource proxy created, create the swap chain wrapper and store it.
	// The resource proxy can then be used later on by the application to get the
	// RTV or texture ID if needed.

	m_vSwapChains.add( new SwapChainDX11( pSwapChain, Proxy ) );

	return( m_vSwapChains.count() - 1 );
}
//--------------------------------------------------------------------------------
ResourcePtr RendererDX11::CreateVertexBuffer( BufferConfigDX11* pConfig,  D3D11_SUBRESOURCE_DATA* pData )
{
	// Create the buffer with the specified configuration.
	ID3D11Buffer* pBuffer = 0;
	HRESULT hr = m_pDevice->CreateBuffer( &pConfig->m_State, pData, &pBuffer );

	if ( pBuffer )
	{
		VertexBufferDX11* pVertexBuffer = new VertexBufferDX11( pBuffer );
		pVertexBuffer->SetDesiredDescription( pConfig->m_State );
		m_vResources.add( pVertexBuffer );

		// Return an index with the lower 16 bits of index, and the upper
		// 16 bits to identify the resource type.

		int ResourceID = ( m_vResources.count() - 1 ) + RT_VERTEXBUFFER;
		ResourcePtr Proxy( new ResourceProxyDX11( ResourceID, pConfig, this ) );

		return( Proxy );
	}

	return( ResourcePtr( new ResourceProxyDX11() ) );
}
//--------------------------------------------------------------------------------
ResourcePtr RendererDX11::CreateIndexBuffer( BufferConfigDX11* pConfig,  D3D11_SUBRESOURCE_DATA* pData )
{
	// Create the buffer with the specified configuration.

	ID3D11Buffer* pBuffer = 0;
	HRESULT hr = m_pDevice->CreateBuffer( &pConfig->m_State, pData, &pBuffer );

	if ( pBuffer )
	{
		IndexBufferDX11* pIndexBuffer = new IndexBufferDX11( pBuffer );
		pIndexBuffer->SetDesiredDescription( pConfig->m_State );
		m_vResources.add( pIndexBuffer );

		// Return an index with the lower 16 bits of index, and the upper
		// 16 bits to identify the resource type.

		int ResourceID = ( m_vResources.count() - 1 ) + RT_INDEXBUFFER;
		ResourcePtr Proxy( new ResourceProxyDX11( ResourceID, pConfig, this ) );

		return( Proxy );
	}

	return( ResourcePtr( new ResourceProxyDX11() ) );
}
//--------------------------------------------------------------------------------
ResourcePtr RendererDX11::CreateStructuredBuffer( BufferConfigDX11* pConfig,  D3D11_SUBRESOURCE_DATA* pData )
{
	// Create the buffer with the specified configuration.

	ID3D11Buffer* pBuffer = 0;
	HRESULT hr = m_pDevice->CreateBuffer( &pConfig->m_State, pData, &pBuffer );

	if ( pBuffer )
	{
		StructuredBufferDX11* pStructuredBuffer = new StructuredBufferDX11( pBuffer );
		pStructuredBuffer->SetDesiredDescription( pConfig->m_State );
		m_vResources.add( pStructuredBuffer );

		// Return an index with the lower 16 bits of index, and the upper
		// 16 bits to identify the resource type.

		int ResourceID = ( m_vResources.count() - 1 ) + RT_STRUCTUREDBUFFER;
		ResourcePtr Proxy( new ResourceProxyDX11( ResourceID, pConfig, this ) );

		return( Proxy );
	}

	return( ResourcePtr( new ResourceProxyDX11() ) );
}
//--------------------------------------------------------------------------------
ResourcePtr RendererDX11::CreateByteAddressBuffer( BufferConfigDX11* pConfig,  D3D11_SUBRESOURCE_DATA* pData )
{
	// Create the buffer with the specified configuration.

	ID3D11Buffer* pBuffer = 0;
	HRESULT hr = m_pDevice->CreateBuffer( &pConfig->m_State, pData, &pBuffer );

	if ( pBuffer )
	{
		ByteAddressBufferDX11* pByteAddressBuffer = new ByteAddressBufferDX11( pBuffer );
		pByteAddressBuffer->SetDesiredDescription( pConfig->m_State );
		m_vResources.add( pByteAddressBuffer );

		// Return an index with the lower 16 bits of index, and the upper
		// 16 bits to identify the resource type.

		int ResourceID = ( m_vResources.count() - 1 ) + RT_BYTEADDRESSBUFFER;
		ResourcePtr Proxy( new ResourceProxyDX11( ResourceID, pConfig, this ) );

		return( Proxy );
	}

	return( ResourcePtr( new ResourceProxyDX11() ) );
}
//--------------------------------------------------------------------------------
ResourcePtr RendererDX11::CreateIndirectArgsBuffer( BufferConfigDX11* pConfig,  D3D11_SUBRESOURCE_DATA* pData )
{
	// Create the buffer with the specified configuration.

	ID3D11Buffer* pBuffer = 0;
	HRESULT hr = m_pDevice->CreateBuffer( &pConfig->m_State, pData, &pBuffer );

	if ( pBuffer )
	{
		ByteAddressBufferDX11* pByteAddressBuffer = new ByteAddressBufferDX11( pBuffer );
		pByteAddressBuffer->SetDesiredDescription( pConfig->m_State );
		m_vResources.add( pByteAddressBuffer );

		// Return an index with the lower 16 bits of index, and the upper
		// 16 bits to identify the resource type.

		int ResourceID = ( m_vResources.count() - 1 ) + RT_INDIRECTARGSBUFFER;
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

	ID3D11Buffer* pBuffer = 0;
	HRESULT hr = m_pDevice->CreateBuffer( &pConfig->m_State, pData, &pBuffer );

	if ( pBuffer )
	{
		ConstantBufferDX11* pConstantBuffer = new ConstantBufferDX11( pBuffer );
		pConstantBuffer->SetDesiredDescription( pConfig->m_State );
		pConstantBuffer->SetAutoUpdate( bAutoUpdate );
		m_vResources.add( pConstantBuffer );

		// Return an index with the lower 16 bits of index, and the upper
		// 16 bits to identify the resource type.
		int ResourceID = ( m_vResources.count() - 1 ) + RT_CONSTANTBUFFER;
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
	ID3D11Texture1D* pTexture = 0;
	HRESULT hr = m_pDevice->CreateTexture1D( &pConfig->m_State, pData, &pTexture );

	if ( pTexture )
	{
		Texture1dDX11* pTex = new Texture1dDX11( pTexture );
		pTex->SetDesiredDescription( pConfig->GetTextureDesc() );
		m_vResources.add( pTex );

		// Return an index with the lower 16 bits of index, and the upper
		// 16 bits to identify the resource type.
		int ResourceID = ( m_vResources.count() - 1 ) + RT_TEXTURE1D;
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
	ID3D11Texture2D* pTexture = 0;
	HRESULT hr = m_pDevice->CreateTexture2D( &pConfig->m_State, pData, &pTexture );

	if ( pTexture )
	{
		Texture2dDX11* pTex = new Texture2dDX11( pTexture );
		pTex->SetDesiredDescription( pConfig->GetTextureDesc() );
		m_vResources.add( pTex );

		// Return an index with the lower 16 bits of index, and the upper
		// 16 bits to identify the resource type.
		int ResourceID = ( m_vResources.count() - 1 ) + RT_TEXTURE2D;
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
	ID3D11Texture3D* pTexture = 0;
	HRESULT hr = m_pDevice->CreateTexture3D( &pConfig->m_State, pData, &pTexture );

	if ( pTexture )
	{
		Texture3dDX11* pTex = new Texture3dDX11( pTexture );
		pTex->SetDesiredDescription( pConfig->GetTextureDesc() );
		m_vResources.add( pTex );

		// Return an index with the lower 16 bits of index, and the upper
		// 16 bits to identify the resource type.
		int ResourceID = ( m_vResources.count() - 1 ) + RT_TEXTURE3D;
		ResourcePtr Proxy( new ResourceProxyDX11( ResourceID, pConfig, this, pSRVConfig, pRTVConfig, pUAVConfig ) );

		return( Proxy );
	}

	return( ResourcePtr( new ResourceProxyDX11() ) );
}
//--------------------------------------------------------------------------------
int RendererDX11::CreateShaderResourceView( int ResourceID, D3D11_SHADER_RESOURCE_VIEW_DESC* pDesc )
{
	int TYPE = ResourceID & 0xFF0000;
	int ID = ResourceID & 0xFFFF;
	ID3D11Resource* pResource = 0;

	// Check if this ID is in range
	if ( m_vResources.inrange( ID ) )
		pResource = m_vResources[ID]->GetResource();

	if ( pResource )
	{
		ID3D11ShaderResourceView* pView = 0;
		HRESULT hr = m_pDevice->CreateShaderResourceView( pResource, pDesc, &pView );

		if ( pView )
		{
			ShaderResourceViewDX11* pShaderResource = new ShaderResourceViewDX11( pView );
			m_vShaderResourceViews.add( pShaderResource );

			return( m_vShaderResourceViews.count() - 1 );
		}
	}

	return( -1 );
}
//--------------------------------------------------------------------------------
int RendererDX11::CreateRenderTargetView( int ResourceID, D3D11_RENDER_TARGET_VIEW_DESC* pDesc )
{
	int TYPE = ResourceID & 0xFF0000;
	int ID = ResourceID & 0xFFFF;
	ID3D11Resource* pResource = 0;

	// Check if this ID is in range
	if ( m_vResources.inrange( ID ) )
		pResource = m_vResources[ID]->GetResource();

	if ( pResource )
	{
		ID3D11RenderTargetView* pView = 0;
		HRESULT hr = m_pDevice->CreateRenderTargetView( pResource, pDesc, &pView );

		if ( pView )
		{
			RenderTargetViewDX11* pRenderTarget = new RenderTargetViewDX11( pView );
			m_vRenderTargetViews.add( pRenderTarget );

			return( m_vRenderTargetViews.count() - 1 );
		}
	}

	return( -1 );
}
//--------------------------------------------------------------------------------
int RendererDX11::CreateDepthStencilView( int ResourceID, D3D11_DEPTH_STENCIL_VIEW_DESC* pDesc )
{
	int TYPE = ResourceID & 0x00FF0000;
	int ID = ResourceID & 0x0000FFFF;
	ID3D11Resource* pResource = 0;

	// Check if this ID is in range
	if ( m_vResources.inrange( ID ) )
		pResource = m_vResources[ID]->GetResource();

	if ( pResource )
	{
		ID3D11DepthStencilView* pView = 0;
		HRESULT hr = m_pDevice->CreateDepthStencilView( pResource, pDesc, &pView );

		if ( pView )
		{
			DepthStencilViewDX11* pDepthStencil = new DepthStencilViewDX11( pView );
			m_vDepthStencilViews.add( pDepthStencil );

			return( m_vDepthStencilViews.count() - 1 );
		}
	}

	return( -1 );
}
//--------------------------------------------------------------------------------
int RendererDX11::CreateUnorderedAccessView( int ResourceID, D3D11_UNORDERED_ACCESS_VIEW_DESC* pDesc )
{
	int TYPE = ResourceID & 0xFF0000;
	int ID = ResourceID & 0xFFFF;
	ID3D11Resource* pResource = 0;

	// Check if this ID is in range
	if ( m_vResources.inrange( ID ) )
		pResource = m_vResources[ID]->GetResource();

	if ( pResource )
	{
		ID3D11UnorderedAccessView* pView = 0;
		HRESULT hr = m_pDevice->CreateUnorderedAccessView( pResource, pDesc, &pView );

		if ( pView )
		{
			UnorderedAccessViewDX11* pUnorderedAccess = new UnorderedAccessViewDX11( pView );
			m_vUnorderedAccessViews.add( pUnorderedAccess );

			return( m_vUnorderedAccessViews.count() - 1 );
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
	Texture2dDX11* pOldTexture = GetTexture2DByIndex( rid );
	D3D11_TEXTURE2D_DESC TexDesc = pOldTexture->GetActualDescription();
	TexDesc.Width = width;
	TexDesc.Height = height;

	// Create the new texture...
	ID3D11Texture2D* pTexture = 0;
	HRESULT hr = m_pDevice->CreateTexture2D( &TexDesc, 0, &pTexture );
	
	// Release the old texture, and replace it with the new one.
	pOldTexture->m_pTexture->Release(); 
	pOldTexture->m_pTexture = pTexture;

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
	if ( SRVID == -1 ) {
		return;
	}

	int TYPE = RID & 0xFF0000;
	int ID = RID & 0xFFFF;

	// Check that the input resources / views are legit.
	if ( !m_vResources.inrange( ID ) || !m_vShaderResourceViews.inrange( SRVID ) || (TYPE != RT_TEXTURE2D ) ) {
		Log::Get().Write( L"Error trying to resize a SRV!!!!" );
		return;
	}

	// Get the existing UAV.
	ShaderResourceViewDX11* pOldSRV = m_vShaderResourceViews[SRVID];

	// Get its description.
	D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc;
	pOldSRV->m_pShaderResourceView->GetDesc( &SRVDesc );
	
	// Create the new one.
	ID3D11ShaderResourceView* pView = 0;
	HRESULT hr = m_pDevice->CreateShaderResourceView( m_vResources[ID]->GetResource(), &SRVDesc, &pView );

	// Release the old one and replace it with the new one.
	pOldSRV->m_pShaderResourceView->Release();
	pOldSRV->m_pShaderResourceView = pView;
}
//--------------------------------------------------------------------------------
void RendererDX11::ResizeTextureRTV( int RID, int RTVID, UINT width, UINT height )
{
	// Check to make sure we are supposed to do anything...
	if ( RTVID == -1 ) {
		return;
	}

	int TYPE = RID & 0xFF0000;
	int ID = RID & 0xFFFF;

	// Check that the input resources / views are legit.
	if ( !m_vResources.inrange( ID ) || !m_vRenderTargetViews.inrange( RTVID ) || (TYPE != RT_TEXTURE2D ) ) {
		Log::Get().Write( L"Error trying to resize a RTV!!!!" );
		return;
	}

	// Get the existing UAV.
	RenderTargetViewDX11* pOldRTV = m_vRenderTargetViews[RTVID];

	// Get its description.
	D3D11_RENDER_TARGET_VIEW_DESC RTVDesc;
	pOldRTV->m_pRenderTargetView->GetDesc( &RTVDesc );
	
	// Create the new one.
	ID3D11RenderTargetView* pView = 0;
	HRESULT hr = m_pDevice->CreateRenderTargetView( m_vResources[ID]->GetResource(), &RTVDesc, &pView );

	// Release the old one and replace it with the new one.
	pOldRTV->m_pRenderTargetView->Release();
	pOldRTV->m_pRenderTargetView = pView;
}
//--------------------------------------------------------------------------------
void RendererDX11::ResizeTextureDSV( int RID, int DSVID, UINT width, UINT height )
{
	// Check to make sure we are supposed to do anything...
	if ( DSVID == -1 ) {
		return;
	}

	int TYPE = RID & 0xFF0000;
	int ID = RID & 0xFFFF;

	// Check that the input resources / views are legit.
	if ( !m_vResources.inrange( ID ) || !m_vDepthStencilViews.inrange( DSVID ) || (TYPE != RT_TEXTURE2D ) ) {
		Log::Get().Write( L"Error trying to resize a DSV!!!!" );
		return;
	}

	// Get the existing UAV.
	DepthStencilViewDX11* pOldDSV = m_vDepthStencilViews[DSVID];

	// Get its description.
	D3D11_DEPTH_STENCIL_VIEW_DESC DSVDesc;
	pOldDSV->m_pDepthStencilView->GetDesc( &DSVDesc );
	
	// Create the new one.
	ID3D11DepthStencilView* pView = 0;
	HRESULT hr = m_pDevice->CreateDepthStencilView( m_vResources[ID]->GetResource(), &DSVDesc, &pView );

	// Release the old one and replace it with the new one.
	pOldDSV->m_pDepthStencilView->Release();
	pOldDSV->m_pDepthStencilView = pView;
}
//--------------------------------------------------------------------------------
void RendererDX11::ResizeTextureUAV( int RID, int UAVID, UINT width, UINT height )
{
	// Check to make sure we are supposed to do anything...
	if ( UAVID == -1 ) {
		return;
	}

	int TYPE = RID & 0xFF0000;
	int ID = RID & 0xFFFF;

	// Check that the input resources / views are legit.
	if ( !m_vResources.inrange( ID ) || !m_vUnorderedAccessViews.inrange( UAVID ) || (TYPE != RT_TEXTURE2D ) ) {
		Log::Get().Write( L"Error trying to resize a UAV!!!!" );
		return;
	}

	// Get the existing UAV.
	UnorderedAccessViewDX11* pOldUAV = m_vUnorderedAccessViews[UAVID];

	// Get its description.
	D3D11_UNORDERED_ACCESS_VIEW_DESC UAVDesc;
	pOldUAV->m_pUnorderedAccessView->GetDesc( &UAVDesc );
	
	// Create the new one.
	ID3D11UnorderedAccessView* pView = 0;
	HRESULT hr = m_pDevice->CreateUnorderedAccessView( m_vResources[ID]->GetResource(), &UAVDesc, &pView );

	// Release the old one and replace it with the new one.
	pOldUAV->m_pUnorderedAccessView->Release();
	pOldUAV->m_pUnorderedAccessView = pView;
}
//--------------------------------------------------------------------------------
void RendererDX11::ResizeSwapChain( int SID, UINT width, UINT height )
{
	if ( !m_vSwapChains.inrange( SID ) ) {
		Log::Get().Write( L"Error trying to resize swap chain!" );
		return;
	}

	SwapChainDX11* pSwapChain = m_vSwapChains[SID];


	Texture2dDX11* pBackBuffer = GetTexture2DByIndex( pSwapChain->m_Resource->m_iResource );
	pBackBuffer->m_pTexture->Release();

	RenderTargetViewDX11* pRTV = m_vRenderTargetViews[pSwapChain->m_Resource->m_iResourceRTV];
	


	// Get its description.
	D3D11_RENDER_TARGET_VIEW_DESC RTVDesc;
	pRTV->m_pRenderTargetView->GetDesc( &RTVDesc );
	
	pRTV->m_pRenderTargetView->Release();



	this->pImmPipeline->ClearPipelineState();
	
	for ( int i = 0; i < NUM_THREADS; i++ ) {
		g_aPayload[i].pPipeline->ClearPipelineState();
	}

	// Resize the buffers.
	HRESULT hr = pSwapChain->m_pSwapChain->ResizeBuffers( 2, width, height, DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, 0 );

	if ( FAILED(hr) ) {
		Log::Get().Write( L"Failed to resize buffers!" );
	}

	// Re-acquire the back buffer reference.
	ID3D11Texture2D* pSwapChainBuffer = 0;
	hr = pSwapChain->m_pSwapChain->GetBuffer( 0, __uuidof( ID3D11Texture2D ), 
		reinterpret_cast< void** >( &pSwapChainBuffer ) );

	pBackBuffer->m_pTexture = pSwapChainBuffer;


	// Create the new one.
	ID3D11RenderTargetView* pView = 0;
	hr = m_pDevice->CreateRenderTargetView( pSwapChainBuffer, &RTVDesc, &pView );

	// Release the old one and replace it with the new one.
	pRTV->m_pRenderTargetView = pView;

}
//--------------------------------------------------------------------------------
void RendererDX11::ResizeViewport( int ID, UINT width, UINT height )
{
	if ( !m_vViewPorts.inrange( ID ) ) {
		Log::Get().Write( L"Error trying to resize viewport!" );
	}

	ViewPortDX11* pViewport = m_vViewPorts[ID];
	pViewport->m_ViewPort.Width = static_cast<float>( width );
	pViewport->m_ViewPort.Height = static_cast<float>( height );
}
//--------------------------------------------------------------------------------
int RendererDX11::LoadShader( ShaderType type, std::wstring& filename, std::wstring& function, 
                             std::wstring& model, bool enablelogging )
{
    return LoadShader( type, filename, function, model, NULL, enablelogging );
}
//--------------------------------------------------------------------------------
int RendererDX11::LoadShader( ShaderType type, std::wstring& filename, std::wstring& function, 
                                std::wstring& model, const D3D10_SHADER_MACRO* pDefines, bool enablelogging )
{
	HRESULT hr = S_OK;

	ID3DBlob* pCompiledShader = NULL;
	ID3DBlob* pErrorMessages = NULL;

	char AsciiFunction[1024];
	char AsciiModel[1024];
	WideCharToMultiByte(CP_ACP, 0, function.c_str(), -1, AsciiFunction, 1024, NULL, NULL);
	WideCharToMultiByte(CP_ACP, 0, model.c_str(), -1, AsciiModel, 1024, NULL, NULL);

    UINT flags = D3D10_SHADER_PACK_MATRIX_ROW_MAJOR;
#ifdef _DEBUG
    flags |= D3D10_SHADER_DEBUG | D3D10_SHADER_SKIP_OPTIMIZATION | D3D10_SHADER_WARNINGS_ARE_ERRORS;
#endif

	// Get the current path to the shader folders, and add the filename to it.

	FileSystem fs;
	filename = fs.GetShaderFolder() + filename;

	if ( FAILED( hr = D3DX11CompileFromFile(
		filename.c_str(),
		pDefines,
		0,
		AsciiFunction,
		AsciiModel,
		flags,
		0,//UINT Flags2,
		0,
		&pCompiledShader,
		&pErrorMessages,
		&hr
		) ) )
	{

		std::wstringstream message;

		message << L"Error compiling shader program: " << filename << std::endl << std::endl;
		message << L"The following error was reported:" << std::endl;

		if ( ( enablelogging ) && ( pErrorMessages != 0 ) )
		{
			LPVOID pCompileErrors = pErrorMessages->GetBufferPointer();
			const char* pMessage = (const char*)pCompileErrors;
			message << GlyphString::ToUnicode( std::string( pMessage ) );
			Log::Get().Write( message.str() );
		}

		EventManager::Get()->ProcessEvent( new EvtErrorMessage( message.str() ) );

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

	m_vShaders.add( pShaderWrapper );


	// Create the reflection interface to query information about the shader.  Note that
	// this reflection interface does not depend on the shader type!

	ID3D11ShaderReflection* pReflector = NULL;
	hr = D3DReflect( pCompiledShader->GetBufferPointer(), pCompiledShader->GetBufferSize(),
		IID_ID3D11ShaderReflection, (void**) &pReflector);

	if ( FAILED( hr ) )
	{
		Log::Get().Write( L"Failed to create shader reflection interface!" );
		return( -1 );
	}


	// Get the top level shader information, including the number of constant buffers,
	// as well as the number bound resources (constant buffers + other objects), the
	// number of input elements, and the number of output elements for the shader.

	D3D11_SHADER_DESC desc;
	pReflector->GetDesc( &desc );
	pShaderWrapper->ShaderDescription = desc;


	// Get the input and output signature description arrays.  These can be used later
	// for verification of linking shader stages together.
	for ( UINT i = 0; i < desc.InputParameters; i++ )
	{
		D3D11_SIGNATURE_PARAMETER_DESC input_desc;
		pReflector->GetInputParameterDesc( i, &input_desc );
		SignatureParameterDesc d(input_desc);
		pShaderWrapper->InputSignatureParameters.add( d );
	}
	for ( UINT i = 0; i < desc.OutputParameters; i++ )
	{
		D3D11_SIGNATURE_PARAMETER_DESC output_desc;
		pReflector->GetOutputParameterDesc( i, &output_desc );
		SignatureParameterDesc d(output_desc);
		pShaderWrapper->OutputSignatureParameters.add( d );
	}


	// Get the constant buffer information, which will be used for setting parameters
	// for use by this shader at rendering time.

	for ( UINT i = 0; i < desc.ConstantBuffers; i++ )
	{
		ID3D11ShaderReflectionConstantBuffer* pConstBuffer = pReflector->GetConstantBufferByIndex( i );
		
		D3D11_SHADER_BUFFER_DESC bufferDesc;
		pConstBuffer->GetDesc( &bufferDesc );
		
		if ( bufferDesc.Type == D3D_CT_CBUFFER || bufferDesc.Type == D3D_CT_TBUFFER )
		{
			ConstantBufferLayout BufferLayout;
			BufferLayout.Description = ShaderBufferDesc( bufferDesc );
			BufferLayout.pParamRef = m_pParamMgr->GetConstantBufferParameterRef( BufferLayout.Description.Name );

			// Load the description of each variable for use later on when binding a buffer
			for ( UINT j = 0; j < BufferLayout.Description.Variables; j++ )
			{
				// Get the variable description and store it
				ID3D11ShaderReflectionVariable* pVariable = pConstBuffer->GetVariableByIndex( j );
				D3D11_SHADER_VARIABLE_DESC var_desc;
				pVariable->GetDesc( &var_desc );
				ShaderVariableDesc variabledesc( var_desc );

				BufferLayout.Variables.add( variabledesc );

				// Get the variable type description and store it
				ID3D11ShaderReflectionType* pType = pVariable->GetType();
				D3D11_SHADER_TYPE_DESC type_desc;
				pType->GetDesc( &type_desc );
				ShaderTypeDesc typedesc( type_desc );

				BufferLayout.Types.add( typedesc );

				// Get references to the parameters for binding to these variables.
				RenderParameterDX11* pParam = 0;
				if ( typedesc.Class == D3D10_SVC_VECTOR )
				{
					pParam = m_pParamMgr->GetVectorParameterRef( variabledesc.Name );
				}
				else if ( ( typedesc.Class == D3D10_SVC_MATRIX_ROWS ) ||
							( typedesc.Class == D3D10_SVC_MATRIX_COLUMNS ) )
				{
					// Check if it is an array of matrices first...
					unsigned int count = typedesc.Elements;
					if ( count == 0 ) 
					{
						pParam = m_pParamMgr->GetMatrixParameterRef( variabledesc.Name );
					}
					else
					{
						pParam = m_pParamMgr->GetMatrixArrayParameterRef( variabledesc.Name, count );
					}
				}

				BufferLayout.Parameters.add( pParam );
			}

			pShaderWrapper->ConstantBuffers.add( BufferLayout );
		}
	}


	// Get the overall resource binding information for this shader.  This includes
	// the constant buffers, plus all of the other objects that can be bound to the
	// pipeline with resource views (shader resource views and unordered access views).

	for ( UINT i = 0; i < desc.BoundResources; i++ )
	{
		D3D11_SHADER_INPUT_BIND_DESC resource_desc;
		pReflector->GetResourceBindingDesc( i, &resource_desc );
		ShaderInputBindDesc binddesc( resource_desc );

		if ( resource_desc.Type == D3D10_SIT_CBUFFER || resource_desc.Type == D3D10_SIT_TBUFFER )
		{
			binddesc.pParamRef = m_pParamMgr->GetConstantBufferParameterRef( binddesc.Name );
		}
		else if ( resource_desc.Type == D3D10_SIT_TEXTURE || resource_desc.Type == D3D11_SIT_STRUCTURED )
		{
			binddesc.pParamRef = m_pParamMgr->GetShaderResourceParameterRef( binddesc.Name );
		}
		else if ( resource_desc.Type == D3D10_SIT_SAMPLER )
		{
			binddesc.pParamRef = m_pParamMgr->GetSamplerStateParameterRef( binddesc.Name );
		}
		else if ( resource_desc.Type == D3D11_SIT_UAV_RWTYPED || resource_desc.Type == D3D11_SIT_UAV_RWSTRUCTURED
			|| resource_desc.Type == D3D11_SIT_BYTEADDRESS || resource_desc.Type == D3D11_SIT_UAV_RWBYTEADDRESS
			|| resource_desc.Type == D3D11_SIT_UAV_APPEND_STRUCTURED || resource_desc.Type == D3D11_SIT_UAV_CONSUME_STRUCTURED
			|| resource_desc.Type == D3D11_SIT_UAV_RWSTRUCTURED_WITH_COUNTER )
		{
			binddesc.pParamRef = m_pParamMgr->GetUnorderedAccessParameterRef( binddesc.Name );
		}


		pShaderWrapper->ResourceBindings.add( binddesc );
	}


	// Release the shader reflection interface
	pReflector->Release();

	// Store the compiled shader in the shader wrapper for use later on in creating
	// and checking input and output signatures.
	pShaderWrapper->pCompiledShader = pCompiledShader;



	// Initialize the constant buffers of this shader, so that they aren't 
	// lazy created later on...

	pShaderWrapper->InitializeConstantBuffers( m_pParamMgr );

	//pShaderWrapper->PrintShaderDetails();


	// Return the index for future referencing.

	return( m_vShaders.count() - 1 );

}
//--------------------------------------------------------------------------------
const char* RendererDX11::TranslateShaderVariableClass( D3D10_SHADER_VARIABLE_CLASS varclass )
{
	switch( varclass )
	{
	case D3D10_SVC_SCALAR:
		return( "SCALAR" );
	case D3D10_SVC_VECTOR:
		return( "VECTOR" );
	case D3D10_SVC_MATRIX_ROWS:
		return( "MATRIX_ROWS" );
	case D3D10_SVC_MATRIX_COLUMNS:
		return( "MATRIX_COLUMNS" );
	case D3D10_SVC_OBJECT:
		return( "OBJECT" );
	case D3D10_SVC_STRUCT:
		return( "STRUCT" );
	case D3D11_SVC_INTERFACE_CLASS:
		return( "INTERFACE_CLASS" );
	case D3D11_SVC_INTERFACE_POINTER:
		return( "INTERFACE_POINTER" );
	}

	return( "UNKNOWN_CLASS" );
}
//--------------------------------------------------------------------------------
const char* RendererDX11::TranslateShaderVariableType( D3D10_SHADER_VARIABLE_TYPE vartype )
{
	switch( vartype )
	{
	case D3D10_SVT_VOID:
		return( "VOID" );
	case D3D10_SVT_BOOL:
		return( "BOOL" );
	case D3D10_SVT_INT:
		return( "INT" );
    case D3D10_SVT_FLOAT:
		return( "FLOAT" );
    case D3D10_SVT_STRING:
		return( "STRING" );
    case D3D10_SVT_TEXTURE:
		return( "TEXTURE" );
    case D3D10_SVT_TEXTURE1D:
		return( "TEXTURE_1D" );
    case D3D10_SVT_TEXTURE2D:
		return( "TEXTURE_2D" );
    case D3D10_SVT_TEXTURE3D:
		return( "TEXTURE_3D" );
    case D3D10_SVT_TEXTURECUBE:
		return( "TEXTURE_CUBE" );
    case D3D10_SVT_SAMPLER:
		return( "SAMPLER" );
    case D3D10_SVT_PIXELSHADER:
		return( "PIXEL_SHADER" );
    case D3D10_SVT_VERTEXSHADER:
		return( "VERTEX_SHADER" );
    case D3D10_SVT_UINT:
		return( "UINT" );
    case D3D10_SVT_UINT8:
		return( "UINT8" );
    case D3D10_SVT_GEOMETRYSHADER:
		return( "GEOMETRY_SHADER" );
    case D3D10_SVT_RASTERIZER:
		return( "RASTERIZER" );
    case D3D10_SVT_DEPTHSTENCIL:
		return( "DEPTH_STENCIL" );
    case D3D10_SVT_BLEND:
		return( "BLEND" );
    case D3D10_SVT_BUFFER:
		return( "BUFFER" );
    case D3D10_SVT_CBUFFER:
		return( "CBUFFER" );
    case D3D10_SVT_TBUFFER:
		return( "TBUFFER" );
    case D3D10_SVT_TEXTURE1DARRAY:
		return( "TEXTURE_1D_ARRAY" );
    case D3D10_SVT_TEXTURE2DARRAY:
		return( "TEXTURE_2D_ARRAY" );
    case D3D10_SVT_RENDERTARGETVIEW:
		return( "RENDER_TARGET_VIEW" );
    case D3D10_SVT_DEPTHSTENCILVIEW:
		return( "DEPTH_STENCIL_VIEW" );
    case D3D10_SVT_TEXTURE2DMS:
		return( "TEXTURE_2D_MS" );
    case D3D10_SVT_TEXTURE2DMSARRAY:
		return( "TEXTURE_2D_MS_ARRAY" );
    case D3D10_SVT_TEXTURECUBEARRAY:
		return( "TEXTURE_CUBE_ARRAY" );
    case D3D11_SVT_HULLSHADER:
		return( "HULL_SHADER" );
    case D3D11_SVT_DOMAINSHADER:
		return( "DOMAIN_SHADER" );
    case D3D11_SVT_INTERFACE_POINTER:
		return( "INTERFACE_POINTER" );
    case D3D11_SVT_COMPUTESHADER:
		return( "COMPUTE_SHADER" );
    case D3D11_SVT_DOUBLE:
		return( "DOUBLE" );
	}

	return( "UNKNOWN_TYPE" );

}
//--------------------------------------------------------------------------------
const char* RendererDX11::TranslateSystemVariableName( D3D10_NAME name )
{
	switch( name )
	{
	case D3D10_NAME_UNDEFINED:
		return( "UNDEFINED" );
	case D3D10_NAME_POSITION:
		return( "POSITION" );
	case D3D10_NAME_CLIP_DISTANCE:
		return( "CLIP_DISTANCE" );
	case D3D10_NAME_CULL_DISTANCE:
		return( "CULL_DISTANCE" );
	case D3D10_NAME_RENDER_TARGET_ARRAY_INDEX:
		return( "RENDER_TARGET_ARRAY_INDEX" );
	case D3D10_NAME_VIEWPORT_ARRAY_INDEX:
		return( "VIEWPORT_ARRAY_INDEX" );
	case D3D10_NAME_VERTEX_ID:
		return( "VERTEX_ID" );
	case D3D10_NAME_PRIMITIVE_ID:
		return( "PRIMITIVE_ID" );
	case D3D10_NAME_INSTANCE_ID:
		return( "INSTANCE_ID" );
	case D3D10_NAME_IS_FRONT_FACE:
		return( "IS_FRONT_FACE" );
	case D3D10_NAME_SAMPLE_INDEX:
		return( "SAMPLE_INDEX" );
	case D3D11_NAME_FINAL_QUAD_EDGE_TESSFACTOR:
		return( "FINAL_QUAD_EDGE_TESSFACTOR" );
	case D3D11_NAME_FINAL_QUAD_INSIDE_TESSFACTOR:
		return( "FINAL_QUAD_INSIDE_TESSFACTOR" );
	case D3D11_NAME_FINAL_TRI_EDGE_TESSFACTOR:
		return( "FINAL_TRI_EDGE_TESSFACTOR" );
	case D3D11_NAME_FINAL_TRI_INSIDE_TESSFACTOR:
		return( "FINAL_TRI_INSIDE_TESSFACTOR" );
	case D3D11_NAME_FINAL_LINE_DETAIL_TESSFACTOR:
		return( "FINAL_LINE_DETAIL_TESSFACTOR" );
	case D3D11_NAME_FINAL_LINE_DENSITY_TESSFACTOR:
		return( "FINAL_LINE_DENSITY_TESSFACTOR" );
	case D3D10_NAME_TARGET:
		return( "TARGET" );
	case D3D10_NAME_DEPTH:
		return( "DEPTH" );
	case D3D10_NAME_COVERAGE:
		return( "COVERAGE" );
	case D3D11_NAME_DEPTH_GREATER_EQUAL:
		return( "DEPTH_GREATER_EQUAL" );
	case D3D11_NAME_DEPTH_LESS_EQUAL:
		return( "DEPTH_LESS_EQUAL" );
	}

	return( "UNKNOWN_SYSTEM_VARIABLE" );

}
//--------------------------------------------------------------------------------
const char* RendererDX11::TranslateRegisterComponentType( D3D10_REGISTER_COMPONENT_TYPE type )
{
	switch( type )
	{
	case D3D10_REGISTER_COMPONENT_UNKNOWN:
		return( "UNKNOWN" );
	case D3D10_REGISTER_COMPONENT_UINT32:
		return( "UINT32" );
	case D3D10_REGISTER_COMPONENT_SINT32:
		return( "SINT32" );
	case D3D10_REGISTER_COMPONENT_FLOAT32:
		return( "FLOAT32" );
	}

	return( "UNKNOWN_REGISTER_COMPONENT_TYPE" );
}
//--------------------------------------------------------------------------------
const char* RendererDX11::TranslateCBufferType( D3D11_CBUFFER_TYPE type )
{
	switch( type )
	{
	case D3D11_CT_CBUFFER:
		return( "CBUFFER" );
	case D3D11_CT_TBUFFER:
		return( "TBUFFER" );
	case D3D11_CT_INTERFACE_POINTERS:
		return( "INTERFACE_POINTERS" );
	case D3D11_CT_RESOURCE_BIND_INFO:
		return( "RESOURCE_BIND_INFO" );
	}

	return( "UNKOWN_CBUFFER_TYPE" );
}
//--------------------------------------------------------------------------------
const char* RendererDX11::TranslateShaderInputType( D3D10_SHADER_INPUT_TYPE type )
{
	switch( type )
	{
	case D3D10_SIT_CBUFFER:
		return( "CBUFFER" );
	case D3D10_SIT_TBUFFER:
		return( "TBUFFER" );
	case D3D10_SIT_TEXTURE:
		return( "TEXTURE" );
	case D3D10_SIT_SAMPLER:
		return( "SAMPLER" );
	case D3D11_SIT_UAV_RWTYPED:
		return( "UAV_RW_TYPED" );
	case D3D11_SIT_STRUCTURED:
		return( "STRUCTURED" );
	case D3D11_SIT_UAV_RWSTRUCTURED:
		return( "UAV_RW_STRUCTURED" );
	case D3D11_SIT_BYTEADDRESS:
		return( "UAV_BYTE_ADDRESS" );
	case D3D11_SIT_UAV_RWBYTEADDRESS:
		return( "UAV_RW_BYTE_ADDRESS" );
//	case D3D11_SIT_UAV_APPEND_BYTEADDRESS:
//		return( "UAV_APPEND_BYTE_ADDRESS" );
//	case D3D11_SIT_UAV_CONSUME_BYTEADDRESS:
//		return( "UAV_CONSUME_BYTE_ADDRESS" );
	case D3D11_SIT_UAV_APPEND_STRUCTURED:
		return( "UAV_APPEND_STRUCTURED" );
	case D3D11_SIT_UAV_CONSUME_STRUCTURED:
		return( "UAV_CONSUME_STRUCTURED" );
	}

	return( "UNKOWN_SHADER_INPUT_TYPE" );
}
//--------------------------------------------------------------------------------
const char* RendererDX11::TranslateResourceReturnType( D3D11_RESOURCE_RETURN_TYPE type )
{
	switch( type )
	{
	case D3D11_RETURN_TYPE_UNORM:
		return( "UNORM" );
	case D3D11_RETURN_TYPE_SNORM:
		return( "SNORM" );
	case D3D11_RETURN_TYPE_SINT:
		return( "SINT" );
	case D3D11_RETURN_TYPE_UINT:
		return( "UINT" );
	case D3D11_RETURN_TYPE_FLOAT:
		return( "FLOAT" );
	case D3D11_RETURN_TYPE_MIXED:
		return( "MIXED" );
	case D3D11_RETURN_TYPE_DOUBLE:
		return( "DOUBLE" );
	case D3D11_RETURN_TYPE_CONTINUED:
		return( "CONTINUED" );
	}

	return( "UNKOWN_RESOURCE_RETURN_TYPE" );
}
//--------------------------------------------------------------------------------
int RendererDX11::CreateInputLayout( TArray<D3D11_INPUT_ELEMENT_DESC>& elements, int ShaderID  )
{
	// Create array of elements here for the API call.
	D3D11_INPUT_ELEMENT_DESC* pElements = new D3D11_INPUT_ELEMENT_DESC[elements.count()];
	for ( int i = 0; i < elements.count(); i++ )
		pElements[i] = elements[i];
	
	// Attempt to create the input layout from the input information.
	ID3DBlob* pCompiledShader = m_vShaders[ShaderID]->pCompiledShader;
	ID3D11InputLayout* pLayout = 0;

	HRESULT hr = m_pDevice->CreateInputLayout( pElements, elements.count(), 
		pCompiledShader->GetBufferPointer(), pCompiledShader->GetBufferSize(), &pLayout );

	// Release the input elements array.
	delete[] pElements;

	// On failure, log the error and return an invalid index.
	if ( FAILED( hr ) )
	{
		Log::Get().Write( L"Failed to create input layout!" );
		return( -1 );
	}

	// Create the input layout wrapper instance and store it in the renderer's list.
	InputLayoutDX11* pLayoutWrapper = new InputLayoutDX11( pLayout );
	m_vInputLayouts.add( pLayoutWrapper );
	int index = m_vInputLayouts.count() - 1;

	// Return the index for referencing later on.
	return( index );
}
//--------------------------------------------------------------------------------
ResourcePtr RendererDX11::LoadTexture( std::wstring filename, D3DX11_IMAGE_LOAD_INFO* pLoadInfo )
{
	ID3D11Resource* pTexture = 0;

	FileSystem fs;
	filename = fs.GetTextureFolder() + filename;

	HRESULT hr = D3DX11CreateTextureFromFile(
		m_pDevice,
		filename.c_str(),
		pLoadInfo,
		0,
		&pTexture,
		0
	);

	if ( FAILED( hr ) )
	{
		Log::Get().Write( L"Failed to load texture from file!" );
		return( ResourcePtr( new ResourceProxyDX11() ) );
	}

	m_vResources.add( new Texture2dDX11( reinterpret_cast< ID3D11Texture2D* >( pTexture ) ) );

	int ResourceID = ( m_vResources.count() - 1 ) + RT_TEXTURE2D;
	Texture2dConfigDX11 TextureConfig;
	reinterpret_cast< ID3D11Texture2D* >( pTexture )->GetDesc( &TextureConfig.m_State );

	return( ResourcePtr( new ResourceProxyDX11( ResourceID, &TextureConfig, this ) ) );
}
//--------------------------------------------------------------------------------
ResourcePtr RendererDX11::LoadTexture( void* pData, SIZE_T sizeInBytes, D3DX11_IMAGE_LOAD_INFO* pLoadInfo )
{
	ID3D11Resource* pTexture = 0;

	HRESULT hr = D3DX11CreateTextureFromMemory(
		m_pDevice,
		pData,
		sizeInBytes,
		pLoadInfo,
		0,
		&pTexture,
		0
	);

	if ( FAILED( hr ) )
	{
		Log::Get().Write( L"Failed to load texture from memory!" );
		return( ResourcePtr( new ResourceProxyDX11() ) );
	}

	m_vResources.add( new Texture2dDX11( reinterpret_cast< ID3D11Texture2D* >( pTexture ) ) );

	int ResourceID = ( m_vResources.count() - 1 ) + RT_TEXTURE2D;
	Texture2dConfigDX11 TextureConfig;
	reinterpret_cast< ID3D11Texture2D* >( pTexture )->GetDesc( &TextureConfig.m_State );

	return( ResourcePtr( new ResourceProxyDX11( ResourceID, &TextureConfig, this ) ) );
}
//--------------------------------------------------------------------------------
int RendererDX11::CreateBlendState( BlendStateConfigDX11* pConfig )
{
	ID3D11BlendState* pState = 0;

	HRESULT hr = m_pDevice->CreateBlendState( dynamic_cast<D3D11_BLEND_DESC*>( pConfig ), &pState );

	if ( FAILED( hr ) )
	{
		Log::Get().Write( L"Failed to create blend state!" );
		return( -1 );
	}

	BlendStatePtr ptr = BlendStatePtr( new BlendStateDX11( pState ) );
	m_vBlendStates.add( ptr );

	return( m_vBlendStates.count() - 1 );
}
//--------------------------------------------------------------------------------
int RendererDX11::CreateDepthStencilState( DepthStencilStateConfigDX11* pConfig )
{
	ID3D11DepthStencilState* pState = 0;

	HRESULT hr = m_pDevice->CreateDepthStencilState( dynamic_cast<D3D11_DEPTH_STENCIL_DESC*>( pConfig ), &pState );

	if ( FAILED( hr ) )
	{
		Log::Get().Write( L"Failed to create depth stencil state!" );
		return( -1 );
	}

	DepthStencilStatePtr ptr = DepthStencilStatePtr( new DepthStencilStateDX11( pState ) );
	m_vDepthStencilStates.add( ptr );

	return( m_vDepthStencilStates.count() - 1 );
}
//--------------------------------------------------------------------------------
int RendererDX11::CreateRasterizerState( RasterizerStateConfigDX11* pConfig )
{
	ID3D11RasterizerState* pState = 0;

	HRESULT hr = m_pDevice->CreateRasterizerState( dynamic_cast<D3D11_RASTERIZER_DESC*>( pConfig ), &pState );

	if ( FAILED( hr ) )
	{
		Log::Get().Write( L"Failed to create rasterizer state!" );
		return( -1 );
	}

	RasterizerStatePtr ptr = RasterizerStatePtr( new RasterizerStateDX11( pState ) );
	m_vRasterizerStates.add( ptr );

	return( m_vRasterizerStates.count() - 1 );
}
//--------------------------------------------------------------------------------
int RendererDX11::CreateSamplerState( D3D11_SAMPLER_DESC* pDesc )
{
	ID3D11SamplerState* pState = 0;

	HRESULT hr = m_pDevice->CreateSamplerState( pDesc, &pState );

	if ( FAILED( hr ) )
	{
		Log::Get().Write( L"Failed to create sampler state!" );
		return( -1 );
	}

	m_vSamplerStates.add( new SamplerStateDX11( pState ) );

	return( m_vSamplerStates.count() - 1 );
}
//--------------------------------------------------------------------------------
int RendererDX11::CreateViewPort( D3D11_VIEWPORT viewport )
{
	ViewPortDX11* pViewPort = new ViewPortDX11( viewport );
	m_vViewPorts.add( pViewPort );

	return( m_vViewPorts.count() - 1 );
}
//--------------------------------------------------------------------------------
VertexBufferDX11* RendererDX11::GetVertexBuffer( int index )
{
	if ( ( index > -1 ) && ( index < m_vResources.count() ) )
		return( (VertexBufferDX11*)m_vResources[index] );
	else
		return( 0 );
}
//--------------------------------------------------------------------------------
IndexBufferDX11* RendererDX11::GetIndexBuffer( int index )
{
	if ( ( index > -1 ) && ( index < m_vResources.count() ) )
		return( (IndexBufferDX11*)m_vResources[index] );
	else
		return( 0 );
}
//--------------------------------------------------------------------------------
ResourcePtr RendererDX11::GetSwapChainResource( int ID )
{
	if ( ( ID >= 0 ) && ( m_vSwapChains.count() ) )
		return( m_vSwapChains[ID]->m_Resource );
	Log::Get().Write( L"Tried to get an invalid swap buffer index texture ID!" );

	return( ResourcePtr( new ResourceProxyDX11() ) );
}
//--------------------------------------------------------------------------------
std::wstring RendererDX11::Print_D3D11_SHADER_DESC( D3D11_SHADER_DESC description )
{
	std::wstringstream s;

	s << L"Version: " << description.Version << std::endl;
	s << L"Creator: " << description.Creator << std::endl;
	s << L"Flags: " << description.Flags << std::endl;
	s << L"ConstantBuffers: " << description.ConstantBuffers << std::endl;
	s << L"BoundResources: " << description.BoundResources << std::endl;
	s << L"InputParameters: " << description.InputParameters << std::endl;
	s << L"OutputParameters: " << description.OutputParameters << std::endl;
	s << L"InstructionCount: " << description.InstructionCount << std::endl;
	s << L"TempRegisterCount: " << description.TempRegisterCount << std::endl;
	s << L"TempArrayCount: " << description.TempArrayCount << std::endl;
	s << L"DefCount: " << description.DefCount << std::endl;
	s << L"DclCount: " << description.DclCount << std::endl;
	s << L"TextureNormalInstructions: " << description.TextureNormalInstructions << std::endl;
	s << L"TextureLoadInstructions: " << description.TextureLoadInstructions << std::endl;
	s << L"TextureCompInstructions: " << description.TextureCompInstructions << std::endl;
	s << L"TextureBiasInstructions: " << description.TextureBiasInstructions << std::endl;
	s << L"TextureGradientInstructions: " << description.TextureGradientInstructions << std::endl;
	s << L"FloatInstructionCount: " << description.FloatInstructionCount << std::endl;
	s << L"IntInstructionCount: " << description.IntInstructionCount << std::endl;
	s << L"UintInstructionCount: " << description.UintInstructionCount << std::endl;
	s << L"StaticFlowControlCount: " << description.StaticFlowControlCount << std::endl;
	s << L"DynamicFlowControlCount: " << description.DynamicFlowControlCount << std::endl;
	s << L"MacroInstructionCount: " << description.MacroInstructionCount << std::endl;
	s << L"ArrayInstructionCount: " << description.ArrayInstructionCount << std::endl;
	s << L"CutInstructionCount: " << description.CutInstructionCount << std::endl;
	s << L"EmitInstructionCount: " << description.EmitInstructionCount << std::endl;
	s << L"GSOutputTopology: " << Print_D3D10_PRIMITIVE_TOPOLOGY( description.GSOutputTopology ) << std::endl;
	s << L"GSMaxOutputVertexCount: " << description.GSMaxOutputVertexCount << std::endl;
	s << L"InputPrimitive: " << Print_D3D11_PRIMITIVE( description.InputPrimitive ) << std::endl;
	s << L"PatchConstantParameters: " << description.PatchConstantParameters << std::endl;
	s << L"cGSInstanceCount: " << description.cGSInstanceCount << std::endl;
	s << L"cControlPoints: " << description.cControlPoints << std::endl;
	s << L"HSOutputPrimitive: " << Print_D3D11_TESSELLATOR_OUTPUT_PRIMITIVE( description.HSOutputPrimitive ) << std::endl;
	s << L"HSPartitioning: " << Print_D3D11_TESSELLATOR_PARTITIONING( description.HSPartitioning ) << std::endl;
	s << L"TessellatorDomain: " << Print_D3D11_TESSELLATOR_DOMAIN( description.TessellatorDomain ) << std::endl;
	s << L"cBarrierInstructions: " << description.cBarrierInstructions << std::endl;
	s << L"cInterlockedInstructions: " << description.cInterlockedInstructions << std::endl;
	s << L"cTextureStoreInstructions: " << description.cTextureStoreInstructions << std::endl;

	return( s.str() );
}
//--------------------------------------------------------------------------------
std::wstring RendererDX11::Print_D3D10_PRIMITIVE_TOPOLOGY( D3D10_PRIMITIVE_TOPOLOGY topology )
{
	std::wstringstream s;

	switch ( topology )
	{
	case D3D10_PRIMITIVE_TOPOLOGY_UNDEFINED:
		s << "UNDEFINED";
		break;
	case D3D10_PRIMITIVE_TOPOLOGY_POINTLIST:
		s << "POINT LIST";
		break;
	case D3D10_PRIMITIVE_TOPOLOGY_LINELIST:
		s << "LINE LIST";
		break;
	case D3D10_PRIMITIVE_TOPOLOGY_LINESTRIP:
		s << "LINE STRIP";
		break;
	case D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST:
		s << "TRIANGLE LIST";
		break;
	case D3D10_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP:
		s << "TRIANGLE STRIP";
		break;
	case D3D10_PRIMITIVE_TOPOLOGY_LINELIST_ADJ:
		s << "LINE LIST w/ADJACENCY";
		break;
	case D3D10_PRIMITIVE_TOPOLOGY_LINESTRIP_ADJ:
		s << "LINE STRIP w/ADJACENCY";
		break;
	case D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST_ADJ:
		s << "TRIANGLE LIST w/ADJACENCY";
		break;
	case D3D10_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP_ADJ:
		s << "TRIANGLE STRIP w/ADJACENCY";
		break;
	}

	return( s.str() );
}
//--------------------------------------------------------------------------------
std::wstring RendererDX11::Print_D3D10_PRIMITIVE( D3D10_PRIMITIVE primitive )
{
	std::wstringstream s;

	switch ( primitive )
	{
	case D3D10_PRIMITIVE_UNDEFINED:
		s << "UNDEFINED";
		break;
	case D3D10_PRIMITIVE_POINT:
		s << "POINT";
		break;
	case D3D10_PRIMITIVE_LINE:
		s << "LINE";
		break;
	case D3D10_PRIMITIVE_TRIANGLE:
		s << "TRIANGLE";
		break;
	case D3D10_PRIMITIVE_LINE_ADJ:
		s << "LINE w/ADJACENCY";
		break;
	case D3D10_PRIMITIVE_TRIANGLE_ADJ:
		s << "TRIANGLE w/ADJACENCY";
		break;
	}
	return( s.str() );
}
//--------------------------------------------------------------------------------
std::wstring RendererDX11::Print_D3D11_PRIMITIVE( D3D11_PRIMITIVE primitive )
{
	std::wstringstream s;

	switch ( primitive )
	{
	case D3D11_PRIMITIVE_TOPOLOGY_UNDEFINED:
		s << "UNDEFINED";
		break;
	case D3D11_PRIMITIVE_TOPOLOGY_POINTLIST:
		s << "POINT LIST";
		break;
	case D3D11_PRIMITIVE_TOPOLOGY_LINELIST:
		s << "LINE LIST";
		break;
	case D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP:
		s << "LINE STRIP";
		break;
	case D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST:
		s << "TRIANGLE LIST";
		break;
	case D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP:
		s << "TRIANGLE STRIP";
		break;
	case D3D11_PRIMITIVE_TOPOLOGY_LINELIST_ADJ:
		s << "LINE LIST WITH ADJACENCY";
		break;
	case D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP_ADJ:
		s << "LINE STRIP WITH ADJACENCY";
		break;
	case D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST_ADJ:
		s << "TRIANGLE LIST WITH ADJACENCY";
		break;
	case D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP_ADJ:
		s << "TRIANGLE STRIP WITH ADJACENCY";
		break;
	case D3D11_PRIMITIVE_TOPOLOGY_1_CONTROL_POINT_PATCHLIST:
		s << "01 CONTROL POINT PATCH LIST";
		break;
	case D3D11_PRIMITIVE_TOPOLOGY_2_CONTROL_POINT_PATCHLIST:
		s << "02 CONTROL POINT PATCH LIST";
		break;
	case D3D11_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST:
		s << "03 CONTROL POINT PATCH LIST";
		break;
	case D3D11_PRIMITIVE_TOPOLOGY_4_CONTROL_POINT_PATCHLIST:
		s << "04 CONTROL POINT PATCH LIST";
		break;
	case D3D11_PRIMITIVE_TOPOLOGY_5_CONTROL_POINT_PATCHLIST:
		s << "05 CONTROL POINT PATCH LIST";
		break;
	case D3D11_PRIMITIVE_TOPOLOGY_6_CONTROL_POINT_PATCHLIST:
		s << "06 CONTROL POINT PATCH LIST";
		break;
	case D3D11_PRIMITIVE_TOPOLOGY_7_CONTROL_POINT_PATCHLIST:
		s << "07 CONTROL POINT PATCH LIST";
		break;
	case D3D11_PRIMITIVE_TOPOLOGY_8_CONTROL_POINT_PATCHLIST:
		s << "08 CONTROL POINT PATCH LIST";
		break;
	case D3D11_PRIMITIVE_TOPOLOGY_9_CONTROL_POINT_PATCHLIST:
		s << "09 CONTROL POINT PATCH LIST";
		break;
	case D3D11_PRIMITIVE_TOPOLOGY_10_CONTROL_POINT_PATCHLIST:
		s << "10 CONTROL POINT PATCH LIST";
		break;
	case D3D11_PRIMITIVE_TOPOLOGY_11_CONTROL_POINT_PATCHLIST:
		s << "11 CONTROL POINT PATCH LIST";
		break;
	case D3D11_PRIMITIVE_TOPOLOGY_12_CONTROL_POINT_PATCHLIST:
		s << "12 CONTROL POINT PATCH LIST";
		break;
	case D3D11_PRIMITIVE_TOPOLOGY_13_CONTROL_POINT_PATCHLIST:
		s << "13 CONTROL POINT PATCH LIST";
		break;
	case D3D11_PRIMITIVE_TOPOLOGY_14_CONTROL_POINT_PATCHLIST:
		s << "14 CONTROL POINT PATCH LIST";
		break;
	case D3D11_PRIMITIVE_TOPOLOGY_15_CONTROL_POINT_PATCHLIST:
		s << "15 CONTROL POINT PATCH LIST";
		break;
	case D3D11_PRIMITIVE_TOPOLOGY_16_CONTROL_POINT_PATCHLIST:
		s << "16 CONTROL POINT PATCH LIST";
		break;
	case D3D11_PRIMITIVE_TOPOLOGY_17_CONTROL_POINT_PATCHLIST:
		s << "17 CONTROL POINT PATCH LIST";
		break;
	case D3D11_PRIMITIVE_TOPOLOGY_18_CONTROL_POINT_PATCHLIST:
		s << "18 CONTROL POINT PATCH LIST";
		break;
	case D3D11_PRIMITIVE_TOPOLOGY_19_CONTROL_POINT_PATCHLIST:
		s << "19 CONTROL POINT PATCH LIST";
		break;
	case D3D11_PRIMITIVE_TOPOLOGY_20_CONTROL_POINT_PATCHLIST:
		s << "20 CONTROL POINT PATCH LIST";
		break;
	case D3D11_PRIMITIVE_TOPOLOGY_21_CONTROL_POINT_PATCHLIST:
		s << "21 CONTROL POINT PATCH LIST";
		break;
	case D3D11_PRIMITIVE_TOPOLOGY_22_CONTROL_POINT_PATCHLIST:
		s << "22 CONTROL POINT PATCH LIST";
		break;
	case D3D11_PRIMITIVE_TOPOLOGY_23_CONTROL_POINT_PATCHLIST:
		s << "23 CONTROL POINT PATCH LIST";
		break;
	case D3D11_PRIMITIVE_TOPOLOGY_24_CONTROL_POINT_PATCHLIST:
		s << "24 CONTROL POINT PATCH LIST";
		break;
	case D3D11_PRIMITIVE_TOPOLOGY_25_CONTROL_POINT_PATCHLIST:
		s << "25 CONTROL POINT PATCH LIST";
		break;
	case D3D11_PRIMITIVE_TOPOLOGY_26_CONTROL_POINT_PATCHLIST:
		s << "26 CONTROL POINT PATCH LIST";
		break;
	case D3D11_PRIMITIVE_TOPOLOGY_27_CONTROL_POINT_PATCHLIST:
		s << "27 CONTROL POINT PATCH LIST";
		break;
	case D3D11_PRIMITIVE_TOPOLOGY_28_CONTROL_POINT_PATCHLIST:
		s << "28 CONTROL POINT PATCH LIST";
		break;
	case D3D11_PRIMITIVE_TOPOLOGY_29_CONTROL_POINT_PATCHLIST:
		s << "29 CONTROL POINT PATCH LIST";
		break;
	case D3D11_PRIMITIVE_TOPOLOGY_30_CONTROL_POINT_PATCHLIST:
		s << "30 CONTROL POINT PATCH LIST";
		break;
	case D3D11_PRIMITIVE_TOPOLOGY_31_CONTROL_POINT_PATCHLIST:
		s << "31 CONTROL POINT PATCH LIST";
		break;
	case D3D11_PRIMITIVE_TOPOLOGY_32_CONTROL_POINT_PATCHLIST:
		s << "32 CONTROL POINT PATCH LIST";
		break;
	}

	return( s.str() );
}
//--------------------------------------------------------------------------------
std::wstring RendererDX11::Print_D3D11_TESSELLATOR_OUTPUT_PRIMITIVE( D3D11_TESSELLATOR_OUTPUT_PRIMITIVE primitive )
{
	std::wstringstream s;

	switch ( primitive )
	{
	case D3D11_TESSELLATOR_OUTPUT_UNDEFINED:
		s << "UNDEFINED";
		break;
	case D3D11_TESSELLATOR_OUTPUT_POINT:
		s << "POINT";
		break;
	case D3D11_TESSELLATOR_OUTPUT_LINE:
		s << "LINE";
		break;
	case D3D11_TESSELLATOR_OUTPUT_TRIANGLE_CW:
		s << "TRIANGLE CLOCKWISE";
		break;
	case D3D11_TESSELLATOR_OUTPUT_TRIANGLE_CCW:
		s << "TRIANGLE COUNTER CLOCKWISE";
		break;
	}

	return( s.str() );
}
//--------------------------------------------------------------------------------
std::wstring RendererDX11::Print_D3D11_TESSELLATOR_PARTITIONING( D3D11_TESSELLATOR_PARTITIONING partitioning )
{
	std::wstringstream s;

	switch ( partitioning )
	{
	case D3D11_TESSELLATOR_PARTITIONING_UNDEFINED:
		s << "UNDEFINED";
		break;
	case D3D11_TESSELLATOR_PARTITIONING_INTEGER:
		s << "INTEGER";
		break;
	case D3D11_TESSELLATOR_PARTITIONING_POW2:
		s << "POW2";
		break;
	case D3D11_TESSELLATOR_PARTITIONING_FRACTIONAL_ODD:
		s << "FRACTIONAL ODD";
		break;
	case D3D11_TESSELLATOR_PARTITIONING_FRACTIONAL_EVEN:
		s << "FRACTIONAL EVEN";
		break;
	}

	return( s.str() );
}
//--------------------------------------------------------------------------------
std::wstring RendererDX11::Print_D3D11_TESSELLATOR_DOMAIN( D3D11_TESSELLATOR_DOMAIN domain )
{
	std::wstringstream s;

	switch ( domain )
	{
	case D3D11_TESSELLATOR_DOMAIN_UNDEFINED:
		s << "UNDEFINED";
		break;
	case D3D11_TESSELLATOR_DOMAIN_ISOLINE:
		s << "ISOLINE";
		break;
	case D3D11_TESSELLATOR_DOMAIN_TRI:
		s << "TRIANGLE";
		break;
	case D3D11_TESSELLATOR_DOMAIN_QUAD:
		s << "QUAD";
		break;
	}

	return( s.str() );
}
//--------------------------------------------------------------------------------
Vector2f RendererDX11::GetDesktopResolution()
{
	// Acquire the DXGI device, then the adapter, then the output...

	IDXGIDevice * pDXGIDevice;
	HRESULT hr = m_pDevice->QueryInterface(__uuidof(IDXGIDevice), (void **)&pDXGIDevice);
	IDXGIAdapter * pDXGIAdapter;
	hr = pDXGIDevice->GetParent(__uuidof(IDXGIAdapter), (void **)&pDXGIAdapter);

	// Take the first output in the adapter.

	IDXGIOutput* pDXGIOutput;
	pDXGIAdapter->EnumOutputs( 0, &pDXGIOutput );

	// Use the output interface to get the output description.

	DXGI_OUTPUT_DESC desc;
	pDXGIOutput->GetDesc( &desc );

	// Release all of the interfaces

	pDXGIOutput->Release();
	pDXGIAdapter->Release();
	pDXGIDevice->Release();

	// Return the current output's resolution from the description.
	return( Vector2f( static_cast<float>( desc.DesktopCoordinates.right - desc.DesktopCoordinates.left ),
						static_cast<float>( desc.DesktopCoordinates.bottom - desc.DesktopCoordinates.top ) ) );
}
//--------------------------------------------------------------------------------
BlendStatePtr RendererDX11::GetBlendState( int index )
{
	return( m_vBlendStates[index] ); 
}
//--------------------------------------------------------------------------------
DepthStencilStatePtr RendererDX11::GetDepthState( int index )
{
	return( m_vDepthStencilStates[index] );
}
//--------------------------------------------------------------------------------
RasterizerStatePtr RendererDX11::GetRasterizerState( int index )
{
	return( m_vRasterizerStates[index] );
}
//--------------------------------------------------------------------------------
ViewPortDX11* RendererDX11::GetViewPort( int index )
{
	return( m_vViewPorts[index] );
}
//--------------------------------------------------------------------------------
ResourceDX11* RendererDX11::GetResource( int index )
{
	return( m_vResources[index] );
}
//--------------------------------------------------------------------------------
InputLayoutDX11* RendererDX11::GetInputLayout( int index )
{
	return( m_vInputLayouts[index] );
}
//--------------------------------------------------------------------------------
SamplerStateDX11* RendererDX11::GetSamplerState( int index )
{
	return( m_vSamplerStates[index] );
}
//--------------------------------------------------------------------------------
ShaderDX11* RendererDX11::GetShader( int index )
{
	if ( m_vShaders.inrange( index ) )
		return( m_vShaders[index] );
	else
		return( 0 );
}
//--------------------------------------------------------------------------------
void RendererDX11::QueueRenderView( IRenderView* pRenderView )
{
	m_vQueuedViews.add( pRenderView );
}
//--------------------------------------------------------------------------------
void RendererDX11::ProcessRenderViewQueue( )
{
	if ( !m_bMultiThreadActive )
	{
		for ( int i = m_vQueuedViews.count()-1; i >= 0; i-=NUM_THREADS )
        {
			for ( int j = 0; j < NUM_THREADS; j++ )
			{
				if ( (i-j) >= 0 )
				{
					std::string viewName = ViewRenderParams::ViewIndexToName( m_vQueuedViews[i-j]->GetType() );
					PIXBeginEvent( GlyphString::ToUnicode( "View Draw: " + viewName ).c_str() );

					g_aPayload[j].pRenderView = m_vQueuedViews[i-j];

					m_vQueuedViews[i-j]->Draw( pImmPipeline, g_aPayload[j].pParamManager );

					PIXEndEvent();
				}
			}
        }

		m_vQueuedViews.empty();
	}
	//if ( !m_bMultiThreadActive )
	//{
	//	for ( int i = m_vQueuedViews.count()-1; i >= 0; i-- )
 //       {
 //           std::string viewName = ViewRenderParams::ViewIndexToName( m_vQueuedViews[i]->GetType() );
 //           PIXBeginEvent( GlyphString::ToUnicode( "View Draw: " + viewName ).c_str() );

	//		m_vQueuedViews[i]->Draw( pImmPipeline, g_aPayload[i].pParamManager );

 //           PIXEndEvent();
 //       }

	//	m_vQueuedViews.empty();
	//}

	else
	{

		// While processed count > 0
		//   Execute NUM_THREADS work loads
		//   WFMO
		//   Execute command lists
		//   Update processed count and loop

		for ( int i = m_vQueuedViews.count()-1; i >= 0; i-=NUM_THREADS )
		{
			DWORD count = 0;

			for ( int j = 0; j < NUM_THREADS; j++ )
			{
				if ( (i-j) >= 0 )
				{
					count++;
					g_aPayload[j].pRenderView = m_vQueuedViews[i-j];
					SetEvent( g_aBeginEventHandle[j] );
				}
			}

			WaitForMultipleObjects( count, g_aEndEventHandle, true, INFINITE );

			for ( int j = 0/*count-1*/; count > 0; count-- )
			{
				//g_aPayload[j].pPipeline->m_pContext->ClearState();
				pImmPipeline->ExecuteCommandList( g_aPayload[j].pList );
				g_aPayload[j].pList->ReleaseList();
				//j--;
				j++;
			}

		}

		//for ( int i = m_vQueuedViews.count()-1; i >= 0; i-- )
		//{
		//	g_aPayload[i].pRenderView = m_vQueuedViews[i];
		//	SetEvent( g_aBeginEventHandle[i] );
		//}

		//WaitForMultipleObjects( (DWORD)m_vQueuedViews.count(), g_aEndEventHandle, true, INFINITE );
		
		//for ( int i = m_vQueuedViews.count()-1; i >= 0; i-- )
		//{
		//	g_aPayload[i].pPipeline->m_pContext->ClearState();
		//	pImmPipeline->ExecuteCommandList( g_aPayload[i].pList );
		//	g_aPayload[i].pList->ReleaseList();
		//}

		m_vQueuedViews.empty();

	}
}
//--------------------------------------------------------------------------------
void RendererDX11::PIXBeginEvent( const wchar_t* name )
{
	D3DPERF_BeginEvent( 0xFFFFFFFF, name );
}
//--------------------------------------------------------------------------------
void RendererDX11::PIXEndEvent( )
{
	D3DPERF_EndEvent();
}
//--------------------------------------------------------------------------------
void RendererDX11::SetMultiThreadingState( bool enable )
{
	m_bMultiThreadActive = enable;
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


unsigned int WINAPI _RenderViewThreadProc( void* lpParameter )
{
	ThreadPayLoad* pPayload = (ThreadPayLoad*) lpParameter;

	int id = pPayload->id;


	for ( ; ; )
	{
		// Wait for the main thread to signal that there is a payload available.
		WaitForSingleObject( g_aBeginEventHandle[id], INFINITE );

		pPayload->pPipeline->m_pContext->ClearState();
		pPayload->pPipeline->RasterizerStage.DesiredState.SetRasterizerState( 0 );
		pPayload->pPipeline->OutputMergerStage.DesiredState.SetDepthStencilState( 0 );
		pPayload->pPipeline->OutputMergerStage.DesiredState.SetBlendState( 0 );


		// Execute the render view with the provided pipeline and parameter managers.
		pPayload->pRenderView->Draw( pPayload->pPipeline, pPayload->pParamManager );

		//pPayload->pPipeline->m_pContext->ClearState();

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
	int TYPE = rid & 0xFF0000;
	int ID = rid & 0xFFFF;

	Texture1dDX11* pResult = 0;

	if ( rid != -1 ) {
		// Check if this ID is in range, and that the type is correct.
		if ( !m_vResources.inrange( ID ) || (TYPE != RT_TEXTURE1D ) ) {
			Log::Get().Write( L"Trying to access a non-texture1D resource!!!!" );
		} else {
			pResult = reinterpret_cast<Texture1dDX11*>( m_vResources[ID] );
		}
	}

	return( pResult );
}
//--------------------------------------------------------------------------------
Texture2dDX11* RendererDX11::GetTexture2DByIndex( int rid )
{
	int TYPE = rid & 0xFF0000;
	int ID = rid & 0xFFFF;

	Texture2dDX11* pResult = 0;

	if ( rid != -1 ) {
		// Check if this ID is in range, and that the type is correct.
		if ( !m_vResources.inrange( ID ) || (TYPE != RT_TEXTURE2D ) ) {
			Log::Get().Write( L"Trying to access a non-texture2D resource!!!!" );
		} else {
			pResult = reinterpret_cast<Texture2dDX11*>( m_vResources[ID] );
		}
	}

	return( pResult );
}
//--------------------------------------------------------------------------------
Texture3dDX11* RendererDX11::GetTexture3DByIndex( int rid )
{
	int TYPE = rid & 0xFF0000;
	int ID = rid & 0xFFFF;

	Texture3dDX11* pResult = 0;

	if ( rid != -1 ) {
		// Check if this ID is in range, and that the type is correct.
		if ( !m_vResources.inrange( ID ) || (TYPE != RT_TEXTURE2D ) ) {
			Log::Get().Write( L"Trying to access a non-texture3D resource!!!!" );
		} else {
			pResult = reinterpret_cast<Texture3dDX11*>( m_vResources[ID] );
		}
	}

	return( pResult );
}
//--------------------------------------------------------------------------------
VertexBufferDX11* RendererDX11::GetVertexBufferByIndex( int rid )
{
	int TYPE = rid & 0xFF0000;
	int ID = rid & 0xFFFF;

	VertexBufferDX11* pResult = 0;

	if ( rid != -1 ) {
		// Check if this ID is in range, and that the type is correct.
		if ( !m_vResources.inrange( ID ) || (TYPE != RT_VERTEXBUFFER ) ) {
			Log::Get().Write( L"Trying to access a non-vertex buffer resource!!!!" );
		} else {
			pResult = reinterpret_cast<VertexBufferDX11*>( m_vResources[ID] );
		}
	}

	return( pResult );
}
//--------------------------------------------------------------------------------
IndexBufferDX11* RendererDX11::GetIndexBufferByIndex( int rid )
{
	int TYPE = rid & 0xFF0000;
	int ID = rid & 0xFFFF;

	IndexBufferDX11* pResult = 0;

	if ( rid != -1 ) {
		// Check if this ID is in range, and that the type is correct.
		if ( !m_vResources.inrange( ID ) || (TYPE != RT_INDEXBUFFER ) ) {
			Log::Get().Write( L"Trying to access a non-index buffer resource!!!!" );
		} else {
			pResult = reinterpret_cast<IndexBufferDX11*>( m_vResources[ID] );
		}
	}

	return( pResult );
}
//--------------------------------------------------------------------------------
ByteAddressBufferDX11* RendererDX11::GetByteAddressBufferByIndex( int rid )
{
	int TYPE = rid & 0xFF0000;
	int ID = rid & 0xFFFF;

	ByteAddressBufferDX11* pResult = 0;

	if ( rid != -1 ) {
		// Check if this ID is in range, and that the type is correct.
		if ( !m_vResources.inrange( ID ) || (TYPE != RT_BYTEADDRESSBUFFER ) ) {
			Log::Get().Write( L"Trying to access a non-byte address buffer resource!!!!" );
		} else {
			pResult = reinterpret_cast<ByteAddressBufferDX11*>( m_vResources[ID] );
		}
	}

	return( pResult );
}
//--------------------------------------------------------------------------------
IndirectArgsBufferDX11*	RendererDX11::GetIndirectArgsBufferByIndex( int rid )
{
	int TYPE = rid & 0xFF0000;
	int ID = rid & 0xFFFF;

	IndirectArgsBufferDX11* pResult = 0;

	if ( rid != -1 ) {
		// Check if this ID is in range, and that the type is correct.
		if ( !m_vResources.inrange( ID ) || (TYPE != RT_INDIRECTARGSBUFFER ) ) {
			Log::Get().Write( L"Trying to access a non-vertex buffer resource!!!!" );
		} else {
			pResult = reinterpret_cast<IndirectArgsBufferDX11*>( m_vResources[ID] );
		}
	}

	return( pResult );
}
//--------------------------------------------------------------------------------
StructuredBufferDX11* RendererDX11::GetStructuredBufferByIndex( int rid )
{
	int TYPE = rid & 0xFF0000;
	int ID = rid & 0xFFFF;

	StructuredBufferDX11* pResult = 0;

	if ( rid != -1 ) {
		// Check if this ID is in range, and that the type is correct.
		if ( !m_vResources.inrange( ID ) || (TYPE != RT_STRUCTUREDBUFFER ) ) {
			Log::Get().Write( L"Trying to access a non-structured buffer resource!!!!" );
		} else {
			pResult = reinterpret_cast<StructuredBufferDX11*>( m_vResources[ID] );
		}
	}

	return( pResult );
}
//--------------------------------------------------------------------------------
RenderTargetViewDX11* RendererDX11::GetRenderTargetViewByIndex( int rid )
{
	RenderTargetViewDX11* pResult = 0;

	if ( rid != -1 ) {
		// Check if this ID is in range
		if ( !m_vRenderTargetViews.inrange( rid ) ) {
			Log::Get().Write( L"Trying to access a non-render target view!!!!" );
		} else {
			pResult = m_vRenderTargetViews[rid];
		}
	}

	return( pResult );
}
//--------------------------------------------------------------------------------
DepthStencilViewDX11* RendererDX11::GetDepthStencilViewByIndex( int rid )
{
	DepthStencilViewDX11* pResult = 0;

	if ( rid != -1 ) {
		// Check if this ID is in range
		if ( !m_vDepthStencilViews.inrange( rid ) ) {
			Log::Get().Write( L"Trying to access a non-depth stencil view!!!!" );
		} else {
			pResult = m_vDepthStencilViews[rid];
		}
	}

	return( pResult );
}
//--------------------------------------------------------------------------------
ShaderResourceViewDX11* RendererDX11::GetShaderResourceViewByIndex( int rid )
{
	ShaderResourceViewDX11* pResult = 0;

	if ( rid != -1 ) {
		// Check if this ID is in range
		if ( !m_vShaderResourceViews.inrange( rid ) ) {
			Log::Get().Write( L"Trying to access a non-shader resource view!!!!" );
		} else {
			pResult = m_vShaderResourceViews[rid];
		}
	}

	return( pResult );
}
//--------------------------------------------------------------------------------
UnorderedAccessViewDX11* RendererDX11::GetUnorderedAccessViewByIndex( int rid )
{
	UnorderedAccessViewDX11* pResult = 0;

	if ( rid != -1 ) {
		// Check if this ID is in range
		if ( !m_vUnorderedAccessViews.inrange( rid ) ) {
			Log::Get().Write( L"Trying to access a non-unordered access view!!!!" );
		} else {
			pResult = m_vUnorderedAccessViews[rid];
		}
	}

	return( pResult );
}
//--------------------------------------------------------------------------------
