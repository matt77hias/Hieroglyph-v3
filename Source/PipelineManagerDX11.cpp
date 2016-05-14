//--------------------------------------------------------------------------------
// This file is a portion of the Hieroglyph 3 Rendering Engine.  It is distributed
// under the MIT License, available in the root of this distribution and 
// at the following URL:
//
// http://www.opensource.org/licenses/mit-license.php
//
// Copyright (c) Jason Zink 
//--------------------------------------------------------------------------------

#include "PCH.h"
#include "PipelineManagerDX11.h"
#include "Log.h"
#include "ViewPortDX11.h"

#include "IParameterManager.h"
#include "ResourceDX11.h"

#include "RenderParameterDX11.h"
#include "VectorParameterDX11.h"
#include "MatrixParameterDX11.h"
#include "MatrixArrayParameterDX11.h"
#include "ShaderResourceParameterDX11.h"
#include "UnorderedAccessParameterDX11.h"
#include "ConstantBufferParameterDX11.h"
#include "SamplerParameterDX11.h"

#include "ShaderResourceViewDX11.h"
#include "RenderTargetViewDX11.h"
#include "DepthStencilViewDX11.h"
#include "UnorderedAccessViewDX11.h"

#include "CommandListDX11.h"
#include "GeometryDX11.h"
#include "MaterialDX11.h"

#include "VertexShaderDX11.h"
#include "HullShaderDX11.h"
#include "DomainShaderDX11.h"
#include "GeometryShaderDX11.h"
#include "PixelShaderDX11.h"
#include "ComputeShaderDX11.h"

#include "IndirectArgsBufferDX11.h"

#include "ScreenGrab.h"
#include <wincodec.h>

#include <sstream>

