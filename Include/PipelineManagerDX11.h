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
// PipelineManagerDX11
// 
//--------------------------------------------------------------------------------
#ifndef PipelineManagerDX11_h
#define PipelineManagerDX11_h
//--------------------------------------------------------------------------------
#include "VertexStageDX11.h"
#include "HullStageDX11.h"
#include "DomainStageDX11.h"
#include "GeometryStageDX11.h"
#include "StreamOutputStageDX11.h"
#include "RasterizerStageDX11.h"
#include "PixelStageDX11.h"
#include "ComputeStageDX11.h"
#include "InputAssemblerStageDX11.h"
#include "OutputMergerStageDX11.h"
#include "GeometryDX11.h"
#include "RenderEffectDX11.h"
#include "ResourceProxyDX11.h"
#include "ResourceDX11.h"
//--------------------------------------------------------------------------------
namespace Glyph3
{

	class CommandListDX11;

	typedef Microsoft::WRL::ComPtr<ID3DUserDefinedAnnotation> UserDefinedAnnotationComPtr;

	class PipelineManagerDX11
	{
	public:
		PipelineManagerDX11();
		~PipelineManagerDX11();

		// Allow the creator to set the context and feature level

		void SetDeviceContext( DeviceContextComPtr pContext, D3D_FEATURE_LEVEL level );

		// All of the 'Bind/Unbind' functions below are used to bind various resources to the
		// pipeline.  Currently only the CS can accept unordered access views.  A method is
		// provided to apply the resource changes as an optimization, which allows for the
		// renderer to cache resource changes between rendering calls if appropriate.

		void BindConstantBufferParameter( ShaderType type, RenderParameterDX11* pParam, UINT slot, IParameterManager* pParamManager );
		void BindShaderResourceParameter( ShaderType type, RenderParameterDX11* pParam, UINT slot, IParameterManager* pParamManager );
		void BindUnorderedAccessParameter( ShaderType type, RenderParameterDX11* pParam, UINT slot, IParameterManager* pParamManager );
		void BindSamplerStateParameter( ShaderType type, RenderParameterDX11* pParam, UINT slot, IParameterManager* pParamManager );

		void BindShader( ShaderType type, int ID, IParameterManager* pParamManager );


		// The pipeline state is managed through the following methods.  The user must 
		// configure each pipeline stage as they want it, and then utilize these Apply
		// and Clear methods to manage when the state is actually sent to the API.
		//
		// The groups of state are split roughly into the following groups:
		// 1. Input Resources: These determine the input configuration of the pipeline.
		// 2. Pipeline Resources: The state for all processing done inside the pipeline.
		// 3. Render Targets: The state for receiving the output of the pipeline.
		//
		// The changes made to the pipeline resources with the Bind calls are cached and 
		// applied or cleared all at once with these calls.  This reduces the number 
		// of API function calls needed to configure the pipeline.

		void ApplyInputResources();
		void ClearInputResources();
		
		void ApplyPipelineResources();
		void ClearPipelineResources();

		void ClearRenderTargets();	
		void ApplyRenderTargets();
		
		void ClearPipelineState();
		void ClearPipelineSRVs();


		// Pipeline execution calls - these are the methods for executing the 
		// pipeline with the given configuration (supplied by the render effect).
		// With the dispatch call, the same configuration is used except that you
		// specify the dimensions of the thread groups to instantiate.
		
		void Draw( RenderEffectDX11& effect, GeometryPtr chunk, IParameterManager* pParamManager );
		void Draw( RenderEffectDX11& effect, ResourcePtr vb, ResourcePtr ib,
					int inputLayout, D3D11_PRIMITIVE_TOPOLOGY primType,
					UINT vertexStride, UINT numIndices, IParameterManager* pParamManager);
        void DrawNonIndexed( RenderEffectDX11& effect, ResourcePtr vb, int inputLayout, 
                    D3D11_PRIMITIVE_TOPOLOGY primType, UINT vertexStride, UINT vertexCount, 
                    UINT startVertexLocation, IParameterManager* pParamManager);
		void DrawInstanced( RenderEffectDX11& effect, GeometryPtr chunk,
							ResourcePtr instanceData, UINT instanceDataStride,
							UINT numInstances, IParameterManager* pParamManager );
		void DrawInstanced( RenderEffectDX11& effect, ResourcePtr vb,
							D3D11_PRIMITIVE_TOPOLOGY primType, ResourcePtr ib,
							int inputLayout, UINT vertexStride, UINT numIndices,
							ResourcePtr instanceData, UINT instanceDataStride,
							UINT numInstances, IParameterManager* pParamManager );

		void Dispatch( RenderEffectDX11& effect, UINT x, UINT y, UINT z, IParameterManager* pParamManager );

		void DispatchIndirect( RenderEffectDX11& effect, ResourcePtr args, UINT offset, IParameterManager* pParamManager );
		void DrawIndirect( RenderEffectDX11& effect, ResourcePtr args, UINT offset, int inputLayout,
			D3D11_PRIMITIVE_TOPOLOGY primType, UINT vertexStride, IParameterManager* pParamManager);


