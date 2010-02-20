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
#include "RendererDX11.h"

#include "Log.h"

#include "VertexBufferDX11.h"
#include "IndexBufferDX11.h"
#include "ConstantBufferDX11.h"
#include "StructuredBufferDX11.h"
#include "ByteAddressBufferDX11.h"
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
#include "ShaderResourceParameterDX11.h"
#include "UnorderedAccessParameterDX11.h"
#include "ConstantBufferParameterDX11.h"
#include "SamplerParameterDX11.h"

#include "RenderEffectDX11.h"
#include "GeometryDX11.h"

#include "DXGIAdapter.h"
#include "DXGIOutput.h"

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
	m_pContext = 0;
	m_pDebugger = 0;
	m_driverType = D3D_DRIVER_TYPE_NULL;
	m_pQuery = 0;

	// Reference the shader stages for use in the binding process.

	ShaderStages[VERTEX_SHADER] = &VertexShaderStage;
	ShaderStages[HULL_SHADER] = &HullShaderStage;
	ShaderStages[DOMAIN_SHADER] = &DomainShaderStage;
	ShaderStages[GEOMETRY_SHADER] = &GeometryShaderStage;
	ShaderStages[PIXEL_SHADER] = &PixelShaderStage;
	ShaderStages[COMPUTE_SHADER] = &ComputeShaderStage;
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

	if ( m_pDevice )
	{
		FeatureLevel = m_pDevice->GetFeatureLevel();
	}
	else
	{
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
	}

	if ( FAILED( hr ) )
	{
		Log::Get().Write( L"Failed to determine the available hardware feature level!" );
	}

	return( FeatureLevel );
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
    UINT CreateDeviceFlags = D3D11_CREATE_DEVICE_SINGLETHREADED; // not interested in multi-threading for now