// NOTE:
// These d3d9 related items are only needed for the D3DPERF event definitions.  It can be removed
// once D3D11.1 is available, and replaced with ID3DUserDefinedAnnotation instead.
#include <d3d9.h>
#pragma comment( lib, "d3d9.lib" )
//--------------------------------------------------------------------------------
using namespace Glyph3;
//--------------------------------------------------------------------------------
PipelineManagerDX11::PipelineManagerDX11()
{
    m_iCurrentQuery = 0;

    ZeroMemory(&m_PipelineStatsData, sizeof(D3D11_QUERY_DATA_PIPELINE_STATISTICS));

	// Reference the shader stages for use in the binding process.

	ShaderStages[VERTEX_SHADER] = &VertexShaderStage;
	ShaderStages[HULL_SHADER] = &HullShaderStage;
	ShaderStages[DOMAIN_SHADER] = &DomainShaderStage;
	ShaderStages[GEOMETRY_SHADER] = &GeometryShaderStage;
	ShaderStages[PIXEL_SHADER] = &PixelShaderStage;
	ShaderStages[COMPUTE_SHADER] = &ComputeShaderStage;
}
//--------------------------------------------------------------------------------
PipelineManagerDX11::~PipelineManagerDX11()
{
	if( m_pContext ) m_pContext->ClearState();
	if( m_pContext ) m_pContext->Flush();
}
//--------------------------------------------------------------------------------
void PipelineManagerDX11::SetDeviceContext( DeviceContextComPtr pContext, D3D_FEATURE_LEVEL level )
{
	m_pContext = pContext;
	m_FeatureLevel = level;

	m_pAnnotation = nullptr;
	HRESULT hr = m_pContext.CopyTo( m_pAnnotation.GetAddressOf() );
	

	// For each pipeline stage object, set its feature level here so they know
	// what they can do and what they can't do.

	ShaderStages[VERTEX_SHADER]->SetFeatureLevel( level );
	ShaderStages[HULL_SHADER]->SetFeatureLevel( level );
	ShaderStages[DOMAIN_SHADER]->SetFeatureLevel( level );
	ShaderStages[GEOMETRY_SHADER]->SetFeatureLevel( level );
	ShaderStages[PIXEL_SHADER]->SetFeatureLevel( level );
	ShaderStages[COMPUTE_SHADER]->SetFeatureLevel( level );

	InputAssemblerStage.SetFeautureLevel( level );
	StreamOutputStage.SetFeautureLevel( level );
	RasterizerStage.SetFeatureLevel( level );
	OutputMergerStage.SetFeautureLevel( level );
}
//--------------------------------------------------------------------------------
void PipelineManagerDX11::BindConstantBufferParameter( ShaderType type, RenderParameterDX11* pParam, UINT slot, 
                                                      IParameterManager* pParamManager )
{
	RendererDX11* pRenderer = RendererDX11::Get();

	unsigned int tID = pParamManager->GetID();

	if ( pParam != 0 ) {

		// Check the type of the parameter
		if ( pParam->GetParameterType() == CBUFFER ) {
			ConstantBufferParameterDX11* pBuffer = reinterpret_cast<ConstantBufferParameterDX11*>( pParam );
			int ID = pBuffer->GetIndex( tID ); 

			ResourceDX11* pResource = pRenderer->GetResourceByIndex( ID );

			// Allow a range including -1 up to the number of resources
			if ( pResource || ( ID == -1 ) ) {
				// Get the resource to be set, and pass it in to the desired shader type
				
				ID3D11Buffer* pBuffer = 0;
				
				if ( ID >= 0 ) {
					pBuffer = (ID3D11Buffer*)pResource->GetResource();
				}

				ShaderStages[type]->DesiredState.ConstantBuffers.SetState( slot, pBuffer );
			} else {
				Log::Get().Write( L"Tried to set an invalid constant buffer ID!" );
			}
		} else {
			Log::Get().Write( L"Tried to set a non-constant buffer ID as a constant buffer!" );
		}
	} else {
		Log::Get().Write( L"Tried to set a non-existing parameter as a constant buffer!" );
	}
}
//--------------------------------------------------------------------------------
void PipelineManagerDX11::BindShaderResourceParameter( ShaderType type, RenderParameterDX11* pParam, UINT slot, 
                                                      IParameterManager* pParamManager )
{
	RendererDX11* pRenderer = RendererDX11::Get();

	unsigned int tID = pParamManager->GetID();

	if ( pParam != 0 ) {

		// Check the type of the parameter
		if ( pParam->GetParameterType() == SHADER_RESOURCE ) {
			ShaderResourceParameterDX11* pResource = 
				reinterpret_cast<ShaderResourceParameterDX11*>( pParam );

			int ID = pResource->GetIndex( tID ); 

			ShaderResourceViewDX11& view = pRenderer->GetShaderResourceViewByIndex( ID );
			ShaderStages[type]->DesiredState.ShaderResourceViews.SetState( slot, view.m_pShaderResourceView.Get() );

		} else {
			Log::Get().Write( L"Tried to set a non-shader resource ID as a shader resource!" );
		}
	} else {
		Log::Get().Write( L"Tried to set a non-existing parameter as a shader resource!" );
	}
}
//--------------------------------------------------------------------------------
void PipelineManagerDX11::BindUnorderedAccessParameter( ShaderType type, RenderParameterDX11* pParam, UINT slot, 
                                                       IParameterManager* pParamManager )
{
	RendererDX11* pRenderer = RendererDX11::Get();

	unsigned int tID = pParamManager->GetID();

	if ( pParam != 0 ) {

		// Check the type of the parameter
		if ( pParam->GetParameterType() == UNORDERED_ACCESS ) {

			UnorderedAccessParameterDX11* pResource = 
				reinterpret_cast<UnorderedAccessParameterDX11*>( pParam );

			int ID = pResource->GetIndex( tID ); 
			unsigned int initial = pResource->GetInitialCount( tID );

			UnorderedAccessViewDX11& view = pRenderer->GetUnorderedAccessViewByIndex( ID );

			ShaderStages[type]->DesiredState.UnorderedAccessViews.SetState( slot, view.m_pUnorderedAccessView.Get() );
			ShaderStages[type]->DesiredState.UAVInitialCounts.SetState( slot, initial );

		} else {
			Log::Get().Write( L"Tried to set a non-unordered access view ID as a unordered access view!" );
		}
	} else {
		Log::Get().Write( L"Tried to set a non-existing parameter as a unordered access view!" );
	}
}
//--------------------------------------------------------------------------------
void PipelineManagerDX11::BindSamplerStateParameter( ShaderType type, RenderParameterDX11* pParam, UINT slot, 
                                                    IParameterManager* pParamManager )
{
	RendererDX11* pRenderer = RendererDX11::Get();

	unsigned int tID = pParamManager->GetID();

	if ( pParam != 0 ) {

		// Check the type of the parameter
		if ( pParam->GetParameterType() == SAMPLER ) {

			SamplerParameterDX11* pResource = 
				reinterpret_cast<SamplerParameterDX11*>( pParam );

			int ID = pResource->GetIndex( tID ); 

			// Get the resource to be set, and pass it in to the desired shader type

			ID3D11SamplerState* pSampler = nullptr;

			if ( ID >= 0 ) {
				SamplerStateComPtr pState = pRenderer->GetSamplerState( ID );
				pSampler = pState.Get();
			}

			ShaderStages[type]->DesiredState.SamplerStates.SetState( slot, pSampler );

		} else {
			Log::Get().Write( L"Tried to set a non-sampler state ID as a sampler state!" );
		}
	} else {
		Log::Get().Write( L"Tried to set a non-existing parameter as a sampler state!" );
	}
}
//--------------------------------------------------------------------------------
void PipelineManagerDX11::ClearRenderTargets( )
{
	OutputMergerStage.ClearDesiredState();
}
//--------------------------------------------------------------------------------
void PipelineManagerDX11::ApplyRenderTargets( )
{
	OutputMergerStage.ApplyDesiredRenderTargetStates( m_pContext.Get() );
}
//--------------------------------------------------------------------------------
void PipelineManagerDX11::ApplyInputResources( )
{
	InputAssemblerStage.ApplyDesiredState( m_pContext.Get() );
}
//--------------------------------------------------------------------------------
void PipelineManagerDX11::ClearInputResources( )
{
	InputAssemblerStage.ClearDesiredState();
}
//--------------------------------------------------------------------------------
void PipelineManagerDX11::ApplyPipelineResources( )
{
	VertexShaderStage.ApplyDesiredState( m_pContext.Get() );
	HullShaderStage.ApplyDesiredState( m_pContext.Get() );
	DomainShaderStage.ApplyDesiredState( m_pContext.Get() );
	GeometryShaderStage.ApplyDesiredState( m_pContext.Get() );
	PixelShaderStage.ApplyDesiredState( m_pContext.Get() );
	ComputeShaderStage.ApplyDesiredState( m_pContext.Get() );

	// TODO: this may not be the correct place to set this state! The Rasterizer
	// state should probably be split into two parts like the OM state.

	StreamOutputStage.ApplyDesiredState( m_pContext.Get() );
	RasterizerStage.ApplyDesiredState( m_pContext.Get() );
	OutputMergerStage.ApplyDesiredBlendAndDepthStencilStates( m_pContext.Get() );
}
//--------------------------------------------------------------------------------
void PipelineManagerDX11::ClearPipelineResources( )
{
	VertexShaderStage.ClearDesiredState( );
	HullShaderStage.ClearDesiredState( );
	DomainShaderStage.ClearDesiredState( );
	GeometryShaderStage.ClearDesiredState( );
	PixelShaderStage.ClearDesiredState( );
	ComputeShaderStage.ClearDesiredState( );

	StreamOutputStage.ClearDesiredState( );
}
//--------------------------------------------------------------------------------
void PipelineManagerDX11::ClearPipelineState( )
{
	// First clear out our cached states.

	InputAssemblerStage.ClearCurrentState();
	InputAssemblerStage.ClearDesiredState();

	VertexShaderStage.ClearCurrentState();
	VertexShaderStage.ClearDesiredState();

	HullShaderStage.ClearCurrentState();
	HullShaderStage.ClearDesiredState();

	DomainShaderStage.ClearCurrentState();
	DomainShaderStage.ClearDesiredState();

	GeometryShaderStage.ClearCurrentState();
	GeometryShaderStage.ClearDesiredState();

	StreamOutputStage.ClearCurrentState();
	StreamOutputStage.ClearDesiredState();

	RasterizerStage.ClearCurrentState( );
	RasterizerStage.ClearDesiredState( );

	PixelShaderStage.ClearCurrentState();
	PixelShaderStage.ClearDesiredState();

	OutputMergerStage.ClearCurrentState();
	OutputMergerStage.ClearDesiredState();

	ComputeShaderStage.ClearCurrentState();
	ComputeShaderStage.ClearDesiredState();
	
	// After our cached states have been cleared, then we can clear the API state.

	m_pContext->ClearState();

	if ( m_pContext->GetType() == D3D11_DEVICE_CONTEXT_DEFERRED ) {
		ID3D11CommandList* pList;
		m_pContext->FinishCommandList( true, &pList );
		pList->Release();
	}
}
//--------------------------------------------------------------------------------
void PipelineManagerDX11::ClearPipelineSRVs()
{
	// Here we reset all of the shader resource view states.  This is useful for
	// ensuring that the pipeline is free from resources that may be used for 
	// output in a subsequent rendering pass.

	VertexShaderStage.DesiredState.ShaderResourceViews.InitializeStates();
	HullShaderStage.DesiredState.ShaderResourceViews.InitializeStates();
	DomainShaderStage.DesiredState.ShaderResourceViews.InitializeStates();
	GeometryShaderStage.DesiredState.ShaderResourceViews.InitializeStates();
	PixelShaderStage.DesiredState.ShaderResourceViews.InitializeStates();
	ComputeShaderStage.DesiredState.ShaderResourceViews.InitializeStates();
}
//--------------------------------------------------------------------------------
void PipelineManagerDX11::DrawIndexed( UINT IndexCount, UINT StartIndex, int VertexOffset )
{
	m_pContext->DrawIndexed( IndexCount, StartIndex, VertexOffset );
}
//--------------------------------------------------------------------------------
void PipelineManagerDX11::Draw( UINT VertexCount, UINT StartVertex )
{
	m_pContext->Draw( VertexCount, StartVertex );
}
//--------------------------------------------------------------------------------
void PipelineManagerDX11::DrawIndexedInstanced( UINT IndexCountPerInstance, UINT InstanceCount, UINT StartIndexLocation, INT BaseVertexLocation, UINT StartInstanceLocation )
{
	m_pContext->DrawIndexedInstanced( IndexCountPerInstance, InstanceCount, StartIndexLocation, BaseVertexLocation, StartInstanceLocation );
}
//--------------------------------------------------------------------------------
void PipelineManagerDX11::DrawInstancedIndirect( ID3D11Buffer* argsBuffer, UINT offset )
{
	m_pContext->DrawInstancedIndirect( argsBuffer, offset );
}
//--------------------------------------------------------------------------------
void PipelineManagerDX11::Draw( RenderEffectDX11& effect, GeometryPtr geometry, 
										IParameterManager* pParamManager )
{
	Draw( effect, geometry->m_VB, geometry->m_IB,
		geometry->GetInputLayout( effect.GetVertexShader() ), geometry->m_ePrimType,
		geometry->GetVertexSize(), geometry->GetIndexCount(), pParamManager );
}
//--------------------------------------------------------------------------------
void PipelineManagerDX11::Draw( RenderEffectDX11& effect, ResourcePtr vb, ResourcePtr ib,
						int inputLayout, D3D11_PRIMITIVE_TOPOLOGY primType,
						UINT vertexStride, UINT numIndices, IParameterManager* pParamManager )
{
	InputAssemblerStage.ClearDesiredState();

	// Configure the pipeline input data with the input assembler
	// state object.

	InputAssemblerStage.DesiredState.PrimitiveTopology.SetState( primType );
	
	// Bind the vertex and index buffers.
	if ( vb != NULL ) {
		InputAssemblerStage.DesiredState.VertexBuffers.SetState( 0, vb->m_iResource );
		InputAssemblerStage.DesiredState.VertexBufferStrides.SetState( 0, vertexStride );
		InputAssemblerStage.DesiredState.VertexBufferOffsets.SetState( 0, 0 );
	}

	if ( ib != NULL ) {
		InputAssemblerStage.DesiredState.IndexBuffer.SetState( ib->m_iResource );
		InputAssemblerStage.DesiredState.IndexBufferFormat.SetState( DXGI_FORMAT_R32_UINT );
	}

	// Bind the input layout.
	if ( vb != NULL ) {
		InputAssemblerStage.DesiredState.InputLayout.SetState( inputLayout );
	}

	// Set and apply the state in this pipeline manager's input assembler.
	InputAssemblerStage.ApplyDesiredState( m_pContext.Get() );

	// Use the effect to load all of the pipeline stages here.
	ClearPipelineResources();
	effect.ConfigurePipeline( this, pParamManager );
	ApplyPipelineResources();
	
	m_pContext->DrawIndexed( numIndices, 0, 0 );
}
//--------------------------------------------------------------------------------
void PipelineManagerDX11::DrawNonIndexed( RenderEffectDX11& effect, ResourcePtr vb, 
                                           int inputLayout, D3D11_PRIMITIVE_TOPOLOGY primType,
                                           UINT vertexStride, UINT vertexCount, UINT startVertexLocation,
                                           IParameterManager* pParamManager )
{
	InputAssemblerStage.ClearDesiredState();
	
	// Specify the type of geometry that we will be dealing with.
    InputAssemblerStage.DesiredState.PrimitiveTopology.SetState( primType );

    // Bind the vertex buffer.
	InputAssemblerStage.DesiredState.VertexBuffers.SetState( 0, vb->m_iResource );
	InputAssemblerStage.DesiredState.VertexBufferStrides.SetState( 0, vertexStride );
	InputAssemblerStage.DesiredState.VertexBufferOffsets.SetState( 0, 0 );

    // Bind the input layout.
	InputAssemblerStage.DesiredState.InputLayout.SetState( inputLayout );

	// Set and apply the state in this pipeline manager's input assembler.
	InputAssemblerStage.ApplyDesiredState( m_pContext.Get() );

    // Use the effect to load all of the pipeline stages here.
    ClearPipelineResources();
    effect.ConfigurePipeline( this, pParamManager );
    ApplyPipelineResources();

    m_pContext->Draw( vertexCount, startVertexLocation );
}
//--------------------------------------------------------------------------------
void PipelineManagerDX11::DrawInstanced( RenderEffectDX11& effect, GeometryPtr geometry,
								 ResourcePtr instanceData, UINT instanceDataStride,
								 UINT numInstances, IParameterManager* pParamManager )
{
	DrawInstanced( effect, geometry->m_VB, geometry->m_ePrimType, geometry->m_IB,
		geometry->GetInputLayout( effect.GetVertexShader() ),
		geometry->GetVertexSize(), geometry->GetIndexCount(),
		instanceData, instanceDataStride, numInstances, pParamManager );
}
//--------------------------------------------------------------------------------
void PipelineManagerDX11::DrawInstanced( RenderEffectDX11& effect, ResourcePtr vb,
								 D3D11_PRIMITIVE_TOPOLOGY primType, ResourcePtr ib,
								 int inputLayout, UINT vertexStride, UINT numIndices,
								 ResourcePtr instanceData, UINT instanceDataStride,
								 UINT numInstances, IParameterManager* pParamManager )
{
	InputAssemblerStage.ClearDesiredState();

	// Specify the type of geometry that we will be dealing with.
    InputAssemblerStage.DesiredState.PrimitiveTopology.SetState( primType );

    // Bind the vertex buffers.
	InputAssemblerStage.DesiredState.VertexBuffers.SetState( 0, vb->m_iResource );
	InputAssemblerStage.DesiredState.VertexBufferStrides.SetState( 0, vertexStride );
	InputAssemblerStage.DesiredState.VertexBufferOffsets.SetState( 0, 0 );

	InputAssemblerStage.DesiredState.VertexBuffers.SetState( 1, instanceData->m_iResource );
	InputAssemblerStage.DesiredState.VertexBufferStrides.SetState( 1, instanceDataStride );
	InputAssemblerStage.DesiredState.VertexBufferOffsets.SetState( 1, 0 );

	// Bind the index buffer.
	InputAssemblerStage.DesiredState.IndexBuffer.SetState( ib->m_iResource );
	InputAssemblerStage.DesiredState.IndexBufferFormat.SetState( DXGI_FORMAT_R32_UINT );

    // Bind the input layout.
	InputAssemblerStage.DesiredState.InputLayout.SetState( inputLayout );

	// Set and apply the state in this pipeline manager's input assembler.
	InputAssemblerStage.ApplyDesiredState( m_pContext.Get() );

	// Use the effect to load all of the pipeline stages here.

	ClearPipelineResources();
	effect.ConfigurePipeline( this, pParamManager );
	ApplyPipelineResources();

	m_pContext->DrawIndexedInstanced( numIndices, numInstances, 0, 0, 0 );
}
//--------------------------------------------------------------------------------
void PipelineManagerDX11::DispatchIndirect( RenderEffectDX11& effect,
										    ResourcePtr args,
											UINT offset,
											IParameterManager* pParamManager )
{
	// Use the effect to load all of the pipeline stages here.

	BufferDX11* pBuffer = RendererDX11::Get()->GetIndirectArgsBufferByIndex( args->m_iResource );


	if ( pBuffer == NULL ) 	{
		Log::Get().Write( L"Tried to use the wrong resource type for an indirect dispatch!" );
		return;
	}

	ID3D11Buffer* pArgsBuffer = (ID3D11Buffer*)pBuffer->GetResource();


	ClearPipelineResources();
	effect.ConfigurePipeline( this, pParamManager );
	ApplyPipelineResources();

	m_pContext->DispatchIndirect( pArgsBuffer, offset );

}
//--------------------------------------------------------------------------------
void PipelineManagerDX11::DrawIndirect( RenderEffectDX11& effect,
									    ResourcePtr args,
										UINT offset,
										int inputLayout,
										D3D11_PRIMITIVE_TOPOLOGY primType,
										UINT vertexStride,
										IParameterManager* pParamManager )
{
	IndirectArgsBufferDX11* pBuffer = RendererDX11::Get()->GetIndirectArgsBufferByIndex( args->m_iResource );

	if ( !pBuffer ) {
		Log::Get().Write( L"Failure to get indirect args buffer, aborting call..." );
		return;
	} 

	ID3D11Buffer* pArgsBuffer = reinterpret_cast<ID3D11Buffer*>( pBuffer->GetResource() );

	InputAssemblerStage.ClearDesiredState();

	// Specify the type of geometry that we will be dealing with.
    InputAssemblerStage.DesiredState.PrimitiveTopology.SetState( primType );

    // Bind the input layout.
	InputAssemblerStage.DesiredState.InputLayout.SetState( inputLayout );

	// Set and apply the state in this pipeline manager's input assembler.
	InputAssemblerStage.ApplyDesiredState( m_pContext.Get() );

	// Use the effect to load all of the pipeline stages here.
	ClearPipelineResources();
	effect.ConfigurePipeline( this, pParamManager );
	ApplyPipelineResources();
	
	m_pContext->DrawInstancedIndirect( pArgsBuffer, offset );

}
//--------------------------------------------------------------------------------
void PipelineManagerDX11::Dispatch( RenderEffectDX11& effect, UINT x, UINT y, UINT z, IParameterManager* pParamManager )
{
	// Use the effect to load all of the pipeline stages here.

	ClearPipelineResources();
	effect.ConfigurePipeline( this, pParamManager );
	ApplyPipelineResources();

	m_pContext->Dispatch( x, y, z );
}
//--------------------------------------------------------------------------------
void PipelineManagerDX11::CopyStructureCount( ResourcePtr dest, UINT offset, ResourcePtr uav )
{
	int id = dest->m_iResource;

	BufferDX11* pArgsBuffer = RendererDX11::Get()->GetGenericBufferByIndex( id );
	ID3D11Buffer* pRawArgsBuffer = 0;

	if ( pArgsBuffer ) {
		pRawArgsBuffer = (ID3D11Buffer*)pArgsBuffer->GetResource(); // TODO: add ID3D11Buffer accessor to the buffer resource classes!
	}

	UnorderedAccessViewDX11& view = RendererDX11::Get()->GetUnorderedAccessViewByIndex( uav->m_iResourceUAV );
	ID3D11UnorderedAccessView* pRawView = 0;

	if ( pArgsBuffer != nullptr ) {
		m_pContext->CopyStructureCount( pRawArgsBuffer, offset, view.m_pUnorderedAccessView.Get() );
	} else {
		Log::Get().Write( L"ERROR: Trying to copy structure count with null values!" );
	}
}
//--------------------------------------------------------------------------------
void PipelineManagerDX11::ClearBuffers( Vector4f color, float depth, UINT stencil )
{
	// Get the current render target view and depth stencil view from the OM stage.
    ID3D11RenderTargetView* pRenderTargetViews[D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT] = { NULL };
	ID3D11DepthStencilView* pDepthStencilView = 0;

	// Our output merger class manages the current render targets bound to the pipeline.
	// The number of views that are currently set refers to the views that the application
	// has submitted to the pipeline, but not necessarily that have been bound to the API.
	// For each view that the application thinks is bound, we clear it as requested.  Since
	// the view count is the number of non-null views, we don't need to check here.

	UINT viewCount = OutputMergerStage.GetCurrentState().GetRenderTargetCount();

	for( UINT i = 0; i < viewCount; ++i )
	{
	    float clearColours[] = { color.x, color.y, color.z, color.w }; // RGBA
		int rtv = OutputMergerStage.GetCurrentState().RenderTargetViews.GetState( i );
		RenderTargetViewDX11& RTV = RendererDX11::Get()->GetRenderTargetViewByIndex( rtv );
		pRenderTargetViews[i] = RTV.m_pRenderTargetView.Get(); 
		if ( pRenderTargetViews[i] != nullptr ) {
			m_pContext->ClearRenderTargetView( pRenderTargetViews[i], clearColours );
		}
	}

	// Check if the output merger currently has a depth target set, and if so clear it.

	if ( OutputMergerStage.GetCurrentState().DepthTargetViews.GetState() != -1 )
	{
		int dsv = OutputMergerStage.GetCurrentState().DepthTargetViews.GetState();
		DepthStencilViewDX11 DSV = RendererDX11::Get()->GetDepthStencilViewByIndex( dsv );
		pDepthStencilView = DSV.m_pDepthStencilView.Get();
		if ( pDepthStencilView != nullptr ) {
			m_pContext->ClearDepthStencilView( pDepthStencilView, D3D11_CLEAR_DEPTH, depth, stencil );
		}
	}
}
//--------------------------------------------------------------------------------
void PipelineManagerDX11::BindShader( ShaderType type, int ID, IParameterManager* pParamManager )
{
	RendererDX11* pRenderer = RendererDX11::Get();
	ShaderDX11* pShaderDX11 = pRenderer->GetShader( ID );

	// Record the shader ID for use later on
	ShaderStages[type]->DesiredState.ShaderProgram.SetState( ID );

	// Check if the shader has a valid identifier
	if ( pShaderDX11 )
	{
		// Perform the actual binding to the pipeline, and then bind all needed
		// parameters.

		if ( pShaderDX11->GetType() == type )
		{
			// Before binding the shader, have it update its required parameters.  These
			// parameters will then be bound to the pipeline after the shader is bound.

			pShaderDX11->GetReflection()->BindParameters( type, this, pParamManager );
		}
		else
		{
			Log::Get().Write( L"Tried to set the wrong type of shader ID!" );
		}
	}
}
//--------------------------------------------------------------------------------
D3D11_MAPPED_SUBRESOURCE PipelineManagerDX11::MapResource( int rid, UINT subresource, D3D11_MAP actions, UINT flags )
{
	// Acquire the engine's resource wrapper.
	ResourceDX11* pGlyphResource = 0; 
	pGlyphResource = RendererDX11::Get()->GetResourceByIndex( rid );

	return( MapResource( pGlyphResource, subresource, actions, flags ) );
}
//--------------------------------------------------------------------------------
void PipelineManagerDX11::UnMapResource( int rid, UINT subresource )
{
	// Acquire the engine's resource wrapper.
	ResourceDX11* pGlyphResource = 0; 
	pGlyphResource = RendererDX11::Get()->GetResourceByIndex( rid );

	UnMapResource( pGlyphResource, subresource );
}
//--------------------------------------------------------------------------------
D3D11_MAPPED_SUBRESOURCE PipelineManagerDX11::MapResource( ResourcePtr pResource, UINT subresource, D3D11_MAP actions, UINT flags )
{
	// Acquire the engine's resource wrapper.
	ResourceDX11* pGlyphResource = 0; 
	pGlyphResource = RendererDX11::Get()->GetResourceByIndex( pResource->m_iResource );

	return( MapResource( pGlyphResource, subresource, actions, flags ) );
}
//--------------------------------------------------------------------------------
void PipelineManagerDX11::UnMapResource( ResourcePtr pResource, UINT subresource )
{
	// Acquire the engine's resource wrapper.
	ResourceDX11* pGlyphResource = 0; 
	pGlyphResource = RendererDX11::Get()->GetResourceByIndex( pResource->m_iResource );

	UnMapResource( pGlyphResource, subresource );
}
//--------------------------------------------------------------------------------
D3D11_MAPPED_SUBRESOURCE PipelineManagerDX11::MapResource( ResourceDX11* pGlyphResource, UINT subresource, D3D11_MAP actions, UINT flags )
{
	D3D11_MAPPED_SUBRESOURCE Data;
	Data.pData = NULL;
	Data.DepthPitch = Data.RowPitch = 0;

	if ( nullptr == pGlyphResource ) {
		Log::Get().Write( L"Trying to map a subresource that doesn't exist!!!" );
		return( Data );
	}
	// TODO: Update this to use a ComPtr!
	// Acquire the native resource pointer.
	ID3D11Resource* pResource = 0;
	pResource = pGlyphResource->GetResource();

	if ( nullptr == pResource ) {
		Log::Get().Write( L"Trying to map a subresource that has no native resource in it!!!" );
		return( Data );
	}

	// Perform the mapping of the resource.
	HRESULT hr = m_pContext->Map( pResource, subresource, actions, flags, &Data );
	
	if ( FAILED( hr ) ) {
		Log::Get().Write( L"Failed to map resource!" );
	}

	return( Data );
}
//--------------------------------------------------------------------------------
void PipelineManagerDX11::UnMapResource( ResourceDX11* pGlyphResource, UINT subresource )
{
	if ( NULL == pGlyphResource ) {
		Log::Get().Write( L"Trying to unmap a subresource that doesn't exist!!!" );
		return;
	}

	// Acquire the native resource pointer.
	ID3D11Resource* pResource = 0;
	pResource = pGlyphResource->GetResource();

	if ( NULL == pResource ) {
		Log::Get().Write( L"Trying to unmap a subresource that has no native resource in it!!!" );
		return;
	}

	// Unmap the resource - there is no HRESULT returned, so trust that it works...
	m_pContext->Unmap( pResource, subresource );
}
//--------------------------------------------------------------------------------
void PipelineManagerDX11::UpdateSubresource( int rid, UINT DstSubresource, const D3D11_BOX *pDstBox, const void *pSrcData, UINT SrcRowPitch, UINT SrcDepthPitch )
{
	// Acquire the engine's resource wrapper.
	ResourceDX11* pGlyphResource = 0; 
	pGlyphResource = RendererDX11::Get()->GetResourceByIndex( rid );

	if ( NULL == pGlyphResource ) {
		Log::Get().Write( L"Trying to update a subresource that doesn't exist!!!" );
		return;
	}

	// Acquire the native resource pointer.
	ID3D11Resource* pResource = 0;
	pResource = pGlyphResource->GetResource();

	if ( NULL == pResource ) {
		Log::Get().Write( L"Trying to update a subresource that has no native resource in it!!!" );
		return;
	}

	// Perform the update of the resource.
	m_pContext->UpdateSubresource( pResource, DstSubresource, pDstBox, pSrcData, SrcRowPitch, SrcDepthPitch );
}
//--------------------------------------------------------------------------------
void PipelineManagerDX11::StartPipelineStatistics( )
{
	if ( m_Queries[m_iCurrentQuery] )            
		m_pContext->Begin( m_Queries[m_iCurrentQuery].Get() );    
	else
		Log::Get().Write( L"Tried to begin pipeline statistics without a query object!" );
}
//--------------------------------------------------------------------------------
void PipelineManagerDX11::EndPipelineStatistics( )
{
	if ( m_Queries[m_iCurrentQuery] )
	{        
		m_pContext->End( m_Queries[m_iCurrentQuery].Get() );
     
        m_iCurrentQuery = ( m_iCurrentQuery + 1 ) % NumQueries;
        HRESULT hr = m_pContext->GetData( m_Queries[m_iCurrentQuery].Get(), &m_PipelineStatsData, 
                                            sizeof(D3D11_QUERY_DATA_PIPELINE_STATISTICS), 0);
        if ( FAILED( hr ) )
            Log::Get().Write( L"Failed attempting to retrieve query data" );        
	}
	else
	{
		Log::Get().Write( L"Tried to end pipeline statistics without a valid query object!" );
	}
}
//--------------------------------------------------------------------------------
std::wstring PipelineManagerDX11::PrintPipelineStatistics( )
{
	std::wstringstream s;
	s << "Pipeline Statistics:" << std::endl;
	s << "Number of vertices read by the IA: " << m_PipelineStatsData.IAVertices << std::endl;
	s << "Number of primitives read by the IA: " << m_PipelineStatsData.IAPrimitives << std::endl;
	s << "Number of vertex shader invocations: " << m_PipelineStatsData.VSInvocations << std::endl;
	s << "Number of hull shader invocations: " << m_PipelineStatsData.HSInvocations << std::endl;
	s << "Number of domain shader invocations: " << m_PipelineStatsData.DSInvocations << std::endl;
	s << "Number of geometry shader invocations: " << m_PipelineStatsData.GSInvocations << std::endl;
	s << "Number of primitives output by the geometry shader: " << m_PipelineStatsData.GSPrimitives << std::endl;
	s << "Number of primitives sent to the rasterizer: " << m_PipelineStatsData.CInvocations << std::endl;
	s << "Number of primitives rendered: " << m_PipelineStatsData.CPrimitives << std::endl;
	s << "Number of pixel shader invocations: " << m_PipelineStatsData.PSInvocations << std::endl;

	return( s.str() );
}
//--------------------------------------------------------------------------------
void PipelineManagerDX11::SaveTextureScreenShot( int index, std::wstring filename )
{
	// Get the texture as a resource and save it to file
	ID3D11Resource* pResource = RendererDX11::Get()->GetResourceByIndex( index )->GetResource();

	if ( pResource )
	{
		// Increment the file number for allowing a sequence of images to be stored...
		static int iScreenNum = 100000;
		iScreenNum++;

		// Build the output file name
		std::wstringstream out;
		out << filename << iScreenNum << L".png";

		HRESULT hr = DirectX::SaveWICTextureToFile( 
			m_pContext.Get(), 
			pResource,
			GUID_ContainerFormatPng,
			out.str().c_str() );

		// Note: Use this version for WP8!
		//HRESULT hr = DirectX::SaveDDSTextureToFile( 
		//	m_pContext,
		//	pResource,
		//	out.str().c_str() );

		if ( FAILED( hr ) )
			Log::Get().Write( L"ERROR: Failed to save texture screen shot!" );
	}
}
//--------------------------------------------------------------------------------
void PipelineManagerDX11::CopySubresourceRegion( ResourcePtr DestResource, UINT DstSubresource, 
	UINT DstX, UINT DstY, UINT DstZ, ResourcePtr SrcResource, UINT SrcSubresource, D3D11_BOX* pSrcBox )
{
	ID3D11Resource* pDestResource = RendererDX11::Get()->GetResourceByIndex(DestResource->m_iResource)->GetResource();
	ID3D11Resource* pSrcResource = RendererDX11::Get()->GetResourceByIndex(SrcResource->m_iResource)->GetResource();

	m_pContext->CopySubresourceRegion( pDestResource, DstSubresource, DstX, DstY, DstZ,
		pSrcResource, SrcSubresource, pSrcBox );
}
//--------------------------------------------------------------------------------
void PipelineManagerDX11::CopyResource( ResourcePtr DestResource, ResourcePtr SrcResource )
{
	ID3D11Resource* pDestResource = RendererDX11::Get()->GetResourceByIndex(DestResource->m_iResource)->GetResource();
	ID3D11Resource* pSrcResource = RendererDX11::Get()->GetResourceByIndex(SrcResource->m_iResource)->GetResource();

	m_pContext->CopyResource( pDestResource, pSrcResource );
}
//--------------------------------------------------------------------------------
void PipelineManagerDX11::GenerateCommandList( CommandListDX11* pList )
{
	if ( m_pContext->GetType() == D3D11_DEVICE_CONTEXT_DEFERRED )
	{
		m_pContext->FinishCommandList( false, &pList->m_pList );

		// Reset the cached context state to default, since we do that for all
		// command lists.

		InputAssemblerStage.ClearCurrentState();
		InputAssemblerStage.ClearDesiredState();

		VertexShaderStage.ClearCurrentState();
		VertexShaderStage.ClearDesiredState();

		HullShaderStage.ClearCurrentState();
		HullShaderStage.ClearDesiredState();

		DomainShaderStage.ClearCurrentState();
		DomainShaderStage.ClearDesiredState();

		GeometryShaderStage.ClearCurrentState();
		GeometryShaderStage.ClearDesiredState();

		StreamOutputStage.ClearCurrentState();
		StreamOutputStage.ClearDesiredState();

		RasterizerStage.ClearCurrentState( );
		RasterizerStage.ClearDesiredState( );

		PixelShaderStage.ClearCurrentState();
		PixelShaderStage.ClearDesiredState();

		OutputMergerStage.ClearCurrentState();
		OutputMergerStage.ClearDesiredState();

		ComputeShaderStage.ClearCurrentState();
		ComputeShaderStage.ClearDesiredState();

	}
}
//--------------------------------------------------------------------------------
void PipelineManagerDX11::ExecuteCommandList( CommandListDX11* pList )
{
	if ( pList->ListAvailable() )
		m_pContext->ExecuteCommandList( pList->m_pList, false );

	InputAssemblerStage.ClearCurrentState();
	InputAssemblerStage.ClearDesiredState();

	VertexShaderStage.ClearCurrentState();
	VertexShaderStage.ClearDesiredState();

	HullShaderStage.ClearCurrentState();
	HullShaderStage.ClearDesiredState();

	DomainShaderStage.ClearCurrentState();
	DomainShaderStage.ClearDesiredState();

	GeometryShaderStage.ClearCurrentState();
	GeometryShaderStage.ClearDesiredState();

	StreamOutputStage.ClearCurrentState();
	StreamOutputStage.ClearDesiredState();

	RasterizerStage.ClearCurrentState( );
	RasterizerStage.ClearDesiredState( );

	PixelShaderStage.ClearCurrentState();
	PixelShaderStage.ClearDesiredState();

	OutputMergerStage.ClearCurrentState();
	OutputMergerStage.ClearDesiredState();

	ComputeShaderStage.ClearCurrentState();
	ComputeShaderStage.ClearDesiredState();
}
//--------------------------------------------------------------------------------
void PipelineManagerDX11::ResolveSubresource( ResourcePtr DestResource, UINT DstSubresource, 
                                              ResourcePtr SrcResource, UINT SrcSubresource, 
                                              DXGI_FORMAT format )
{
    int DestID = DestResource->m_iResource;
    ID3D11Resource* pDestResource = RendererDX11::Get()->GetResourceByIndex(DestID)->GetResource();

    int SrcID = SrcResource->m_iResource;
    ID3D11Resource* pSrcResource = RendererDX11::Get()->GetResourceByIndex(SrcID)->GetResource();
 
    m_pContext->ResolveSubresource( pDestResource, DstSubresource, pSrcResource, SrcSubresource, format );
}
//--------------------------------------------------------------------------------
void PipelineManagerDX11::BeginEvent( std::wstring& name )
{
	if ( m_pAnnotation ) {
		m_pAnnotation->BeginEvent( name.c_str() );
	} else {
		D3DPERF_BeginEvent( 0xFFFFFFFF, name.c_str() );
	}
}
//--------------------------------------------------------------------------------
void PipelineManagerDX11::EndEvent()
{
	if ( m_pAnnotation ) {
		m_pAnnotation->EndEvent();
	} else {
		D3DPERF_EndEvent();
	}
}
//--------------------------------------------------------------------------------
void PipelineManagerDX11::SetMarker( std::wstring& name )
{
	if ( m_pAnnotation ) {
		m_pAnnotation->SetMarker( name.c_str() );
	}
}
//--------------------------------------------------------------------------------
ID3D11DeviceContext* PipelineManagerDX11::GetDeviceContext()
{
	return( m_pContext.Get() );
}
//--------------------------------------------------------------------------------
