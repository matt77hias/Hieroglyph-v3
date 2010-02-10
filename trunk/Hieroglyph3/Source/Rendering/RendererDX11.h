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
// RendererDX11
// 
//--------------------------------------------------------------------------------
#ifndef RendererDX11_h
#define RendererDX11_h
//--------------------------------------------------------------------------------
#include "IRenderer.h"
#include "TArray.h"
#include <boost/smart_ptr/shared_ptr.hpp>

#include <initguid.h>
#include <string>
#include <map>

#pragma comment( lib, "d3d11.lib" )
#pragma comment( lib, "d3dx11.lib" )
#pragma comment( lib, "d3dx10.lib" )
#pragma comment( lib, "DXGI.lib" )

#include <dxgi.h>
#include <d3d11.h>
#include <d3dCompiler.h>
#include <d3dx11.h>
#include <D3DX10core.h>
#include <D3D11Shader.h>

#include "Vector4f.h"
#include "Matrix4f.h"

#include "ViewRenderParams.h"
#include "VertexStageDX11.h"
#include "HullStageDX11.h"
#include "DomainStageDX11.h"
#include "GeometryStageDX11.h"
#include "PixelStageDX11.h"
#include "ComputeStageDX11.h"

#define SAFE_RELEASE( x ) {if(x){(x)->Release();(x)=NULL;}}
#define SAFE_DELETE( x ) {if(x){delete (x);(x)=NULL;}}

//--------------------------------------------------------------------------------

namespace Glyph3
{
	class VertexBufferDX11;
	class IndexBufferDX11;
	class ConstantBufferDX11;
	class StructuredBufferDX11;
	class ByteAddressBufferDX11;
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

	class GeometryDX11;

	class ShaderResourceViewDX11;
	class RenderTargetViewDX11;
	class DepthStencilViewDX11;
	class UnorderedAccessViewDX11;

	class ShaderDX11;
	class InputLayoutDX11;
	class BlendStateDX11;
	class DepthStencilStateDX11;
	class RasterizerStateDX11;
	class SamplerStateDX11;

	class BlendStateConfigDX11;
	class DepthStencilStateConfigDX11;
	class RasterizerStateConfigDX11;

	class VertexShaderDX11;
	class HullShaderDX11;
	class DomainShaderDX11;
	class GeometryShaderDX11;
	class PixelShaderDX11;
	class ComputeShaderDX11;
	class RenderEffectDX11;

	class RenderParameterDX11;

	enum ResourceType
	{
		RT_VERTEXBUFFER = 0x010000,
		RT_INDEXBUFFER = 0x020000,
		RT_CONSTANTBUFFER = 0x030000,
		RT_STRUCTUREDBUFFER = 0x040000,
		RT_BYTEADDRESSBUFFER = 0x050000,
		RT_TEXTURE1D = 0x060000,
		RT_TEXTURE2D = 0x070000,
		RT_TEXTURE3D = 0x080000
	};
	
	class RendererDX11 : public IRenderer
	{
	public:
		RendererDX11();
		virtual ~RendererDX11();

		// Access to the renderer.  There should only be a single instance
		// of the renderer at any given time.

		static RendererDX11* Get();

		// Provide the feature level of the current machine.  This can be 
		// called before or after the device has been created.

		D3D_FEATURE_LEVEL GetAvailableFeatureLevel( D3D_DRIVER_TYPE DriverType );

		// Renderer initialization and shutdown methods.  These methods
		// obtain and release all of the hardware specific resources that
		// are used during rendering.

		virtual bool Initialize( D3D_DRIVER_TYPE DriverType, D3D_FEATURE_LEVEL FeatureLevel );
		virtual void Shutdown();

		// These methods provide rendering frame control.  They are closely
		// related to the API for sequencing rendering batches.

		virtual void Present( HWND hWnd = 0, int SwapChain = -1 );
		virtual void ClearBuffers( Vector4f color, float depth = 1.0f );

		// Allow the application to create swap chains

		int CreateSwapChain( SwapChainConfigDX11* pConfig );

		// The create buffer method provides a way to create any of the buffer
		// structures - vertex, index, and constant buffers.  These all utilize the
		// same ID3D11Buffer interface, and only vary with a flag in the description.

		int CreateVertexBuffer( BufferConfigDX11* pConfig,  D3D11_SUBRESOURCE_DATA* pData );
		int CreateIndexBuffer( BufferConfigDX11* pConfig,  D3D11_SUBRESOURCE_DATA* pData );
		int CreateConstantBuffer( BufferConfigDX11* pConfig,  D3D11_SUBRESOURCE_DATA* pData );
		int CreateStructuredBuffer( BufferConfigDX11* pConfig,  D3D11_SUBRESOURCE_DATA* pData );
		int CreateByteAddressBuffer( BufferConfigDX11* pConfig,  D3D11_SUBRESOURCE_DATA* pData );

