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
// PipelineManagerDX11
// 
//--------------------------------------------------------------------------------
#ifndef PipelineManagerDX11_h
#define PipelineManagerDX11_h
//--------------------------------------------------------------------------------
#include "RendererDX11.h"
#include "VertexStageDX11.h"
#include "HullStageDX11.h"
#include "DomainStageDX11.h"
#include "GeometryStageDX11.h"
#include "PixelStageDX11.h"
#include "ComputeStageDX11.h"
//--------------------------------------------------------------------------------
namespace Glyph3
{

	class CommandListDX11;

	class PipelineManagerDX11
	{
	public:
		PipelineManagerDX11();
		~PipelineManagerDX11();

		// Allow the creator to set the context and feature level

		void SetDeviceContext( ID3D11DeviceContext* pContext, D3D_FEATURE_LEVEL level );

		// The individual state blocks can be set for use in rendering.

		void SetBlendState( int ID );
		void SetDepthStencilState( int ID, UINT stencilRef = 0 );
		void SetRasterizerState( int ID );
		void SetViewPort( int ID );

		// State accessors
		D3D11_VIEWPORT GetCurrentViewport( );

		// All of the 'Bind/Unbind' functions below are used to bind various resources to the
		// pipeline.  Currently only the CS can accept unordered access views.  A method is
		// provided to apply the resource changes as an optimization, which allows for the
		// renderer to cache resource changes between rendering calls if appropriate.

		void BindConstantBufferParameter( ShaderType type, std::wstring name, UINT slot, ParameterManagerDX11* pParamManager );
		void BindShaderResourceParameter( ShaderType type, std::wstring name, UINT slot, ParameterManagerDX11* pParamManager );
		void BindUnorderedAccessParameter( ShaderType type, std::wstring name, UINT slot, ParameterManagerDX11* pParamManager );
		void BindSamplerStateParameter( ShaderType type, std::wstring name, UINT slot, ParameterManagerDX11* pParamManager );

		// TODO: Create objects to represent each of the fixed function stages, and 
		//       utilize this 'Clear', 'Bind', and 'Apply' paradigm.

		// Render targets are bound to the output merger stage, which is represented
		// internally as a seperate class.  The array of render targets is cached on
		// the CPU side prior to actual binding with the API.  This allows a series of
		// changes to be made to several render targerts prior to actually binding.

		void BindRenderTargets( int index, ResourcePtr RenderID );
		void BindDepthTarget( ResourcePtr DepthID );
		void ClearRenderTargets( );	
		void ApplyRenderTargets( );

		void BindShader( ShaderType type, int ID, ParameterManagerDX11* pParamManager );
		void UnbindShader( ShaderType type );

		// The changes made to the pipeline resources with the Bind calls are cached and 
		// applied or cleared all at once with the following calls.  This reduces the number 
		// of API function calls needed to configure the pipeline.

		void ApplyPipelineResources( );
		void ClearPipelineResources( );

		void BindInputLayout( int ID );
		void UnbindInputLayout( );

		void BindVertexBuffer( ResourcePtr resource, UINT stride, UINT offset = 0 );
		void BindVertexBuffers( const ResourcePtr* resources, const UINT* strides,
								const UINT* offsets, UINT startSlot, UINT numBuffers );
		void UnbindVertexBuffer( );
		void UnbindAllVertexBuffers( );

		void BindIndexBuffer( ResourcePtr resource );
		void UnbindIndexBuffer( );

		// Pipeline execution calls - these are the methods for executing the 
		// pipeline with the given configuration (supplied by the render effect).
		// With the dispatch call, the same configuration is used except that you
		// specify the dimensions of the thread groups to instantiate.

		void Draw( RenderEffectDX11& effect, GeometryDX11& chunk, ParameterManagerDX11* pParamManager );
		void Draw( RenderEffectDX11& effect, ResourcePtr vb, ResourcePtr ib,
					int inputLayout, D3D11_PRIMITIVE_TOPOLOGY primType,
					UINT vertexStride, UINT numIndices, ParameterManagerDX11* pParamManager);
		void DrawInstanced( RenderEffectDX11& effect, GeometryDX11& chunk,
							ResourcePtr instanceData, UINT instanceDataStride,
							UINT numInstances, ParameterManagerDX11* pParamManager );
		void DrawInstanced( RenderEffectDX11& effect, ResourcePtr vb,
							D3D11_PRIMITIVE_TOPOLOGY primType, ResourcePtr ib,
							int inputLayout, UINT vertexStride, UINT numIndices,
							ResourcePtr instanceData, UINT instanceDataStride,
							UINT numInstances, ParameterManagerDX11* pParamManager );
		
		void Dispatch( RenderEffectDX11& effect, UINT x, UINT y, UINT z, ParameterManagerDX11* pParamManager );

		// The pipeline state can be modified with command lists.  This allows
		// the renderer to generate a command list on a deferred context, and
		// then utilize the list to perform rendering on an immediate context.

		void GenerateCommandList( CommandListDX11* pList );
		void ExecuteCommandList( CommandListDX11* pList );

		void ClearBuffers( Vector4f color, float depth = 1.0f, UINT stencil = 0 );

		// Resources can be mapped in order to manually modify or read their
		// contents.  The returned structure provides information about the
		// resource including the pitch and width to be used in accessing it.

		D3D11_MAPPED_SUBRESOURCE	MapResource( int index, UINT subresource, D3D11_MAP actions, UINT flags );
		void						UnMapResource( int index, UINT subresource );

		// Pipeline statistics are available through the query objects in D3D11.
		// Call start, do some rendering, call end, and then print the results to
		// a string and either log them or display them.

		void StartPipelineStatistics( );
		void EndPipelineStatistics( );
		std::wstring PrintPipelineStatistics( );

		void SaveTextureScreenShot( int ID, std::wstring filename, D3DX11_IMAGE_FILE_FORMAT format = D3DX11_IFF_PNG );

		// Copy from one resource to another resource.

		void CopySubresourceRegion( ResourcePtr DestResource, UINT DstSubresource, UINT DstX, UINT DstY, UINT DstZ,
			ResourcePtr SrcResource, UINT SrcSubresource, D3D11_BOX* pSrcBox );


		D3D_FEATURE_LEVEL			m_FeatureLevel;

		ID3D11DeviceContext*		m_pContext;
		ID3D11Query*				m_pQuery;

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


		OutputMergerStageDX11	OutputMergerStage;

	};
};

#endif // PipelineManagerDX11_h
