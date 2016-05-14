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
// RendererDX11
//
//--------------------------------------------------------------------------------
#ifndef RendererDX11_h
#define RendererDX11_h
//--------------------------------------------------------------------------------
#include "PCH.h"

#include "TConfiguration.h"

#include "Vector2f.h"
#include "Vector3f.h"
#include "Vector4f.h"
#include "Matrix4f.h"

#include "ResourceProxyDX11.h"
#include "ShaderDX11.h"
//--------------------------------------------------------------------------------
namespace Glyph3
{
	class BufferDX11;
	class VertexBufferDX11;
	class IndexBufferDX11;
	class ConstantBufferDX11;
	class StructuredBufferDX11;
	class ByteAddressBufferDX11;
	class IndirectArgsBufferDX11;
	class Texture1dDX11;
	class Texture2dDX11;
	class Texture3dDX11;
	class ResourceDX11;
	class SwapChainDX11;
	class ViewPortDX11;

	class BufferConfigDX11;
	class Texture1dConfigDX11;
	class Texture2dConfigDX11;
	class Texture3dConfigDX11;
	class SwapChainConfigDX11;

	class CommandListDX11;
	class GeometryDX11;

	class ShaderResourceViewDX11;
	class RenderTargetViewDX11;
	class DepthStencilViewDX11;
	class UnorderedAccessViewDX11;

	class BlendStateConfigDX11;
	class DepthStencilStateConfigDX11;
	class RasterizerStateConfigDX11;

	class RenderEffectDX11;

	class IParameterManager;
	class PipelineManagerDX11;

	class Task;

	typedef Microsoft::WRL::ComPtr<ID3D11DeviceContext> DeviceContextComPtr;
	typedef Microsoft::WRL::ComPtr<ID3D11Query> QueryComPtr;

	typedef Microsoft::WRL::ComPtr<ID3D11Buffer> BufferComPtr;
	typedef Microsoft::WRL::ComPtr<ID3D11Texture1D> Texture1DComPtr;
	typedef Microsoft::WRL::ComPtr<ID3D11Texture2D> Texture2DComPtr;
	typedef Microsoft::WRL::ComPtr<ID3D11Texture3D> Texture3DComPtr;

	typedef Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> ShaderResourceViewComPtr;
	typedef Microsoft::WRL::ComPtr<ID3D11DepthStencilView> DepthStencilViewComPtr;
	typedef Microsoft::WRL::ComPtr<ID3D11RenderTargetView> RenderTargetViewComPtr;
	typedef Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> UnorderedAccessViewComPtr;

	typedef Microsoft::WRL::ComPtr<ID3D11BlendState> BlendStateComPtr;
	typedef Microsoft::WRL::ComPtr<ID3D11DepthStencilState> DepthStencilStateComPtr;
	typedef Microsoft::WRL::ComPtr<ID3D11RasterizerState> RasterizerStateComPtr;
	typedef Microsoft::WRL::ComPtr<ID3D11SamplerState> SamplerStateComPtr;
	typedef Microsoft::WRL::ComPtr<ID3D11InputLayout> InputLayoutComPtr;

	enum ResourceType
	{
		RT_VERTEXBUFFER = 0x010000,
		RT_INDEXBUFFER = 0x020000,
		RT_CONSTANTBUFFER = 0x030000,
		RT_STRUCTUREDBUFFER = 0x040000,
		RT_BYTEADDRESSBUFFER = 0x050000,
		RT_INDIRECTARGSBUFFER = 0x060000,
		RT_TEXTURE1D = 0x070000,
		RT_TEXTURE2D = 0x080000,
		RT_TEXTURE3D = 0x090000
	};
	struct ThreadPayLoad
	{
		int id;
		bool bComplete;
		PipelineManagerDX11* pPipeline;
		IParameterManager* pParamManager;
		Task* pTask;
		CommandListDX11* pList;
	};




	class RendererDX11
	{
	public:
		RendererDX11();
		~RendererDX11();

		// Access to the renderer.  There should only be a single instance
		// of the renderer at any given time.