		// These draw calls are thin wrappers around the basic API draw calls.  They are intended
		// to be used by the PipelineExecutorDX11 subclasses to invoke the pipeline.

		void Draw( UINT VertexCount, UINT StartVertex );
		void DrawIndexed( UINT IndexCount, UINT StartIndex, int VertexOffset ); 
		void DrawIndexedInstanced( UINT IndexCountPerInstance, UINT InstanceCount, UINT StartIndexLocation, INT BaseVertexLocation, UINT StartInstanceLocation );
		void DrawInstancedIndirect( ID3D11Buffer* argsBuffer, UINT offset );



		// The pipeline state can be modified with command lists.  This allows
		// the renderer to generate a command list on a deferred context, and
		// then utilize the list to perform rendering on an immediate context.

		void GenerateCommandList( CommandListDX11* pList );
		void ExecuteCommandList( CommandListDX11* pList );


		//--------------------------------------------------------------------------------------
		// Each of the following methods are used to modify resources in some way.  The user is
		// required to understand the implications of using these methods on both immediate
		// contexts and deferred contexts - this means for immediate that the command is carried
		// out more or less right away (placed in the instruction queue of the driver anyways)
		// while the deferred context command goes into a command list to be executed later on.
		// Please check to make sure what you are trying to do will work in both contexts!!!
		//--------------------------------------------------------------------------------------

		// Clear buffers does what it says - the currently bound render target views and depth
		// stencil view are cleared to the provided values.

		void ClearBuffers( Vector4f color, float depth = 1.0f, UINT stencil = 0 );
		
		// Resources can be mapped in order to manually modify or read their contents.  The 
		// returned structure provides information about the resource including the pitch and
		// width to be used in accessing it.

		D3D11_MAPPED_SUBRESOURCE	MapResource( int index, UINT subresource, D3D11_MAP actions, UINT flags );
		void						UnMapResource( int index, UINT subresource );

		D3D11_MAPPED_SUBRESOURCE	MapResource( ResourcePtr pGlyphResource, UINT subresource, D3D11_MAP actions, UINT flags );
		void						UnMapResource( ResourcePtr pGlyphResource, UINT subresource );

		D3D11_MAPPED_SUBRESOURCE	MapResource( ResourceDX11* pGlyphResource, UINT subresource, D3D11_MAP actions, UINT flags );
		void						UnMapResource( ResourceDX11* pGlyphResource, UINT subresource );


		// This is an alternative method to mapping for updating resources.  In certain 
		// situations one method may or may not be more efficient than the other, so it is
		// worth trying both to see which is more performant in a given situation.

		void UpdateSubresource( int rid, UINT DstSubresource, const D3D11_BOX *pDstBox, const void *pSrcData,
			UINT SrcRowPitch, UINT SrcDepthPitch );

		// Copy from one resource to another resource.  Check the documentation for restrictions
		// if you get errors when performing the copying.

		void CopySubresourceRegion( ResourcePtr DestResource, UINT DstSubresource, UINT DstX, UINT DstY, UINT DstZ,
			ResourcePtr SrcResource, UINT SrcSubresource, D3D11_BOX* pSrcBox );

		void CopyResource( ResourcePtr DestResource, ResourcePtr SrcResource );

        // Resolve a subresource from a MSAA texture to a non-MSAA texture.
        
        void ResolveSubresource ( ResourcePtr DestResource, UINT DstSubresource, 
                                    ResourcePtr SrcResource, UINT SrcSubresource,
                                    DXGI_FORMAT format );

		// Copy the internal counter variable from a UAV (either from Append/Consume 
		// functionality, or directly a StructuredBuffer internal counter) to the specified
		// output buffer.

		void CopyStructureCount( ResourcePtr dest, UINT offset, ResourcePtr uav );



		// Pipeline statistics are available through the query objects in D3D11.
		// Call start, do some rendering, call end, and then print the results to
		// a string and either log them or display them.

		void StartPipelineStatistics( );
		void EndPipelineStatistics( );
		std::wstring PrintPipelineStatistics( );


		// Provide the ability to set markers and event notifications for use in a debugging
		// tool such as the Graphics Debugger in VS2012.

		void BeginEvent( std::wstring& name );
		void EndEvent();
		void SetMarker( std::wstring& name );


		void SaveTextureScreenShot( int ID, std::wstring filename );

		ID3D11DeviceContext* GetDeviceContext();


		D3D_FEATURE_LEVEL						m_FeatureLevel;
		
		DeviceContextComPtr			            m_pContext;
		UserDefinedAnnotationComPtr				m_pAnnotation;
		
        static const int                        NumQueries = 3;
        int                                     m_iCurrentQuery;
        QueryComPtr					            m_Queries[NumQueries];
		D3D11_QUERY_DATA_PIPELINE_STATISTICS	m_PipelineStatsData;

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

		InputAssemblerStageDX11	InputAssemblerStage;
		StreamOutputStageDX11	StreamOutputStage;
		RasterizerStageDX11		RasterizerStage;
		OutputMergerStageDX11	OutputMergerStage;

	};
};

#endif // PipelineManagerDX11_h