#ifdef _DEBUG
    CreateDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

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
				&m_pContext
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

	// Rasterizer State (RS) - the first state will be index zero, so no need
	// to keep a copy of it here.

	RasterizerStateConfigDX11 RasterizerState;
	SetRasterizerState( CreateRasterizerState( &RasterizerState ) );

	// Depth Stencil State (DS) - the first state will be index zero, so no need
	// to keep a copy of it here.

	DepthStencilStateConfigDX11 DepthStencilState;
	SetDepthStencilState( CreateDepthStencilState( &DepthStencilState ) );

	// Output Merger State (OM) - the first state will be index zero, so no need
	// to keep a copy of it here.

	BlendStateConfigDX11 BlendState;
	SetBlendState( CreateBlendState( &BlendState ) );

	// Create a query object to be used to gather statistics on the pipeline.

	D3D11_QUERY_DESC queryDesc;
	queryDesc.Query = D3D11_QUERY_PIPELINE_STATISTICS;
	queryDesc.MiscFlags = 0;

	hr = m_pDevice->CreateQuery( &queryDesc, &m_pQuery );

	if ( FAILED( hr ) )
	{
		Log::Get().Write( L"Unable to create a query object!" );
		Shutdown();
		return( false );
	}


    D3D11_FEATURE_DATA_D3D10_X_HARDWARE_OPTIONS Options;
    m_pDevice->CheckFeatureSupport(D3D11_FEATURE_D3D10_X_HARDWARE_OPTIONS, &Options, sizeof(Options));
	if ( Options.ComputeShaders_Plus_RawAndStructuredBuffers_Via_Shader_4_x )
		Log::Get().Write( L"Device supports compute shaders plus raw and structured buffers via shader 4.x" );

	// TODO: Enumerate all of the formats and quality levels available for the given format.
	//       It may be beneficial to allow this query from the user instead of enumerating
	//       all possible formats...
	//UINT NumQuality;
	//HRESULT hr1 = m_pDevice->CheckMultisampleQualityLevels( DXGI_FORMAT_R8G8B8A8_UNORM, 4, &NumQuality );

	return( true );
}
//--------------------------------------------------------------------------------
void RendererDX11::Shutdown()
{
	if( m_pContext ) m_pContext->ClearState();
	if( m_pContext ) m_pContext->Flush();

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
	SAFE_RELEASE( m_pQuery );
	SAFE_RELEASE( m_pContext );
	SAFE_RELEASE( m_pDevice );
	SAFE_RELEASE( m_pDebugger );
}
//--------------------------------------------------------------------------------
void RendererDX11::ClearBuffers( Vector4f color, float depth )
{
	// Get the current render target view and depth stencil view from the OM stage.
	ID3D11RenderTargetView* pRenderTargetView = 0;
	ID3D11DepthStencilView* pDepthStencilView = 0;

	m_pContext->OMGetRenderTargets( 1, &pRenderTargetView, &pDepthStencilView );

	if ( pRenderTargetView )
	{
		float clearColours[] = { color.x, color.y, color.z, color.w }; // RGBA
		m_pContext->ClearRenderTargetView( pRenderTargetView, clearColours );
	}

	if ( pDepthStencilView )
	{
		m_pContext->ClearDepthStencilView( pDepthStencilView, D3D11_CLEAR_DEPTH, depth, 0 );
	}

	// Release the two views
	SAFE_RELEASE( pRenderTargetView );
	SAFE_RELEASE( pDepthStencilView );
}
//--------------------------------------------------------------------------------
void RendererDX11::Present( HWND hWnd, int SwapChain )
{
	// _) Present to the screen

	if ( ( SwapChain >= 0 ) && ( SwapChain < m_vSwapChains.count() ) )
	{
		DXGI_SWAP_CHAIN_DESC desc;
		SwapChainDX11* pSwapChain = m_vSwapChains[SwapChain];
		pSwapChain->m_pSwapChain->GetDesc( &desc );
		pSwapChain->m_pSwapChain->Present( 0, 0 );
	}
	else
		Log::Get().Write( L"Tried to present an invalid swap chain index!" );

	//HRESULT presentResult;

	//if ( SwapChain == -1 )
	//	presentResult = m_pDevice->Present( NULL, NULL, hWnd, NULL );
	//else
	//	presentResult = m_vSC[SwapChain]->m_pSwapChain->Present( NULL, NULL, hWnd, NULL, 0 );

	//if ( presentResult == D3DERR_DEVICELOST )
	//	OnLostDevice(); 
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

	// Create and store the render target object from this swap chain.

	ID3D11Texture2D* pSwapChainBuffer = 0;

	hr = pSwapChain->GetBuffer( 0, __uuidof( ID3D11Texture2D ), reinterpret_cast< void** >( &pSwapChainBuffer ) );

	if ( FAILED( hr ) )
	{
		Log::Get().Write( L"Failed to get swap chain texture resource!" );
		return( -1 );
	}

	ID3D11RenderTargetView* pRenderTargetView = 0;

	hr = m_pDevice->CreateRenderTargetView( pSwapChainBuffer, 0, &pRenderTargetView );

	if ( FAILED( hr ) )
	{
		Log::Get().Write( L"Failed to create render target view of swap chain's back buffer!" );
		return( -1 );
	}

	// Add the swap chain's back buffer texture and render target views to the internal data
	// structures to allow setting them later on.

	m_vResources.add( new Texture2dDX11( pSwapChainBuffer ) );
	m_vRenderTargetViews.add( new RenderTargetViewDX11( pRenderTargetView ) );

	// If we get here, then we succeeded in creating our swap chain and it's constituent parts.
	// Now we create the wrapper object and store the result in our container.

	m_vSwapChains.add( new SwapChainDX11( pSwapChain, m_vResources.count() - 1, m_vRenderTargetViews.count() - 1 ) );

	return( m_vSwapChains.count() - 1 );
}
//--------------------------------------------------------------------------------
int RendererDX11::CreateVertexBuffer( BufferConfigDX11* pConfig,  D3D11_SUBRESOURCE_DATA* pData )
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
		return( ( m_vResources.count() - 1 ) + RT_VERTEXBUFFER );
	}

	return( -1 );
}
//--------------------------------------------------------------------------------
int RendererDX11::CreateIndexBuffer( BufferConfigDX11* pConfig,  D3D11_SUBRESOURCE_DATA* pData )
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
		return( ( m_vResources.count() - 1 ) + RT_INDEXBUFFER );
	}

	return( -1 );
}
//--------------------------------------------------------------------------------
int RendererDX11::CreateStructuredBuffer( BufferConfigDX11* pConfig,  D3D11_SUBRESOURCE_DATA* pData )
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
		return( ( m_vResources.count() - 1 ) + RT_STRUCTUREDBUFFER );
	}

	return( -1 );
}
//--------------------------------------------------------------------------------
int RendererDX11::CreateByteAddressBuffer( BufferConfigDX11* pConfig,  D3D11_SUBRESOURCE_DATA* pData )
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
		return( ( m_vResources.count() - 1 ) + RT_BYTEADDRESSBUFFER );
	}

	return( -1 );
}
//--------------------------------------------------------------------------------
int RendererDX11::CreateConstantBuffer( BufferConfigDX11* pConfig,  D3D11_SUBRESOURCE_DATA* pData )
{
	// Set the constant buffer flag in addition to any other flags that
	// the user has set.

	ID3D11Buffer* pBuffer = 0;
	HRESULT hr = m_pDevice->CreateBuffer( &pConfig->m_State, pData, &pBuffer );

	if ( pBuffer )
	{
		ConstantBufferDX11* pConstantBuffer = new ConstantBufferDX11( pBuffer );
		pConstantBuffer->SetDesiredDescription( pConfig->m_State );
		m_vResources.add( pConstantBuffer );

		// Return an index with the lower 16 bits of index, and the upper
		// 16 bits to identify the resource type.
		return( ( m_vResources.count() - 1 ) + RT_CONSTANTBUFFER );
	}

	return( -1 );
}
//--------------------------------------------------------------------------------
int RendererDX11::CreateTexture1D( Texture1dConfigDX11* pConfig, D3D11_SUBRESOURCE_DATA* pData )
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
		return( ( m_vResources.count() - 1 ) + RT_TEXTURE1D );
	}

	return( -1 );
}
//--------------------------------------------------------------------------------
int RendererDX11::CreateTexture2D( Texture2dConfigDX11* pConfig, D3D11_SUBRESOURCE_DATA* pData )
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
		return( ( m_vResources.count() - 1 ) + RT_TEXTURE2D );
	}

	return( -1 );
}
//--------------------------------------------------------------------------------
int RendererDX11::CreateTexture3D( Texture3dConfigDX11* pConfig, D3D11_SUBRESOURCE_DATA* pData )
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
		return( ( m_vResources.count() - 1 ) + RT_TEXTURE3D );
	}

	return( -1 );
}
//--------------------------------------------------------------------------------
int RendererDX11::CreateShaderResourceView( int ResourceID, D3D11_SHADER_RESOURCE_VIEW_DESC* pDesc )
{
	int TYPE = ResourceID & 0xFF0000;
	int ID = ResourceID & 0xFFFF;
	ID3D11Resource* pResource = 0;

	// TODO: Add bounds checking...
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
int RendererDX11::LoadShader( ShaderType type, std::wstring& filename, std::wstring& function, std::wstring& model )
{
	HRESULT hr = S_OK;

	ID3DBlob* pCompiledShader = NULL;
	ID3DBlob* pErrorMessages = NULL;

	char AsciiFunction[1024];
	char AsciiModel[1024];
	WideCharToMultiByte(CP_ACP, 0, function.c_str(), -1, AsciiFunction, 1024, NULL, NULL);
	WideCharToMultiByte(CP_ACP, 0, model.c_str(), -1, AsciiModel, 1024, NULL, NULL);
 

	if ( FAILED( hr = D3DX11CompileFromFile( 
		filename.c_str(),
		0,
		0,
		AsciiFunction,
		AsciiModel,
		D3D10_SHADER_PACK_MATRIX_ROW_MAJOR, 
		0,//UINT Flags2,
		0,
		&pCompiledShader,
		&pErrorMessages,
		&hr
		) ) )
	{
		if ( pErrorMessages != 0 )
		{
			LPVOID pCompileErrors = pErrorMessages->GetBufferPointer();
			const char* pMessage = (const char*)pCompileErrors;
			Log::Get().Write( GlyphString::ToUnicode( std::string( pMessage ) ) );
		}

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
		pReflector->GetInputParameterDesc( i, &output_desc );
		SignatureParameterDesc d(output_desc);
		pShaderWrapper->OutputSignatureParameters.add( d );
	}


	// Get the constant buffer information, which will be used for setting parameters
	// for use by this shader at rendering time.

	for ( UINT i = 0; i < desc.ConstantBuffers; i++ )
	{
		ConstantBufferLayout BufferLayout;
		ID3D11ShaderReflectionConstantBuffer* pConstBuffer = pReflector->GetConstantBufferByIndex( i );
		D3D11_SHADER_BUFFER_DESC desc;
		pConstBuffer->GetDesc( &desc );
		BufferLayout.Description = ShaderBufferDesc( desc );
		
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
		}

		pShaderWrapper->ConstantBuffers.add( BufferLayout );
	}


	// Get the overall resource binding information for this shader.  This includes 
	// the constant buffers, plus all of the other objects that can be bound to the
	// pipeline with resource views.

	for ( UINT i = 0; i < desc.BoundResources; i++ )
	{
		D3D11_SHADER_INPUT_BIND_DESC resource_desc;
		pReflector->GetResourceBindingDesc( i, &resource_desc );
		ShaderInputBindDesc binddesc( resource_desc );
		pShaderWrapper->ResourceBindings.add( binddesc );
	}


	// Release the shader reflection interface
	pReflector->Release();
	

	// Store the compiled shader in the shader wrapper for use later on in creating
	// and checking input and output signatures.
	
	pShaderWrapper->pCompiledShader = pCompiledShader;


	// Return the index for future referencing.

	pShaderWrapper->PrintShaderDetails();

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
void RendererDX11::SetVectorParameter( std::wstring name, Vector4f* pVector )
{
	RenderParameterDX11* pParameter = m_Parameters[name];

	// Only create the new parameter if it hasn't already been registered
	if ( pParameter == 0 )
	{
		pParameter = new VectorParameterDX11();
		m_Parameters[name] = reinterpret_cast<RenderParameterDX11*>( pParameter );
	}

	if ( pParameter->GetParameterType() == VECTOR )
		pParameter->SetParameterData( reinterpret_cast<void*>( pVector ) );
	else
		Log::Get().Write( L"Vector parameter name collision!" );
}
//--------------------------------------------------------------------------------
void RendererDX11::SetMatrixParameter( std::wstring name, Matrix4f* pMatrix )
{
	RenderParameterDX11* pParameter = m_Parameters[name];

	// Only create the new parameter if it hasn't already been registered
	if ( pParameter == 0 )
	{
		pParameter = new MatrixParameterDX11();
		m_Parameters[name] = reinterpret_cast<RenderParameterDX11*>( pParameter );
	}
	
	if ( pParameter->GetParameterType() == MATRIX )
		pParameter->SetParameterData( reinterpret_cast<void*>( pMatrix ) );
	else
		Log::Get().Write( L"Matrix parameter name collision!" );
}
//--------------------------------------------------------------------------------
void RendererDX11::SetShaderResourceParameter( std::wstring name, int* pID )
{
	RenderParameterDX11* pParameter = m_Parameters[name];

	// Only create the new parameter if it hasn't already been registered
	if ( pParameter == 0 )
	{
		pParameter = new ShaderResourceParameterDX11();
		m_Parameters[name] = reinterpret_cast<RenderParameterDX11*>( pParameter );
	}

	if ( pParameter->GetParameterType() == SHADER_RESOURCE )
		pParameter->SetParameterData( reinterpret_cast<void*>( pID ) );
	else
		Log::Get().Write( L"Shader resource view parameter name collision!" );
}
//--------------------------------------------------------------------------------
void RendererDX11::SetUnorderedAccessParameter( std::wstring name, int* pID )
{
	RenderParameterDX11* pParameter = m_Parameters[name];

	// Only create the new parameter if it hasn't already been registered
	if ( pParameter == 0 )
	{
		pParameter = new UnorderedAccessParameterDX11();
		m_Parameters[name] = reinterpret_cast<RenderParameterDX11*>( pParameter );
	}

	if ( pParameter->GetParameterType() == UNORDERED_ACCESS )
		pParameter->SetParameterData( reinterpret_cast<void*>( pID ) );
	else
		Log::Get().Write( L"Unordered access view parameter name collision!" );
}
//--------------------------------------------------------------------------------
void RendererDX11::SetConstantBufferParameter( std::wstring name, int* pID )
{
	RenderParameterDX11* pParameter = m_Parameters[name];

	// Only create the new parameter if it hasn't already been registered
	if ( pParameter == 0 )
	{
		pParameter = new ConstantBufferParameterDX11();
		m_Parameters[name] = reinterpret_cast<RenderParameterDX11*>( pParameter );
	}

	if ( pParameter->GetParameterType() == CBUFFER )
		pParameter->SetParameterData( reinterpret_cast<void*>( pID ) );
	else
		Log::Get().Write( L"Constant buffer parameter name collision!" );
}
//--------------------------------------------------------------------------------
void RendererDX11::SetSamplerParameter( std::wstring name, int* pID )
{
	RenderParameterDX11* pParameter = m_Parameters[name];

	// Only create the new parameter if it hasn't already been registered
	if ( pParameter == 0 )
	{
		pParameter = new SamplerParameterDX11();
		m_Parameters[name] = reinterpret_cast<RenderParameterDX11*>( pParameter );
	}

	if ( pParameter->GetParameterType() == SAMPLER )
		pParameter->SetParameterData( reinterpret_cast<void*>( pID ) );
	else
		Log::Get().Write( L"Sampler parameter name collision!" );
}
//--------------------------------------------------------------------------------
Vector4f RendererDX11::GetVectorParameter( std::wstring name )
{
	Vector4f result;
	result.MakeZero();

	RenderParameterDX11* pParam = m_Parameters[name];

	if ( pParam != 0 )
		if ( pParam->GetParameterType() == VECTOR ) 
			result = reinterpret_cast<VectorParameterDX11*>( pParam )->GetVector();

	return( result );
}
//--------------------------------------------------------------------------------
Matrix4f RendererDX11::GetMatrixParameter( std::wstring name )
{
	// TODO: If the parameter does not exist, create it and return the default value!
	//       This should be done for all parameter types!

	Matrix4f result;
	result.MakeZero();

	RenderParameterDX11* pParam = m_Parameters[name];

	if ( pParam != 0 )
		if ( pParam->GetParameterType() == MATRIX ) 
			result = reinterpret_cast<MatrixParameterDX11*>( pParam )->GetMatrix();

	return( result );
}
//--------------------------------------------------------------------------------
int RendererDX11::GetShaderResourceParameter( std::wstring name )
{
	int result;
	result = -1;

	RenderParameterDX11* pParam = m_Parameters[name];

	if ( pParam != 0 )
		if ( pParam->GetParameterType() == SHADER_RESOURCE ) 
			result = reinterpret_cast<ShaderResourceParameterDX11*>( pParam )->GetIndex();

	return( result );
}
//--------------------------------------------------------------------------------
int RendererDX11::GetUnorderedAccessParameter( std::wstring name )
{
	int result;
	result = -1;

	RenderParameterDX11* pParam = m_Parameters[name];

	if ( pParam != 0 )
		if ( pParam->GetParameterType() == UNORDERED_ACCESS ) 
			result = reinterpret_cast<UnorderedAccessParameterDX11*>( pParam )->GetIndex();

	return( result );
}
//--------------------------------------------------------------------------------
int RendererDX11::GetConstantBufferParameter( std::wstring name )
{
	int result;
	result = -1;

	RenderParameterDX11* pParam = m_Parameters[name];

	if ( pParam != 0 )
		if ( pParam->GetParameterType() == CBUFFER ) 
			result = reinterpret_cast<ConstantBufferParameterDX11*>( pParam )->GetIndex();

	return( result );
}
//--------------------------------------------------------------------------------
int RendererDX11::GetSamplerStateParameter( std::wstring name )
{
	int result;
	result = -1;

	RenderParameterDX11* pParam = m_Parameters[name];

	if ( pParam != 0 )
		if ( pParam->GetParameterType() == SAMPLER ) 
			result = reinterpret_cast<SamplerParameterDX11*>( pParam )->GetIndex();

	return( result );	
}
//--------------------------------------------------------------------------------
void RendererDX11::SetWorldMatrixParameter( Matrix4f* pMatrix )
{
	SetMatrixParameter( std::wstring( L"WorldMatrix" ), pMatrix );

	Matrix4f WorldMatrix = GetMatrixParameter( std::wstring( L"WorldMatrix" ) );
	Matrix4f ViewMatrix = GetMatrixParameter( std::wstring( L"ViewMatrix" ) );
	Matrix4f ProjMatrix = GetMatrixParameter( std::wstring( L"ProjMatrix" ) );

	Matrix4f ViewProjMatrix = ViewMatrix * ProjMatrix;
	Matrix4f WorldViewProjMatrix = WorldMatrix * ViewProjMatrix;

	SetMatrixParameter( std::wstring( L"ViewProjMatrix" ), &ViewProjMatrix );
	SetMatrixParameter( std::wstring( L"WorldViewProjMatrix" ), &WorldViewProjMatrix );
}
//--------------------------------------------------------------------------------
void RendererDX11::SetViewMatrixParameter( Matrix4f* pMatrix )
{
	SetMatrixParameter( std::wstring( L"ViewMatrix" ), pMatrix );

	Matrix4f WorldMatrix = GetMatrixParameter( std::wstring( L"WorldMatrix" ) );
	Matrix4f ViewMatrix = GetMatrixParameter( std::wstring( L"ViewMatrix" ) );
	Matrix4f ProjMatrix = GetMatrixParameter( std::wstring( L"ProjMatrix" ) );

	Matrix4f ViewProjMatrix = ViewMatrix * ProjMatrix;
	Matrix4f WorldViewProjMatrix = WorldMatrix * ViewProjMatrix;

	SetMatrixParameter( std::wstring( L"ViewProjMatrix" ), &ViewProjMatrix );
	SetMatrixParameter( std::wstring( L"WorldViewProjMatrix" ), &WorldViewProjMatrix );
}
//--------------------------------------------------------------------------------
void RendererDX11::SetProjMatrixParameter( Matrix4f* pMatrix )
{
	SetMatrixParameter( std::wstring( L"ProjMatrix" ), pMatrix );

	Matrix4f WorldMatrix = GetMatrixParameter( std::wstring( L"WorldMatrix" ) );
	Matrix4f ViewMatrix = GetMatrixParameter( std::wstring( L"ViewMatrix" ) );
	Matrix4f ProjMatrix = GetMatrixParameter( std::wstring( L"ProjMatrix" ) );

	Matrix4f ViewProjMatrix = ViewMatrix * ProjMatrix;
	Matrix4f WorldViewProjMatrix = WorldMatrix * ViewProjMatrix;

	SetMatrixParameter( std::wstring( L"ViewProjMatrix" ), &ViewProjMatrix );
	SetMatrixParameter( std::wstring( L"WorldViewProjMatrix" ), &WorldViewProjMatrix );
}
//--------------------------------------------------------------------------------
void RendererDX11::BindShader( ShaderType type, int ID )
{
	// Check if the shader has a valid identifier
	if ( ( ID >= 0 ) && ( ID < m_vShaders.count() ) )
	{
		// Get a copy of the shader object for use in the remainder of this function.

		ShaderDX11* pShaderDX11 = m_vShaders[ID];

		// Before binding the shader, have it update its required parameters.  These
		// parameters will then be bound to the pipeline after the shader is bound.
		
		pShaderDX11->UpdateParameters( this );

		// Perform the actual binding to the pipeline, and then bind all needed
		// parameters.

		if ( pShaderDX11->GetType() == type )
		{
			switch( type )
			{
				case VERTEX_SHADER:
				{
					ID3D11VertexShader* pShader = reinterpret_cast<VertexShaderDX11*>( pShaderDX11 )->m_pVertexShader;
					m_pContext->VSSetShader( pShader, 0, 0 );
					break;
				}
				case HULL_SHADER:
				{
					ID3D11HullShader* pShader = reinterpret_cast<HullShaderDX11*>( pShaderDX11 )->m_pHullShader;
					m_pContext->HSSetShader( pShader, 0, 0 );
					break;
				}
				case DOMAIN_SHADER:
				{
					ID3D11DomainShader* pShader = reinterpret_cast<DomainShaderDX11*>( pShaderDX11 )->m_pDomainShader;
					m_pContext->DSSetShader( pShader, 0, 0 );
					break;
				}
				case GEOMETRY_SHADER:
				{
					ID3D11GeometryShader* pShader = reinterpret_cast<GeometryShaderDX11*>( pShaderDX11 )->m_pGeometryShader;
					m_pContext->GSSetShader( pShader, 0, 0 );
					break;
				}
				case PIXEL_SHADER:
				{
					ID3D11PixelShader* pShader = reinterpret_cast<PixelShaderDX11*>( pShaderDX11 )->m_pPixelShader;
					m_pContext->PSSetShader( pShader, 0, 0 );
					break;
				}
				case COMPUTE_SHADER:
				{
					ID3D11ComputeShader* pShader = reinterpret_cast<ComputeShaderDX11*>( pShaderDX11 )->m_pComputeShader;
					m_pContext->CSSetShader( pShader, 0, 0 );
					break;
				}
			}

			pShaderDX11->BindParameters( this );
		}
		else
		{
			Log::Get().Write( L"Tried to set the wrong type of shader ID!" );
		}
	}
	else
	{
		if ( ID == -1 )
			this->UnbindShader( type );
		else
			Log::Get().Write( L"Tried to set an invalid shader ID!" );
	}
}
//--------------------------------------------------------------------------------
void RendererDX11::BindConstantBufferParameter( ShaderType type, std::wstring name, UINT slot )
{
	if ( m_Parameters[name] != 0 )
	{
		// Check the type of the parameter
		if ( m_Parameters[name]->GetParameterType() == CBUFFER )
		{
			ConstantBufferParameterDX11* pBuffer = reinterpret_cast<ConstantBufferParameterDX11*>( m_Parameters[name] );
			int ID = ( pBuffer->GetIndex() & 0xffff ); 

			// Allow a range including -1 up to the number of resources
			if ( ( ID >= -1 ) && ( ID < m_vResources.count() ) )
			{
				// Get the resource to be set, and pass it in to the desired shader type
				
				ID3D11Buffer* pBuffer = 0;
				
				if ( ID >= 0 )
					pBuffer = (ID3D11Buffer*)m_vResources[ID]->GetResource();

				ShaderStages[type]->SetConstantBuffer( slot, pBuffer );
			}
			else
			{
				Log::Get().Write( L"Tried to set an invalid constant buffer ID!" );
			}
		}
		else
		{
			Log::Get().Write( L"Tried to set a non-constant buffer ID as a constant buffer!" );
		}
	}
	else
	{
		Log::Get().Write( L"Tried to set a non-existing parameter as a constant buffer!" );
	}
}
//--------------------------------------------------------------------------------
void RendererDX11::BindShaderResourceParameter( ShaderType type, std::wstring name, UINT slot )
{
	if ( m_Parameters[name] != 0 )
	{
		// Check the type of the parameter
		if ( m_Parameters[name]->GetParameterType() == SHADER_RESOURCE )
		{
			ShaderResourceParameterDX11* pResource = 
				reinterpret_cast<ShaderResourceParameterDX11*>( m_Parameters[name] );

			int ID = pResource->GetIndex(); 

			// Allow a range including -1 up to the number of resources views
			if ( ( ID >= -1 ) && ( ID < m_vShaderResourceViews.count() ) )
			{
				// Get the resource to be set, and pass it in to the desired shader type

				ID3D11ShaderResourceView* pResourceView = 0;

				if ( ID >= 0 )
					pResourceView = m_vShaderResourceViews[ID]->m_pShaderResourceView;

				ShaderStages[type]->SetShaderResourceView( slot, pResourceView );
			}
			else
			{
				Log::Get().Write( L"Tried to set an invalid shader resource ID!" );
			}
		}
		else
		{
			Log::Get().Write( L"Tried to set a non-shader resource ID as a shader resource!" );
		}
	}
	else
	{
		Log::Get().Write( L"Tried to set a non-existing parameter as a shader resource!" );
	}
}
//--------------------------------------------------------------------------------
void RendererDX11::BindUnorderedAccessParameter( ShaderType type, std::wstring name, UINT slot )
{
	if ( m_Parameters[name] != 0 )
	{
		// Check the type of the parameter
		if ( m_Parameters[name]->GetParameterType() == UNORDERED_ACCESS )
		{
			UnorderedAccessParameterDX11* pResource = 
				reinterpret_cast<UnorderedAccessParameterDX11*>( m_Parameters[name] );

			int ID = pResource->GetIndex(); 

			// Allow a range including -1 up to the number of resources views
			if ( ( ID >= -1 ) && ( ID < m_vUnorderedAccessViews.count() ) )
			{
				// Get the resource to be set, and pass it in to the desired shader type

				ID3D11UnorderedAccessView* pResourceView = 0;

				if ( ID >= 0 )
					pResourceView = m_vUnorderedAccessViews[ID]->m_pUnorderedAccessView;

				ShaderStages[type]->SetUnorderedAccessView( slot, pResourceView );
			}
			else
			{
				Log::Get().Write( L"Tried to set an invalid shader resource ID!" );
			}
		}
		else
		{
			Log::Get().Write( L"Tried to set a non-shader resource ID as a shader resource!" );
		}
	}
	else
	{
		Log::Get().Write( L"Tried to set a non-existing parameter as a unordered access view!" );
	}
}
//--------------------------------------------------------------------------------
void RendererDX11::BindSamplerStateParameter( ShaderType type, std::wstring name, UINT slot )
{
	if ( m_Parameters[name] != 0 )
	{
		// Check the type of the parameter
		if ( m_Parameters[name]->GetParameterType() == SAMPLER )
		{
			SamplerParameterDX11* pResource = 
				reinterpret_cast<SamplerParameterDX11*>( m_Parameters[name] );

			int ID = pResource->GetIndex(); 

			// Allow a range including -1 up to the number of samplers

			if ( ( ID >= -1 ) && ( ID < m_vSamplerStates.count() ) )
			{
				// Get the resource to be set, and pass it in to the desired shader type

				ID3D11SamplerState* pSampler = 0;

				if ( ID >= 0 )
					pSampler = m_vSamplerStates[ID]->m_pState; 

				ShaderStages[type]->SetSamplerState( slot, pSampler );
			}
			else
			{
				Log::Get().Write( L"Tried to set an invalid sampler state ID!" );
			}
		}
		else
		{
			Log::Get().Write( L"Tried to set a non-sampler state ID as a sampler state!" );
		}
	}
	else
	{
		Log::Get().Write( L"Tried to set a non-existing parameter as a sampler state!" );
	}
}
//--------------------------------------------------------------------------------
void RendererDX11::ApplyPipelineResources( )
{
	VertexShaderStage.BindResources( m_pContext );
	HullShaderStage.BindResources( m_pContext );
	DomainShaderStage.BindResources( m_pContext );
	GeometryShaderStage.BindResources( m_pContext );
	PixelShaderStage.BindResources( m_pContext );
	ComputeShaderStage.BindResources( m_pContext );
}
//--------------------------------------------------------------------------------
void RendererDX11::ClearPipelineResources( )
{
	VertexShaderStage.UnbindResources( m_pContext );
	HullShaderStage.UnbindResources( m_pContext );
	DomainShaderStage.UnbindResources( m_pContext );
	GeometryShaderStage.UnbindResources( m_pContext );
	PixelShaderStage.UnbindResources( m_pContext );
	ComputeShaderStage.UnbindResources( m_pContext );
}
//--------------------------------------------------------------------------------
void RendererDX11::BindInputLayout( int ID )
{
	if ( ( ID >= 0 ) && ( ID < m_vInputLayouts.count() ) )
		m_pContext->IASetInputLayout( m_vInputLayouts[ID]->m_pInputLayout );
	else
		Log::Get().Write( L"Tried to bind an invalid input layout ID!" );
}
//--------------------------------------------------------------------------------
void RendererDX11::BindVertexBuffer( int index, UINT stride )
{
	// TODO: Add the ability to set multiple vertex buffers at once, and to 
	//       provide an offset to the data contained in the buffers.

	// Select only the index portion of the handle.
	int TYPE	= index & 0x00FF0000;
	int ID		= index & 0x0000FFFF;

	ID3D11Buffer* Buffers = { (ID3D11Buffer*)m_vResources[ID]->GetResource() };
	UINT Strides = { stride };
	UINT Offsets = { 0 };

	if ( ( ID >= 0 ) && ( ID < m_vResources.count() ) )
	{
		m_pContext->IASetVertexBuffers( 0, 1, &Buffers, &Strides, &Offsets );
	}
	else
		Log::Get().Write( L"Tried to bind an invalid vertex buffer ID!" );
}
//--------------------------------------------------------------------------------
void RendererDX11::SetVertexBufferStride( int index, UINT stride )
{
	// Select only the index portion of the handle.
	int TYPE	= index & 0x00FF0000;
	int ID		= index & 0x0000FFFF;

	((VertexBufferDX11*)m_vResources[ID])->m_iVertexSize = stride;
	//Log::Get().Write( L"Tried to bind an invalid vertex buffer ID!" );
}
//--------------------------------------------------------------------------------
void RendererDX11::BindIndexBuffer( int index )
{
	// TODO: Add the ability to use different formats and offsets to this function!

	// Select only the index portion of the handle.
	int TYPE	= index & 0x00FF0000;
	int ID		= index & 0x0000FFFF;

	// TODO: do the in bounds check before indexing the resource!
	ID3D11Buffer* Buffers = (ID3D11Buffer*)m_vResources[ID]->GetResource();

	if ( ( ID >= 0 ) && ( ID < m_vResources.count() ) )
		m_pContext->IASetIndexBuffer( Buffers, DXGI_FORMAT_R32_UINT, 0 );
	else
		Log::Get().Write( L"Tried to bind an invalid index buffer ID!" );
}
//--------------------------------------------------------------------------------
void RendererDX11::BindRenderTargets( int RenderID, int DepthID )
{
	// TODO: Add the ability to bind multiple render target views!

	if ( ( RenderID >= 0 ) && ( RenderID < m_vRenderTargetViews.count() ) 
		&& ( DepthID >= 0 ) && ( DepthID < m_vDepthStencilViews.count() ) )
	{
		ID3D11RenderTargetView* pRenderTarget = { m_vRenderTargetViews[RenderID]->m_pRenderTargetView };
		ID3D11DepthStencilView* pDepthStencilView = m_vDepthStencilViews[DepthID]->m_pDepthStencilView;

		m_pContext->OMSetRenderTargets( 1, &pRenderTarget, pDepthStencilView );
	}
	else
		Log::Get().Write( L"Tried to bind an invalid render target or depth stencil view!" );
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


	// Return the index for referencing later on.

	return( m_vInputLayouts.count() - 1 );
}
//--------------------------------------------------------------------------------
D3D11_MAPPED_SUBRESOURCE RendererDX11::MapResource( int index, UINT subresource, D3D11_MAP actions, UINT flags )
{
	int TYPE	= index & 0x00FF0000;
	int ID		= index & 0x0000FFFF;

	ID3D11Resource* pResource = 0;
	D3D11_MAPPED_SUBRESOURCE Data;

	pResource = m_vResources[ID]->GetResource();

	// Map the resource
	HRESULT hr = m_pContext->Map( pResource, subresource, actions, flags, &Data );
	
	if ( FAILED( hr ) )
		Log::Get().Write( L"Failed to map resource!" );

	return( Data );
}
//--------------------------------------------------------------------------------
void RendererDX11::UnMapResource( int index, UINT subresource )
{
	int TYPE	= index & 0x00FF0000;
	int ID		= index & 0x0000FFFF;

	ID3D11Resource* pResource = 0;

	pResource = m_vResources[ID]->GetResource();

	// Unmap the resource - there is no HRESULT returned, so trust that it works...
	m_pContext->Unmap( pResource, subresource );
}
//--------------------------------------------------------------------------------
void RendererDX11::Draw( RenderEffectDX11& effect, GeometryDX11& geometry )
{
	// Specify the type of geometry that we will be dealing with.

	m_pContext->IASetPrimitiveTopology( geometry.GetPrimitiveType() );

	// Bind the vertex and index buffers.

	BindVertexBuffer( geometry.m_iVB, geometry.GetVertexSize() );
	BindIndexBuffer( geometry.m_iIB );

	// Bind the input layout.  The layout will be automatically generated if it
	// doesn't already exist.

	BindInputLayout( geometry.GetInputLayout( effect.m_iVertexShader ) );

	// Use the effect to load all of the pipeline stages here.

	ClearPipelineResources();
	effect.ConfigurePipeline( this );
	ApplyPipelineResources();

	// TODO: comprehend the last two parameters here and how they can be used...

	m_pContext->DrawIndexed( geometry.GetIndexCount(), 0, 0 );
}
//--------------------------------------------------------------------------------
void RendererDX11::Dispatch( RenderEffectDX11& effect, UINT x, UINT y, UINT z )
{
	// Use the effect to load all of the pipeline stages here.

	ClearPipelineResources();
	effect.ConfigurePipeline( this );
	ApplyPipelineResources();

	m_pContext->Dispatch( x, y, z );
}
//--------------------------------------------------------------------------------
void RendererDX11::StartPipelineStatistics( )
{
	if ( m_pQuery )
		m_pContext->Begin( m_pQuery );
	else
		Log::Get().Write( L"Tried to begin pipeline statistics without a query object!" );
}
//--------------------------------------------------------------------------------
void RendererDX11::EndPipelineStatistics( )
{
	if ( m_pQuery )
		m_pContext->End( m_pQuery );
	else
		Log::Get().Write( L"Tried to end pipeline statistics without a valid query object!" );
}
//--------------------------------------------------------------------------------
std::wstring RendererDX11::PrintPipelineStatistics( )
{
	D3D11_QUERY_DATA_PIPELINE_STATISTICS QueryData; // This data type is different depending on the query type

	if ( S_OK == m_pContext->GetData( m_pQuery, &QueryData, sizeof(QueryData), 0) )
	{
		std::wstringstream s;
		s << "Pipeline Statistics:" << std::endl;
		s << "Number of vertices read by the IA: " << QueryData.IAVertices << std::endl;
		s << "Number of primitives read by the IA: " << QueryData.IAPrimitives << std::endl;
		s << "Number of vertex shader invocations: " << QueryData.VSInvocations << std::endl;
		s << "Number of hull shader invocations: " << QueryData.HSInvocations << std::endl;
		s << "Number of domain shader invocations: " << QueryData.DSInvocations << std::endl;
		s << "Number of geometry shader invocations: " << QueryData.GSInvocations << std::endl;
		s << "Number of primitives output by the geometry shader: " << QueryData.GSPrimitives << std::endl;
		s << "Number of primitives sent to the rasterizer: " << QueryData.CInvocations << std::endl;
		s << "Number of primitives rendered: " << QueryData.CPrimitives << std::endl;
		s << "Number of pixel shader invocations: " << QueryData.PSInvocations << std::endl;

		return( s.str() );
	}

	return( std::wstring( L"Statistics currently not available..." ) );
}
//--------------------------------------------------------------------------------
void RendererDX11::SaveTextureScreenShot( int index, std::wstring filename, D3DX11_IMAGE_FILE_FORMAT format )
{
	// Get the index from the handle
	int TYPE	= index & 0x00FF0000;
	int ID		= index & 0x0000FFFF;

	if ( ( ID >= 0 ) && ( ID < m_vResources.count() ) )
	{
		// Increment the file number for allowing a sequence of images to be stored...
		static int iScreenNum = 100000;
		iScreenNum++;

		// Build the output file name
		std::wstringstream out;
		out << filename << iScreenNum;

		// Select the appropriate format to add the extension to the name.  DDS format
		// may be the only one that can handle certain texture formats, so it may be a 
		// backup format later on.

		switch ( format )
		{
		case D3DX11_IFF_BMP:
			out << ".bmp";
			break;
		case D3DX11_IFF_JPG:
			out << ".jpg";
			break;
		case D3DX11_IFF_PNG:
			out << ".png";
			break;
		case D3DX11_IFF_DDS:
			out << ".dds";
			break;
		case D3DX11_IFF_TIFF:
			out << ".tiff";
			break;
		case D3DX11_IFF_GIF:
			out << ".gif";
			break;
		case D3DX11_IFF_WMP:
			out << ".wmp";
			break;
		default:
			Log::Get().Write( L"Tried to save a texture image in an unsupported format!" );
		}

		// Get the texture as a resource and save it to file
		ID3D11Resource* pResource = m_vResources[ID]->GetResource();

		HRESULT hr = D3DX11SaveTextureToFile(
		  m_pContext,
		  pResource,
		  format,
		  out.str().c_str()
		);

		if ( FAILED( hr ) )
			Log::Get().Write( L"D3DX11SaveTextureToFile has failed!" );
	}
}
//--------------------------------------------------------------------------------
int RendererDX11::LoadTexture( std::wstring filename )
{
	ID3D11Resource* pTexture = 0;

	HRESULT hr = D3DX11CreateTextureFromFile(
		m_pDevice,
		filename.c_str(),
		0,
		0,
		&pTexture,
		0
	);

	if ( FAILED( hr ) )
	{
		Log::Get().Write( L"Failed to load texture from file!" );
		return( -1 );
	}

	m_vResources.add( new Texture2dDX11( reinterpret_cast< ID3D11Texture2D* >( pTexture ) ) );

	return( m_vResources.count() - 1 + RT_TEXTURE2D );
}
//--------------------------------------------------------------------------------
void RendererDX11::UnbindShader( ShaderType type )
{
	switch( type )
	{
	case VERTEX_SHADER:
		m_pContext->VSSetShader( 0, 0, 0 );
		break;
	case HULL_SHADER:
		m_pContext->HSSetShader( 0, 0, 0 );
		break;
	case DOMAIN_SHADER:
		m_pContext->DSSetShader( 0, 0, 0 );
		break;
	case GEOMETRY_SHADER:
		m_pContext->GSSetShader( 0, 0, 0 );
		break;
	case PIXEL_SHADER:
		m_pContext->PSSetShader( 0, 0, 0 );
		break;
	case COMPUTE_SHADER:
		m_pContext->CSSetShader( 0, 0, 0 );
		break;
	}
}
//--------------------------------------------------------------------------------
//void RendererDX11::UnBindConstantBufferParameters( ShaderType type, UINT start, UINT number )
//{
//	for ( UINT index = start; index < number - start; index++ )
//	{
//		ID3D11Buffer* pBuffer = 0;
//
//		switch( type )
//		{
//		case VERTEX_SHADER:
//			m_pContext->VSSetConstantBuffers( index, 1, &pBuffer );
//			break;
//		case HULL_SHADER:
//			m_pContext->HSSetConstantBuffers( index, 1, &pBuffer );
//			break;
//		case DOMAIN_SHADER:
//			m_pContext->DSSetConstantBuffers( index, 1, &pBuffer );
//			break;
//		case GEOMETRY_SHADER:
//			m_pContext->GSSetConstantBuffers( index, 1, &pBuffer );
//			break;
//		case PIXEL_SHADER:
//			m_pContext->PSSetConstantBuffers( index, 1, &pBuffer );
//			break;
//		case COMPUTE_SHADER:
//			m_pContext->CSSetConstantBuffers( index, 1, &pBuffer );
//			break;
//		}
//	}
//}
////--------------------------------------------------------------------------------
//void RendererDX11::UnbindShaderResourceParameter( ShaderType type, UINT start, UINT number )
//{
//	for ( UINT index = start; index < number - start; index++ )
//	{
//		ID3D11ShaderResourceView* pResourceView = 0;
//		switch( type )
//		{
//		case VERTEX_SHADER:
//			m_pContext->VSSetShaderResources( index, 1, &pResourceView );
//			break;
//		case HULL_SHADER:
//			m_pContext->HSSetShaderResources( index, 1, &pResourceView );
//			break;
//		case DOMAIN_SHADER:
//			m_pContext->DSSetShaderResources( index, 1, &pResourceView );
//			break;
//		case GEOMETRY_SHADER:
//			m_pContext->GSSetShaderResources( index, 1, &pResourceView );
//			break;
//		case PIXEL_SHADER:
//			m_pContext->PSSetShaderResources( index, 1, &pResourceView );
//			break;
//		case COMPUTE_SHADER:
//			m_pContext->CSSetShaderResources( index, 1, &pResourceView );
//			break;
//		}
//	}
//}
////--------------------------------------------------------------------------------
//void RendererDX11::UnbindUnorderedAccessParameter( ShaderType type, UINT start, UINT number )
//{
//	for ( UINT index = start; index < number - start; index++ )
//	{
//		ID3D11UnorderedAccessView* pResourceView = 0;
//		switch( type )
//		{
//		case COMPUTE_SHADER:
//			m_pContext->CSSetUnorderedAccessViews( index, 1, &pResourceView, 0 );
//			break;
//		}
//	}
//}
//--------------------------------------------------------------------------------
void RendererDX11::UnbindInputLayout( )
{
	m_pContext->IASetInputLayout( 0 );
}
//--------------------------------------------------------------------------------
void RendererDX11::UnbindVertexBuffer( )
{
	// TODO: Add the ability to unbind multiple vertex buffers at once!
	ID3D11Buffer* Buffers = { 0 };
	UINT Strides = { 0 };
	UINT Offsets = { 0 };

	m_pContext->IASetVertexBuffers( 0, 1, &Buffers, &Strides, &Offsets );
}
//--------------------------------------------------------------------------------
void RendererDX11::UnbindIndexBuffer( )
{
	m_pContext->IASetIndexBuffer( 0, DXGI_FORMAT_R32_UINT, 0 );
}
//--------------------------------------------------------------------------------
void RendererDX11::UnbindRenderTargets( )
{
	// TODO: Add the ability to unbind multiple render target views!

	ID3D11RenderTargetView* pRenderTarget = { 0 };
	ID3D11DepthStencilView* pDepthStencilView = 0;

	m_pContext->OMSetRenderTargets( 1, &pRenderTarget, pDepthStencilView );
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

	boost::shared_ptr<BlendStateDX11> ptr( new BlendStateDX11( pState ) );
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

	boost::shared_ptr<DepthStencilStateDX11> ptr( new DepthStencilStateDX11( pState ) );
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

	boost::shared_ptr<RasterizerStateDX11> ptr( new RasterizerStateDX11( pState ) );
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
void RendererDX11::SetBlendState( int ID )
{
	if ( ( ID >= 0 ) && ( ID < m_vBlendStates.count() ) )
	{
		ID3D11BlendState* pBlendState = m_vBlendStates[ID]->m_pState;
		float afBlendFactors[] = { 1.0f, 1.0f, 1.0f, 1.0f };
		m_pContext->OMSetBlendState( pBlendState, afBlendFactors, 0xFFFFFFFF );
	}
	else
	{
		Log::Get().Write( L"Tried to set an invalid blend state ID!" );
	}
}
//--------------------------------------------------------------------------------
void RendererDX11::SetDepthStencilState( int ID )
{
	if ( ( ID >= 0 ) && ( ID < m_vDepthStencilStates.count() ) )
	{
		ID3D11DepthStencilState* pDepthState = m_vDepthStencilStates[ID]->m_pState;
		m_pContext->OMSetDepthStencilState( pDepthState, 0 );
	}
	else
	{
		Log::Get().Write( L"Tried to set an invalid depth stencil state ID!" );
	}
}
//--------------------------------------------------------------------------------
void RendererDX11::SetRasterizerState( int ID )
{
	if ( ( ID >= 0 ) && ( ID < m_vRasterizerStates.count() ) )
	{
		ID3D11RasterizerState* pRasterizerState = m_vRasterizerStates[ID]->m_pState;
		m_pContext->RSSetState( pRasterizerState );
	}
	else
	{
		Log::Get().Write( L"Tried to set an invalid rasterizer state ID!" );
	}
}
//--------------------------------------------------------------------------------
void RendererDX11::SetViewPort( int ID )
{
	if ( ( ID >= 0 ) && ( ID < m_vViewPorts.count() ) )
		m_pContext->RSSetViewports( 1, &m_vViewPorts[ID]->m_ViewPort );
	else
		Log::Get().Write( L"Tried to set an invalid view port index!" );
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
int RendererDX11::GetSwapChainRenderTargetViewID( int ID )
{
	if ( ( ID >= 0 ) && ( m_vSwapChains.count() ) )
		return( m_vSwapChains[ID]->m_iViewID );
	
	Log::Get().Write( L"Tried to get an invalid swap buffer index render target view ID!" );
	return( -1 );
}
//--------------------------------------------------------------------------------
int RendererDX11::GetSwapChainTextureID( int ID )
{
	if ( ( ID >= 0 ) && ( m_vSwapChains.count() ) )
		return( m_vSwapChains[ID]->m_iTextureID );
	
	Log::Get().Write( L"Tried to get an invalid swap buffer index texture ID!" );
	return( -1 );
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
	return( Vector2f( desc.DesktopCoordinates.right - desc.DesktopCoordinates.left,
						desc.DesktopCoordinates.bottom - desc.DesktopCoordinates.top ) );
}
//--------------------------------------------------------------------------------
void RendererDX11::CopySubresourceRegion( int DestResource, UINT DstSubresource, UINT DstX, UINT DstY, UINT DstZ,
	int SrcResource, UINT SrcSubresource, D3D11_BOX* pSrcBox )
{
	int DestType = DestResource & 0x00FF0000;
	int DestID = DestResource & 0x0000FFFF;
	ID3D11Resource* pDestResource = m_vResources[DestID]->GetResource();


	int SrcType = SrcResource & 0x00FF0000;
	int SrcID = SrcResource & 0x0000FFFF;
	ID3D11Resource* pSrcResource = m_vResources[SrcID]->GetResource();

	m_pContext->CopySubresourceRegion( pDestResource, DstSubresource, DstX, DstY, DstZ,
		pSrcResource, SrcSubresource, pSrcBox );
}
//--------------------------------------------------------------------------------