		static RendererDX11* Get();

		// Provide the feature level of the current machine.  This can be
		// called before or after the device has been created.

		D3D_FEATURE_LEVEL GetAvailableFeatureLevel( D3D_DRIVER_TYPE DriverType );
		D3D_FEATURE_LEVEL GetCurrentFeatureLevel();

		// Provide an estimate of the available video memory.

		UINT64 GetAvailableVideoMemory();

		// Renderer initialization and shutdown methods.  These methods
		// obtain and release all of the hardware specific resources that
		// are used during rendering.

		bool Initialize( D3D_DRIVER_TYPE DriverType, D3D_FEATURE_LEVEL FeatureLevel );
		void Shutdown();

		// These methods provide rendering frame control.  They are closely
		// related to the API for sequencing rendering batches.

		void Present( HWND hWnd = 0, int SwapChain = -1, UINT SyncInterval = 0, UINT PresentFlags = 0 );

		// Allow the application to create swap chains

		int CreateSwapChain( SwapChainConfigDX11* pConfig );// ResourceManagerDX11

		// The create buffer method provides a way to create any of the buffer
		// structures - vertex, index, and constant buffers.  These all utilize the
		// same ID3D11Buffer interface, and only vary with a flag in the description.

		ResourcePtr CreateVertexBuffer( BufferConfigDX11* pConfig,  D3D11_SUBRESOURCE_DATA* pData );
		ResourcePtr CreateIndexBuffer( BufferConfigDX11* pConfig,  D3D11_SUBRESOURCE_DATA* pData );
		ResourcePtr CreateConstantBuffer( BufferConfigDX11* pConfig,  D3D11_SUBRESOURCE_DATA* pData, bool bAutoUpdate = true );
		ResourcePtr CreateStructuredBuffer( BufferConfigDX11* pConfig,  D3D11_SUBRESOURCE_DATA* pData );
		ResourcePtr CreateByteAddressBuffer( BufferConfigDX11* pConfig,  D3D11_SUBRESOURCE_DATA* pData );
		ResourcePtr CreateIndirectArgsBuffer( BufferConfigDX11* pConfig, D3D11_SUBRESOURCE_DATA* pData );

		ResourcePtr CreateTexture1D( Texture1dConfigDX11* pConfig, D3D11_SUBRESOURCE_DATA* pData,
                                        ShaderResourceViewConfigDX11* pSRVConfig = NULL,
                                        RenderTargetViewConfigDX11* pRTVConfig = NULL,
                                        UnorderedAccessViewConfigDX11* pUAVConfig = NULL );
		ResourcePtr CreateTexture2D( Texture2dConfigDX11* pConfig, D3D11_SUBRESOURCE_DATA* pData,
                                        ShaderResourceViewConfigDX11* pSRVConfig = NULL,
                                        RenderTargetViewConfigDX11* pRTVConfig = NULL,
                                        UnorderedAccessViewConfigDX11* pUAVConfig = NULL,
                                        DepthStencilViewConfigDX11* pDSVConfig = NULL );
		ResourcePtr CreateTexture3D( Texture3dConfigDX11* pConfig, D3D11_SUBRESOURCE_DATA* pData,
                                        ShaderResourceViewConfigDX11* pSRVConfig = NULL,
                                        RenderTargetViewConfigDX11* pRTVConfig = NULL,
                                        UnorderedAccessViewConfigDX11* pUAVConfig = NULL );

		void DeleteResource( ResourcePtr ptr );
		void DeleteResource( int index );

		// The resources created in the above function calls can only be accessed by
		// the rendering pipeline when they are bound with resource views.  The following
		// functions provide the interface for creating the views, and returns an index
		// for the application to use during rendering setup.

		int CreateShaderResourceView( int ResourceID, D3D11_SHADER_RESOURCE_VIEW_DESC* pDesc );
		int CreateRenderTargetView( int ResourceID, D3D11_RENDER_TARGET_VIEW_DESC* pDesc );
		int CreateDepthStencilView( int ResourceID, D3D11_DEPTH_STENCIL_VIEW_DESC* pDesc );
		int CreateUnorderedAccessView( int ResourceID, D3D11_UNORDERED_ACCESS_VIEW_DESC* pDesc );