		int CreateTexture1D( Texture1dConfigDX11* pConfig, D3D11_SUBRESOURCE_DATA* pData ); 
		int CreateTexture2D( Texture2dConfigDX11* pConfig, D3D11_SUBRESOURCE_DATA* pData ); 
		int CreateTexture3D( Texture3dConfigDX11* pConfig, D3D11_SUBRESOURCE_DATA* pData ); 

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

		int CreateInputLayout( TArray<D3D11_INPUT_ELEMENT_DESC>& elements, int ShaderID  );

		// The individual states available in the D3D11 pipeline can be created here.  They
		// are referred to by index after being created.

		int CreateBlendState( BlendStateConfigDX11* pConfig );
		int CreateDepthStencilState( DepthStencilStateConfigDX11* pConfig );
		int CreateRasterizerState( RasterizerStateConfigDX11* pConfig );
		int CreateSamplerState( D3D11_SAMPLER_DESC* pDesc );
		int CreateViewPort( D3D11_VIEWPORT viewport );

		// The individual state blocks can be set for use in rendering.

		void SetBlendState( int ID );
		void SetDepthStencilState( int ID );
		void SetRasterizerState( int ID );
		void SetViewPort( int ID );

		// Each programmable shader stage can be loaded from file, and stored in a list for
		// later use.  Either an application can directly set these values or a render effect
		// can encapsulate the entire pipeline configuration.

		int LoadShader( ShaderType type, std::wstring& filename, std::wstring& function, std::wstring& model );

		// Here we allow rendering parameters to be registered and set by name.  These 
		// parameters are then available for setting their value and subsequent use 
		// during rendering.

		void SetVectorParameter( std::wstring name, Vector4f* pVector );
		void SetMatrixParameter( std::wstring name, Matrix4f* pMatrix );
		void SetShaderResourceParameter( std::wstring name, int* pID );
		void SetUnorderedAccessParameter( std::wstring name, int* pID );
		void SetConstantBufferParameter( std::wstring name, int* pID );
		void SetSamplerParameter( std::wstring name, int* pID );

		// Each of the parameter types can also be accessed to inspect their current
		// value prior to setting them.

		Vector4f GetVectorParameter( std::wstring name );
		Matrix4f GetMatrixParameter( std::wstring name );
		int GetShaderResourceParameter( std::wstring name );
		int GetUnorderedAccessParameter( std::wstring name );
		int GetConstantBufferParameter( std::wstring name );
		int GetSamplerStateParameter( std::wstring name );

		// All of the 'Bind/Unbind' functions below are used to bind various resources to the
		// pipeline.  Currently only the CS can accept unordered access views.  A method is
		// provided to apply the resource changes as an optimization, which allows for the
		// renderer to cache resource changes between rendering calls if appropriate.

		void BindConstantBufferParameter( ShaderType type, std::wstring name, UINT slot );
		void BindShaderResourceParameter( ShaderType type, std::wstring name, UINT slot );
		void BindUnorderedAccessParameter( ShaderType type, std::wstring name, UINT slot );
		void BindSamplerStateParameter( ShaderType type, std::wstring name, UINT slot );
		
		// The changes made to the pipeline resources with the Bind calls are cached and 
		// applied or cleared all at once with the following calls.  This reduces the number 
		// of API function calls needed to configure the pipeline.

		void ApplyPipelineResources( );
		void ClearPipelineResources( );

		void BindInputLayout( int ID );
		void UnbindInputLayout( );

		void BindVertexBuffer( int ID, UINT stride );
		void UnbindVertexBuffer( );

		void BindIndexBuffer( int ID );
		void UnbindIndexBuffer( );

		void BindRenderTargets( int RenderID, int DepthID );
		void UnbindRenderTargets( );

		void BindShader( ShaderType type, int ID );
		void UnbindShader( ShaderType type );

		// This function allows a client of the renderer to specify the stride
		// of the vertices within a particular vertex buffer.  This may become
		// unneeded later on if the IA configuration can be used to determine
		// the stride automatically.

		void SetVertexBufferStride( int ID, UINT stride );

		// Resources can be mapped in order to manually modify or read their
		// contents.  The returned structure provides information about the
		// resource including the pitch and width to be used in accessing it.

		D3D11_MAPPED_SUBRESOURCE	MapResource( int index, UINT subresource, D3D11_MAP actions, UINT flags );
		void						UnMapResource( int index, UINT subresource );

		
		// Pipeline execution calls - these are the methods for executing the 
		// pipeline with the given configuration (supplied by the render effect).
		// With the dispatch call, the same configuration is used except that you
		// specify the dimensions of the thread groups to instantiate.

		void Draw( RenderEffectDX11& effect, GeometryDX11& chunk );
		void Dispatch( RenderEffectDX11& effect, UINT x, UINT y, UINT z );

		// Pipeline statistics are available through the query objects in D3D11.
		// Call start, do some rendering, call end, and then print the results to
		// a string and either log them or display them.

		void StartPipelineStatistics( );
		void EndPipelineStatistics( );
		std::wstring PrintPipelineStatistics( );