		// The input layout is required to exactly match the input signature of the first
		// programmable stage that will receive the input assembler output.  This method is
		// provided for creating an input layout and later indexing the value when setting
		// the appropriate vertex shader.

		int CreateInputLayout( std::vector<D3D11_INPUT_ELEMENT_DESC>& elements, int ShaderID  );

		// The individual states available in the D3D11 pipeline can be created here.  They
		// are referred to by index after being created.

		int CreateBlendState( BlendStateConfigDX11* pConfig );
		int CreateDepthStencilState( DepthStencilStateConfigDX11* pConfig );
		int CreateRasterizerState( RasterizerStateConfigDX11* pConfig );
		int CreateSamplerState( D3D11_SAMPLER_DESC* pDesc );
		int CreateViewPort( D3D11_VIEWPORT viewport );


		// Each programmable shader stage can be loaded from file, and stored in a list for
		// later use.  Either an application can directly set these values or a render effect
		// can encapsulate the entire pipeline configuration.

		int LoadShader( ShaderType type, std::wstring& filename, std::wstring& function,
			std::wstring& model, bool enablelogging = true );

        int LoadShader( ShaderType type, std::wstring& filename, std::wstring& function,
            std::wstring& model, const D3D_SHADER_MACRO* pDefines, bool enablelogging = true );
		
		ResourcePtr GetSwapChainResource( int ID );

		// This is an interim solution to get the resolution of the current
		// adapter output resolution.

		Vector2f GetDesktopResolution();

		ResourcePtr LoadTexture( std::wstring filename, bool sRGB = false );
		ResourcePtr LoadTexture( void* pData, SIZE_T sizeInBytes );
		
		// NOTE: This method is likely to be removed in the near future, since there should not
		//       be any situations where there is a raw texture pointer being given to the renderer!
		ResourcePtr LoadTexture( ID3D11Texture2D* pTexture );

		// These methods are used to take an existing texture resource and to resize it.  This
		// will recreate the texture and its associated views with the exact same configuration
		// except that the size will be updated.

		void ResizeTexture( ResourcePtr texture, UINT width, UINT height );
		void ResizeTextureSRV( int RID, int SRVID, UINT width, UINT height );
		void ResizeTextureRTV( int RID, int RTVID, UINT width, UINT height );
		void ResizeTextureDSV( int RID, int DSVID, UINT width, UINT height );
		void ResizeTextureUAV( int RID, int UAVID, UINT width, UINT height );
		void ResizeSwapChain( int ID, UINT width, UINT height );
		void ResizeViewport( int ID, UINT width, UINT height );

		// Provide access to the pipeline states.
		BlendStateComPtr								GetBlendState( int index );
		DepthStencilStateComPtr							GetDepthState( int index );
		RasterizerStateComPtr							GetRasterizerState( int index );
		const ViewPortDX11&								GetViewPort( int index );

		InputLayoutComPtr								GetInputLayout( int index );
		SamplerStateComPtr								GetSamplerState( int index );

		ShaderDX11*										GetShader( int index );

		// Here is the caching API functions
		void QueueTask( Task* pTask );
		void ProcessTaskQueue( );

		// This method is here for allowing easy integration with other libraries
		// which require access to the device.  Do not use this interface to create 
		// objects unless those objects are then registered with this renderer class!!!
		// NOTE: This method is likely to be removed in the near future!

		ID3D11Device* GetDevice();

	protected:

		// The main API interfaces used in the renderer.
		Microsoft::WRL::ComPtr<ID3D11Device>	m_pDevice;
		Microsoft::WRL::ComPtr<ID3D11Debug>		m_pDebugger;
		D3D_DRIVER_TYPE							m_driverType;

		// Static renderer access - used for accessing the renderer when no reference
		// is already available.

		static RendererDX11*					m_spRenderer;