		void SaveTextureScreenShot( int ID, std::wstring filename, D3DX11_IMAGE_FILE_FORMAT format = D3DX11_IFF_PNG );
		int GetSwapChainRenderTargetViewID( int ID );
		
		int LoadTexture( std::wstring filename );

		// These functions are used to convert the enumerated values to human readable text.
		// This is convenient for logging or displaying various states.

		static const char* TranslateShaderVariableClass( D3D10_SHADER_VARIABLE_CLASS varclass );
		static const char* TranslateShaderVariableType( D3D10_SHADER_VARIABLE_TYPE vartype );
		static const char* TranslateSystemVariableName( D3D10_NAME name );
		static const char* TranslateRegisterComponentType( D3D10_REGISTER_COMPONENT_TYPE type );
		static const char* TranslateCBufferType( D3D11_CBUFFER_TYPE type );
		static const char* TranslateShaderInputType( D3D10_SHADER_INPUT_TYPE type );
		static const char* TranslateResourceReturnType( D3D11_RESOURCE_RETURN_TYPE type );

		static std::wstring Print_D3D11_SHADER_DESC( D3D11_SHADER_DESC description );
		static std::wstring Print_D3D10_PRIMITIVE_TOPOLOGY( D3D10_PRIMITIVE_TOPOLOGY topology );
		static std::wstring Print_D3D10_PRIMITIVE( D3D10_PRIMITIVE primitive );
		static std::wstring Print_D3D11_PRIMITIVE( D3D11_PRIMITIVE primitive );
		static std::wstring Print_D3D11_TESSELLATOR_OUTPUT_PRIMITIVE( D3D11_TESSELLATOR_OUTPUT_PRIMITIVE primitive );
		static std::wstring Print_D3D11_TESSELLATOR_PARTITIONING( D3D11_TESSELLATOR_PARTITIONING partitioning );
		static std::wstring Print_D3D11_TESSELLATOR_DOMAIN( D3D11_TESSELLATOR_DOMAIN domain );

	private:

		// The main API interfaces used in the renderer.
		ID3D11Device*				m_pDevice;
		ID3D11Debug*				m_pDebugger;
		D3D_DRIVER_TYPE				m_driverType;
		ID3D11DeviceContext*		m_pContext;
		ID3D11Query*				m_pQuery;

		// Static renderer access - used for accessing the renderer when no reference
		// is already available.

		static RendererDX11*				m_spRenderer;

		// In general, all resources and API objects are housed in expandable arrays
		// wrapper objects.  The position within the array is used to provide fast
		// random access to the renderer clients.

		TArray<SwapChainDX11*>				m_vSwapChains;

		// Resource allocation containers are stored in an expandable array, which
		// provides fast random access with indices.

		TArray<ResourceDX11*>				m_vResources;

		// Resource view containers.  These are indexed by the application for
		// the various pipeline binding operations.

		TArray<ShaderResourceViewDX11*>		m_vShaderResourceViews;
		TArray<RenderTargetViewDX11*>		m_vRenderTargetViews;
		TArray<DepthStencilViewDX11*>		m_vDepthStencilViews;
		TArray<UnorderedAccessViewDX11*>	m_vUnorderedAccessViews;

		// The shader programs are stored in an expandable array of their base classes.

		TArray<ShaderDX11*>					m_vShaders;
		
		// These states are stored as shared pointers to the object.  This is the direction
		// that the renderer is heading in - eventually references to the objects will be
		// returned instead of indices.  This will eliminate issues with creating and
		// destroying many resources, and allow the renderer clients to have greater access
		// the objects without querying the renderer.

		TArray<boost::shared_ptr<BlendStateDX11>>				m_vBlendStates;
		TArray<boost::shared_ptr<DepthStencilStateDX11>>		m_vDepthStencilStates;
		TArray<boost::shared_ptr<RasterizerStateDX11>>			m_vRasterizerStates;

		TArray<InputLayoutDX11*>								m_vInputLayouts;
		TArray<SamplerStateDX11*>								m_vSamplerStates;
		TArray<ViewPortDX11*>									m_vViewPorts;

		// All rendering parameters are stored in a map and are accessed by name.  This may
		// be modified to allow faster random access in the future, possibly using a hash 
		// table to reference an array.

		std::map< std::wstring, RenderParameterDX11* >	m_Parameters;

	protected:

		// The shader stage resources are managed by these classes.

		VertexStageDX11		VertexShaderStage;
		HullStageDX11		HullShaderStage;
		DomainStageDX11		DomainShaderStage;
		GeometryStageDX11	GeometryShaderStage;
		PixelStageDX11		PixelShaderStage;
		ComputeStageDX11	ComputeShaderStage;

		// Each of the shader stages are stored in array that is indexed by the shader
		// type enumeration for fast access.

		ShaderStageDX11*	ShaderStages[6];

		// Internal API for friends to use - only the rendering system should use these!
		
		VertexBufferDX11* GetVertexBuffer( int index );
		IndexBufferDX11* GetIndexBuffer( int index );

		friend GeometryDX11;
	};
};

#endif // RendererDX11_h