		// In general, all resources and API objects are housed in expandable arrays
		// wrapper objects.  The position within the array is used to provide fast
		// random access to the renderer clients.

		std::vector<SwapChainDX11*>				m_vSwapChains;

		// Resource allocation containers are stored in an expandable array, which
		// provides fast random access with indices.

		std::vector<ResourceDX11*>				m_vResources;

		// Resource view containers.  These are indexed by the application for
		// the various pipeline binding operations.

		std::vector<ShaderResourceViewDX11>		m_vShaderResourceViews;
		std::vector<RenderTargetViewDX11>		m_vRenderTargetViews;
		std::vector<DepthStencilViewDX11>		m_vDepthStencilViews;
		std::vector<UnorderedAccessViewDX11>	m_vUnorderedAccessViews;

		// The shader programs are stored in an expandable array of their base classes.

		std::vector<ShaderDX11*>				m_vShaders;
		
		// These states are stored as shared pointers to the object.  This is the direction
		// that the renderer is heading in - eventually references to the objects will be
		// returned instead of indices.  This will eliminate issues with creating and
		// destroying many resources, and allow the renderer clients to have greater access
		// the objects without querying the renderer.

		std::vector<BlendStateComPtr>			m_vBlendStates;
		std::vector<DepthStencilStateComPtr>	m_vDepthStencilStates;
		std::vector<RasterizerStateComPtr>		m_vRasterizerStates;

		std::vector<InputLayoutComPtr>			m_vInputLayouts;
		std::vector<SamplerStateComPtr>			m_vSamplerStates;
		std::vector<ViewPortDX11>				m_vViewPorts;

	public:
		IParameterManager*						m_pParamMgr;
		PipelineManagerDX11*					pImmPipeline;

		ResourceDX11*				GetResourceByIndex( int index );

		// Texture resource accessors
		
		Texture1dDX11*				GetTexture1DByIndex( int rid );
		Texture2dDX11*				GetTexture2DByIndex( int rid );
		Texture3dDX11*				GetTexture3DByIndex( int rid );
		SwapChainDX11*				GetSwapChainByIndex( int sid );

		// Buffer resource accessors

		BufferDX11*					GetGenericBufferByIndex( int rid );
		ConstantBufferDX11*			GetConstantBufferByIndex( int rid );
		VertexBufferDX11*			GetVertexBufferByIndex( int rid );
		IndexBufferDX11*			GetIndexBufferByIndex( int rid );
		ByteAddressBufferDX11*		GetByteAddressBufferByIndex( int rid );
		IndirectArgsBufferDX11*		GetIndirectArgsBufferByIndex( int rid );
		StructuredBufferDX11*		GetStructuredBufferByIndex( int rid );

		// Resource view accessors

		RenderTargetViewDX11&		GetRenderTargetViewByIndex( int rid );
		DepthStencilViewDX11&		GetDepthStencilViewByIndex( int rid );
		ShaderResourceViewDX11&		GetShaderResourceViewByIndex( int rid );
		UnorderedAccessViewDX11&	GetUnorderedAccessViewByIndex( int rid );

		TConfiguration<bool>		MultiThreadingConfig;

	protected:

		int							GetUnusedResourceIndex();
		int							StoreNewResource( ResourceDX11* pResource );

		D3D_FEATURE_LEVEL			m_FeatureLevel;

		std::vector<Task*>			m_vQueuedTasks;

		friend GeometryDX11;
	};
};

unsigned int WINAPI _TaskThreadProc( void* lpParameter );

// Multithreading support objects
extern HANDLE						g_aThreadHandles[NUM_THREADS];
extern Glyph3::ThreadPayLoad		g_aPayload[NUM_THREADS];
extern HANDLE						g_aBeginEventHandle[NUM_THREADS];
extern HANDLE						g_aEndEventHandle[NUM_THREADS];

template <class T>
void LogObjectVector( std::vector<T> objects );
template <class T>
void LogObjectPtrVector( std::vector<T> objects );

#endif // RendererDX11_h